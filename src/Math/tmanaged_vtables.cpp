//============================================================================
//  TManaged<NTempest::C33Matrix> (0x6F954BFC), TManaged<NTempest::C3Vector>
//  (0x6F954BE8), TManaged<NTempest::CImVector> (0x6F954C10) - three small,
//  4-slot vtables an earlier NTempest/NIpse namespace-filtered survey
//  missed: TManaged<T>'s own class name carries no NTempest::/NIpse::
//  prefix even though its template argument does
//  (agent_worktrees/classes/0x6F954BFC.json etc, hierarchy_raw
//  "TManaged<T>: CBaseManaged;").  See docs/targets/TManagedAgileAgentQuery.md.
//
//  Misc/ccamera.cpp and Misc/csoundlistener.cpp already carry a *local*
//  `template <class T> class TManaged : public CBaseManaged` stand-in each
//  uses to placement-construct member sub-objects of this same family -
//  see either file's own header for the field derivation this reuses
//  verbatim (CBaseManaged, then m_reserved0C/m_flags/m_reserved0E[2] at
//  +0x0C-+0x0F, m_field10/m_field14 at +0x10/+0x14, m_field18 at +0x18,
//  the wrapped value at +0x1C).  Those two files never reach *this*
//  address family though - they only ever call the constructor, never any
//  of TManaged<T>'s own four out-of-line vtable slots - so nothing here
//  can be built on top of that stand-in's constructor; this reconstructs
//  the four slot bodies fresh, as plain non-virtual member functions on a
//  local struct per instantiation (the same "no vtable-stamping
//  constructor for this class has been reconstructed, so model each slot
//  as an ordinary function and restamp the base's own vtable literal by
//  hand" shape Misc/ntempest_cpmquery_scalardtors.cpp's own
//  CPmQuery_AgileAgentFilter/CPoQuery/CSeQuery already use) rather than as
//  a real C++ template with real `virtual` members.
//
//  Vtable slots, read straight off raw_asm (identical shape across all
//  three instantiations, only sizeof(T) and the comparison width differ):
//
//    0  ~TManaged<T>()  - CBaseManaged's own scalar deleting destructor,
//       unmodified: TManaged<T> adds no member with its own cleanup (T is
//       plain floats/dwords in all three cases here), so the compiler
//       emitted the identical "restamp to ??_7CBaseManaged@@6B@, unlink
//       the TSLink<CBaseManaged> membership, conditionally Storm-free"
//       body a second (third, fourth) time at this slot purely because
//       this image was linked without /OPT:ICF - the same "no real
//       vtable-stamping constructor for the derived class, so the
//       compiler's own immediate is whatever base it already had" shape
//       AgileAgentQuery::ScalarDeletingDestructor and
//       NIpse::CPoQuery/CSeQuery's own copies of CPmQuery<T>'s destructor
//       already document one level of this same pattern down.
//    1  Advance(float dt) - if a callback is registered at +0x10, calls it
//       with (userdata=+0x14, &copyOfCurrentValue, dt) and applies
//       whatever it wrote back through SetValue() (slot 3); then clears
//       flag bit 0x04 unconditionally (whether or not a callback ran).
//    2  Notify(float dt) - the identical callback invocation, but the
//       computed value is never applied back - evaluate-for-side-effect
//       only.  Same unconditional bit-0x04 clear.
//    3  SetValue(const T&) - compares the incoming value against the
//       stored one (the matrix instantiation calls out to a shared
//       9-float comparator, 0x6F4D6990 - CompareFloat9 below; the vector
//       and colour instantiations compare inline) and, only on a real
//       change, stores it and sets flag bit 0x08 ("dirty").
//
//  The callback's own real signature is not established by anything this
//  dump reaches - no cross-reference in agent_worktrees/funcs/ calls
//  Advance or Notify at all, and none of the three constructors for these
//  three vtables were found either (same "no constructor found" gap for
//  all four classes in this batch).  `void (__fastcall*)(void* userdata,
//  T* value, float dt)` is a plausible best-effort shape (matches the
//  three real arguments a fastcall dispatch to it would need: ecx, edx,
//  one stack float) but the shipped listing's own stack construction for
//  the temporary T copy mixes `push` and `mov` in a way this reconstruction
//  does not reproduce instruction-for-instruction - DIFFERS on slots 1/2
//  for that reason.  Slots 0 and 3 are the load-bearing, well-understood
//  ones and are the real target of this reconstruction.
//============================================================================
#include "storm.h"

//  The real element type of the global tracking list every CBaseManaged
//  descendant links into (storm.h's own TSLink<T> only ever dereferences
//  T* as a T*, never T's own members, so any forward-declared tag works
//  for the link math - this one just names the real list correctly).
class CBaseManaged;

namespace NTempest
{
//  `class`, not `struct` - Misc/ccamera.cpp's own copy of this note
//  applies here too: the shipped vtable symbols mangle each of these with
//  the class tag `V`, confirmed against this file's own three restamped
//  literals (??_7?$TManaged@VC33Matrix@NTempest@@@@6B@ etc).
class C33Matrix { public: float m[9]; };
class C3Vector  { public: float x, y, z; };
class CImVector { public: unsigned int m_bits; };
}

//----------------------------------------------------------------------------
//  0x6F4D6990 - TManaged<C33Matrix>::SetValue's own shared comparator: nine
//  sequential `fld`/`fld`/`fucompp`/`fnstsw`/`test ah,44h` pairs, one per
//  float, short-circuiting on the first difference (unordered counts as
//  different too, matching `test ah,44h` / `jp`, the "unordered or not
//  equal" mask the raw_asm uses throughout this whole family).
//----------------------------------------------------------------------------
static int CompareFloat9(const float* a, const float* b)
{
    //  Written unrolled, not as a loop over nine elements: the shipped
    //  body is nine straight-line fld/fld/fucompp/fnstsw/test/jp groups
    //  with no loop counter at all, and a `for` here compiles to a real
    //  loop instead of unrolling to that shape.  A single chained `||`
    //  rather than nine separate `if (...) return 1;` statements, so
    //  every mismatch short-circuits to the same shared "return 1" site
    //  the shipped code funnels every `jp` to, instead of a fresh return
    //  sequence after each compare.
    return b[0] != a[0] || b[1] != a[1] || b[2] != a[2] ||
           b[3] != a[3] || b[4] != a[4] || b[5] != a[5] ||
           b[6] != a[6] || b[7] != a[7] || b[8] != a[8];
}

//============================================================================
//  TManaged<NTempest::C33Matrix> - 0x6F954BFC.
//============================================================================
struct TManaged_C33Matrix
{
    typedef void (__fastcall *TickFn)(void* userdata, NTempest::C33Matrix* value, float dt);

    //  0x6F4D6D20.
    void* ScalarDeletingDestructor(unsigned int flags);
    //  0x6F4D6A50.
    void Advance(float dt);
    //  0x6F4D6AA0.
    void Notify(float dt);
    //  0x6F4D6A20.
    void SetValue(const NTempest::C33Matrix& value);

    void*                m_vtable;       // +0x00
    TSLink<CBaseManaged> m_link;         // +0x04
    unsigned char        m_reserved0C;   // +0x0C
    unsigned char        m_flags;        // +0x0D
    unsigned char        m_reserved0E[2];// +0x0E
    TickFn               m_tickFn;       // +0x10
    void*                m_userdata;     // +0x14
    float                m_field18;      // +0x18
    NTempest::C33Matrix  m_value;        // +0x1C
};

//  See Pathfinding/crlagent_ctor.cpp's own StampVtable for why this goes
//  through a volatile store rather than a plain literal assignment.
static __forceinline void StampVtable(void* pThis, void* vtbl)
{
    *(void* volatile*)pThis = vtbl;
}

void* TManaged_C33Matrix::ScalarDeletingDestructor(unsigned int flags)
{
    StampVtable(this, (void*)0x6F954BC4);   //  ??_7CBaseManaged@@6B@
    m_link.Unlink();

    if (flags & 1)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void TManaged_C33Matrix::Advance(float dt)
{
    if (m_tickFn)
    {
        NTempest::C33Matrix temp = m_value;
        m_tickFn(m_userdata, &temp, dt);
        SetValue(temp);
    }
    m_flags &= ~0x04;
}

void TManaged_C33Matrix::Notify(float dt)
{
    if (m_tickFn)
    {
        NTempest::C33Matrix temp = m_value;
        m_tickFn(m_userdata, &temp, dt);
    }
    m_flags &= ~0x04;
}

void TManaged_C33Matrix::SetValue(const NTempest::C33Matrix& value)
{
    if (CompareFloat9(value.m, m_value.m))
    {
        m_value = value;
        m_flags |= 0x08;
    }
}

//============================================================================
//  TManaged<NTempest::C3Vector> - 0x6F954BE8.
//============================================================================
struct TManaged_C3Vector
{
    typedef void (__fastcall *TickFn)(void* userdata, NTempest::C3Vector* value, float dt);

    //  0x6F4D6CC0.
    void* ScalarDeletingDestructor(unsigned int flags);
    //  0x6F4D6880.
    void Advance(float dt);
    //  0x6F4D68D0.
    void Notify(float dt);
    //  0x6F4D6830.
    void SetValue(const NTempest::C3Vector& value);

    void*               m_vtable;        // +0x00
    TSLink<CBaseManaged> m_link;         // +0x04
    unsigned char       m_reserved0C;    // +0x0C
    unsigned char       m_flags;         // +0x0D
    unsigned char       m_reserved0E[2]; // +0x0E
    TickFn              m_tickFn;        // +0x10
    void*               m_userdata;      // +0x14
    float               m_field18;       // +0x18
    NTempest::C3Vector  m_value;         // +0x1C
};

void* TManaged_C3Vector::ScalarDeletingDestructor(unsigned int flags)
{
    StampVtable(this, (void*)0x6F954BC4);
    m_link.Unlink();

    if (flags & 1)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void TManaged_C3Vector::Advance(float dt)
{
    if (m_tickFn)
    {
        NTempest::C3Vector temp = m_value;
        m_tickFn(m_userdata, &temp, dt);
        SetValue(temp);
    }
    m_flags &= ~0x04;
}

void TManaged_C3Vector::Notify(float dt)
{
    if (m_tickFn)
    {
        NTempest::C3Vector temp = m_value;
        m_tickFn(m_userdata, &temp, dt);
    }
    m_flags &= ~0x04;
}

void TManaged_C3Vector::SetValue(const NTempest::C3Vector& value)
{
    if (value.x != m_value.x || value.y != m_value.y || value.z != m_value.z)
    {
        m_value = value;
        m_flags |= 0x08;
    }
}

//============================================================================
//  TManaged<NTempest::CImVector> - 0x6F954C10.  CImVector is a packed
//  32-bit colour (this vtable's own SetValue compares it with a plain
//  `cmp`, not a float `fucompp` - confirmed against 0x6F4D6B10's raw_asm),
//  so m_value here is one dword, not a float.
//============================================================================
struct TManaged_CImVector
{
    typedef void (__fastcall *TickFn)(void* userdata, NTempest::CImVector* value, float dt);

    //  0x6F4D6D80.
    void* ScalarDeletingDestructor(unsigned int flags);
    //  0x6F4D6B30.
    void Advance(float dt);
    //  0x6F4D6B70.
    void Notify(float dt);
    //  0x6F4D6B10.
    void SetValue(const NTempest::CImVector& value);

    void*                m_vtable;       // +0x00
    TSLink<CBaseManaged> m_link;         // +0x04
    unsigned char        m_reserved0C;   // +0x0C
    unsigned char        m_flags;        // +0x0D
    unsigned char        m_reserved0E[2];// +0x0E
    TickFn               m_tickFn;       // +0x10
    void*                m_userdata;     // +0x14
    float                m_field18;      // +0x18
    NTempest::CImVector  m_value;        // +0x1C
};

void* TManaged_CImVector::ScalarDeletingDestructor(unsigned int flags)
{
    StampVtable(this, (void*)0x6F954BC4);
    m_link.Unlink();

    if (flags & 1)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void TManaged_CImVector::Advance(float dt)
{
    if (m_tickFn)
    {
        NTempest::CImVector temp = m_value;
        m_tickFn(m_userdata, &temp, dt);
        SetValue(temp);
    }
    m_flags &= ~0x04;
}

void TManaged_CImVector::Notify(float dt)
{
    if (m_tickFn)
    {
        NTempest::CImVector temp = m_value;
        m_tickFn(m_userdata, &temp, dt);
    }
    m_flags &= ~0x04;
}

void TManaged_CImVector::SetValue(const NTempest::CImVector& value)
{
    if (m_value.m_bits != value.m_bits)
    {
        m_value = value;
        m_flags |= 0x08;
    }
}
