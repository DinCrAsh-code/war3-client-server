//============================================================================
//  CCamera - RTTI name only (user_knowledge.json's own vtables table,
//  vtable @ 0x6F95512C, ??_7CCamera@@6B@), and unrelated to CCameraWar3
//  (gameui.h) despite the similar name: its three slot addresses
//  (0x6F4E7Cxx / 0x6F00BFxx) sit in a completely different neighbourhood
//  from CCameraWar3's own (0x6F30xxxx), and nothing here calls into or out
//  of that class.  No constructor dump exists for it this session, so only
//  the three vtable slots below are declared - no data layout is invented.
//
//  Slot 0 (0x6F4E7C10, asm/sub_6F4E7C10_0x6F4E7C10_calltree_asm.md) is the
//  familiar `DeleteSelf(unsigned int flags)` shape this repo already has on
//  TRefCnt/CAgent (refcnt.h/agent.h): call the destructor, then free the
//  object through Storm's own allocator (tagged "delete") when bit 0 of
//  `flags` is set.  Its own destructor call (sub_6F4E7A40) tears down nine
//  embedded `CBaseManaged`-vtabled sub-objects (each unlinked from its own
//  doubly-linked list first) and tail-jumps into a tenth, `CHandleObject`-
//  vtabled base's own destructor (sub_6F4D7880).  Neither `CBaseManaged`
//  nor `CHandleObject` gets a real layout here beyond the handful of bytes
//  these two destructors themselves touch - see the class declarations
//  below Method_0x08 for exactly what is and is not established.
//
//  Slot 1 (0x6F00BFD0) is a defensive "release if non-null" pattern: if
//  `this` is set, call this object's own slot 0 with a literal `1` (i.e.
//  DeleteSelf(1) - free it for real).  Slot 2 (0x6F00BFE0) is a plain
//  nullsub.
//
//  A constructor dump *does* exist now (0x6F4E7790, reached from
//  CCameraWar3's own controller-slot cluster - see
//  GameUI/controllerslotsortedinsert.cpp's own file header for the family
//  this batch closes out): it stamps exactly `??_7CCamera@@6B@` into
//  `[esi]`, confirming the class identification above beyond the RTTI-name
//  match alone.  See CCamera::CCamera below for what it does and does not
//  reproduce.
//============================================================================
#include "storm.h"
#include "cameraangle.h"

class CCamera
{
public:
    //  slot 0 / +0x00 (0x6F4E7C10) - see the file header.  `retn 4`.
    virtual CCamera* DeleteSelf(unsigned int flags);
    //  slot 1 / +0x04 (0x6F00BFD0) - `if (this) this->DeleteSelf(1);`.
    //  `retn 0`.
    virtual void ReleaseIfSelf();
    //  slot 2 / +0x08 (0x6F00BFE0) - `return 0;`.  `retn 0`.
    virtual int Method_0x08();

    //  0x6F4E7A40 - the real destructor: nine embedded CBaseManaged-vtabled
    //  members, each unlinked from its own list before being re-stamped to
    //  CBaseManaged's own vtable, then a tail jump into CHandleObject's own
    //  destructor (sub_6F4D7880).  `retn 0`.  See CCameraDestruct.cpp.
    void Destruct();

    //  0x6F4E7790 - see below.
    CCamera();
};

//============================================================================
//  0x6F4E7790 - CCamera::CCamera().
//
//  The real body: an inlined CDataMgr base construction (CDataMgr::CDataMgr
//  is sub_6F4D85D0, a real, separate address out of this batch's own scope
//  per Widget/cdatamgrpropertyregister.cpp's own note - small enough that
//  /Ob2 inlines it at this one call site anyway), then nine embedded
//  CBaseManaged-vtabled sub-objects constructed in address order - two
//  TManaged<NTempest::C3Vector>, three TManaged<float>, four CAngle
//  (Misc/cameraangle.cpp, real calls to 0x6F4E76C0) - then all nine handed
//  to CDataMgr::RegisterManagedProperty through the two SManagedPropertyRegistrar
//  wrappers Widget/cdatamgrpropertyregister.cpp already reconstructs
//  (kind 3 for the two vectors at slots 7/8, kind 6 for the seven scalars
//  at slots 0-6), exactly the same real calls CCameraWar3::BindControllerProps
//  makes into the *other* half of this family
//  (GameUI/camerabindprops.cpp) - this is CCamera's own equivalent
//  registration, a level below.
//
//  This carries the same `__except_handler4`-shaped SEH frame
//  Misc/cameraangle.cpp's CAngle::CAngle documents and the same toolchain
//  limitation applies (docs/msvc-vc8-idioms.md's "An `__except_handler4`-
//  shaped frame this toolchain cannot reproduce") - `DIFFERS` on the frame
//  alone, independent of the body.  The CDataMgr base's own vtable stamp is
//  not reproduced for the same dead-store reason CAngle's is not: nothing
//  reads `[this]` as CDataMgr before CCamera's own stamp overwrites it two
//  statements later.  CDataMgr is deliberately *not* modelled as a real C++
//  base of CCamera here: its own vtable (if it has one distinct from
//  CCamera's) is unestablished, and doing so would put unknown slots ahead
//  of CCamera's own three already-committed ones
//  (docs/targets/vtables/CCamera.txt) - the raw-offset writes below get the
//  same instruction shapes without touching CCamera's own inheritance or
//  vtable.  The five TManaged<T> default values (three float, two vec3) are
//  this build's own literal-pool floats - like NormalizeAngle's constants,
//  canonlib maps every unresolved data reference on both sides to `SYM`, so
//  only their values matter and only for behaviour, not score - and are not
//  independently confirmed.
//============================================================================
namespace NTempest
{
//  `class`, not `struct`: the shipped vtable symbol mangles the argument as
//  `VC3Vector@NTempest@@` (a class tag, `V`), not `U` - confirmed against
//  this constructor's own `??_7?$TManaged@VC3Vector@NTempest@@@@6B@` stamp.
class C3Vector
{
public:
    float x, y, z;
};
}

//  The generic "animatable managed value" shape controllerslot.h's own
//  SControllerSlot and Misc/cameraangle.cpp's CAngle both already document:
//  CBaseManaged, a flag byte, two reserved ints, a float, then the payload
//  itself (one T-sized value).  Declared here rather than shared: each of
//  this family's own files declares its own minimal, tailored stand-in
//  (see cameraangle.h's own note), and this is the only file that needs the
//  templated form.
template <class T>
class TManaged : public CBaseManaged
{
public:
    TManaged(float field18, const T& value)
    {
        m_reserved0C = 0;
        m_flags = 0;
        m_reserved0E[0] = 0;
        m_reserved0E[1] = 0;
        m_field10 = 0;
        m_field14 = 0;
        m_field18 = field18;
        m_value = value;
    }

    unsigned char m_reserved0C;     // +0x0C
    unsigned char m_flags;           // +0x0D
    unsigned char m_reserved0E[2];   // +0x0E
    int           m_field10;         // +0x10
    int           m_field14;         // +0x14
    float         m_field18;         // +0x18
    T             m_value;            // +0x1C
};

//  The two registration wrappers CCamera's own ctor reaches (Widget/
//  cdatamgrpropertyregister.cpp) - redeclared here as plain receiver types
//  the same way that file declares `CDataMgr` itself: no data, only the
//  thiscall signature, since nothing here constructs one for real.
struct SManagedPropertyRegistrar3 { void __thiscall Register(void*, int, unsigned int); };
struct SManagedPropertyRegistrar6 { void __thiscall Register(void*, int, unsigned int); };

CCamera::CCamera()
{
    char* p = (char*)this;

    //  CDataMgr base: field4, its own TSFixedArray<CBaseManaged*> property
    //  table (Widget/cdatamgrpropertyarray.cpp) zeroed then sized to nine
    //  slots, and the TSExplicitList<CBaseManaged,4> sentinel head
    //  (Widget/cdatamgrbasemanagedlist.cpp) reset to empty.
    *(int*)(p + 0x04) = 0;

    TSFixedArray<CBaseManaged*>* properties =
        (TSFixedArray<CBaseManaged*>*)(p + 0x08);
    properties->m_alloc = 0;
    properties->m_count = 0;
    properties->m_data = 0;
    if (properties->m_count != 9)
    {
        properties->SetAlloc(9);
        properties->m_count = 9;
    }

    TSExplicitList<CBaseManaged, 4>* list =
        (TSExplicitList<CBaseManaged, 4>*)(p + 0x14);
    list->m_linkoffset = 4;
    list->m_terminator.m_next = (TSLink<CBaseManaged>*)(p + 0x18);
    list->m_terminator.m_prevlink = ~(int)(p + 0x18);

    //  CCamera's own nine members, in address order.
    NTempest::C3Vector vecZero = { 0.0f, 0.0f, 0.0f };
    new (p + 0x20) TManaged<NTempest::C3Vector>(0.0f, vecZero);
    new (p + 0x48) TManaged<NTempest::C3Vector>(0.0f, vecZero);
    new (p + 0x70) TManaged<float>(0.0f, 0.0f);
    new (p + 0x90) TManaged<float>(0.0f, 0.0f);
    new (p + 0xB0) TManaged<float>(0.0f, 0.0f);
    new (p + 0xD0) CAngle(0.0f);
    new (p + 0xF8) CAngle(3.14159265f);
    new (p + 0x120) CAngle(0.0f);
    new (p + 0x148) CAngle(0.0f);

    ((SManagedPropertyRegistrar3*)this)->Register(p + 0x20, 7, 0);
    ((SManagedPropertyRegistrar3*)this)->Register(p + 0x48, 8, 0);
    ((SManagedPropertyRegistrar6*)this)->Register(p + 0x70, 1, 0);
    ((SManagedPropertyRegistrar6*)this)->Register(p + 0x90, 2, 0);
    ((SManagedPropertyRegistrar6*)this)->Register(p + 0xB0, 3, 0);
    ((SManagedPropertyRegistrar6*)this)->Register(p + 0xD0, 0, 0);
    ((SManagedPropertyRegistrar6*)this)->Register(p + 0xF8, 4, 0);
    ((SManagedPropertyRegistrar6*)this)->Register(p + 0x120, 5, 0);
    ((SManagedPropertyRegistrar6*)this)->Register(p + 0x148, 6, 0);
}

//  0x6F4E7C10.
CCamera* CCamera::DeleteSelf(unsigned int flags)
{
    Destruct();
    if ((flags & 1) && this)
        SMemFree(this, "delete", -1, 0);
    return this;
}

//  0x6F00BFD0.
void CCamera::ReleaseIfSelf()
{
    if (this)
        DeleteSelf(1);
}

//  0x6F00BFE0.
int CCamera::Method_0x08()
{
    return 0;
}

//  CBaseManaged itself now comes from Misc/cameraangle.h - the same
//  stand-in CAngle derives from - rather than a second, TU-local
//  redeclaration: this file needs the real one anyway to placement-construct
//  CCamera's own CAngle members below, and the vtable-stamp trick
//  Destruct() already uses (`CBaseManaged stamp; vtbl = *(void**)&stamp;`)
//  works identically whether or not the stand-in carries a TSLink member.
//
//  A member sub-object CBaseManaged-vtabled: a vtable pointer followed by
//  its own TSLink<CBaseManaged> membership in some global tracking list
//  (storm.h).  Only these first twelve bytes of each real sub-object are
//  read by Destruct() below; nothing else about its true shape is
//  established, so nothing else is declared.
struct SBaseManagedSlot
{
    void*                 m_vtable;   // +0x00
    TSLink<CBaseManaged>  m_link;     // +0x04
};

//  0x6F4D6E40 - walk the variable-length bucket table CHandleObject embeds
//  at +0x14 and unlink every entry still in it.  `ecx` is the table's own
//  address; `[ecx]` is its data pointer and `[ecx+8]` a byte offset that is
//  re-read after each unlink (so it is not a plain count - whatever
//  decrements it is inside the unlink this loop does not show).  Left as a
//  literal, instruction-for-instruction naked translation rather than a
//  guessed C++ shape: nothing in either dump says what the table's real
//  element type or true field layout is, only this walk's own branches.
//
//  Declared `__fastcall` (not the plain-C default this free function had):
//  the naked body only ever reads its argument through `ecx`, and the
//  caller's own dump has no `push`/stack cleanup around this call at all
//  (`retn` with no operand, no `add esp,N` at the call site either) - a
//  plain-stack declaration merely happened to work by coincidence (the
//  compiler pushes whatever register it just computed the argument into,
//  and that register is not clobbered before the call), but left a
//  needless `push table` / `add esp,4` pair CHandleObject::Destruct's own
//  call site does not have.
__declspec(naked) static void __fastcall UnlinkHandleObjectBuckets(void* /*table*/)
{
    __asm
    {
        push    esi
        push    edi
    loop_top:
        mov     edx, [ecx+8]
        test    edx, edx
        jle     done
        mov     eax, [ecx]
        mov     edi, [eax+edx]
        add     eax, edx
        test    edi, edi
        jz      loop_top
        mov     esi, [eax+4]
        test    esi, esi
        jg      has_pred
        not     esi
        jmp     store_prev
    has_pred:
        mov     edx, eax
        sub     edx, [edi+4]
        add     esi, edx
    store_prev:
        mov     [esi], edi
        mov     edx, [eax]
        mov     esi, [eax+4]
        mov     [edx+4], esi
        mov     dword ptr [eax], 0
        mov     dword ptr [eax+4], 0
        jmp     loop_top
    done:
        pop     edi
        pop     esi
        retn
    }
}

//  0x6F4D7880 - CHandleObject's own destructor tail: unlink every entry of
//  the +0x14 bucket table above, unlink this object's own +0x18
//  TSLink<CHandleObject> membership, free the +0x10 name string through
//  Storm (tagged "delete", the type-descriptor RTTI name) if one was ever
//  set, then stamp ??_7CHandleObject@@6B@.
//  +0x14 is the bucket table UnlinkHandleObjectBuckets walks and +0x18 is
//  this object's own TSLink<CHandleObject> membership - declared only as
//  raw offsets rather than two struct fields, because the table's own true
//  extent (whether it reaches as far as +0x18) is not established and a
//  named field would assert a boundary the dump does not.
//
//  Given the same trivial two-slot vtable shape TRefCnt/Float/Angle/
//  CBaseManaged above all share in this family, so that a plain default
//  constructor is what stamps ??_7CHandleObject@@6B@ (the InitProp
//  technique, applied to the class itself instead of a local stand-in).
struct CHandleObject
{
    virtual void ReleaseSelf() {}
    virtual void OnZeroRefCount() {}

    char   m_reserved04[0x10 - 0x04];
    char*  m_pName;      // +0x10 - freed with Storm_403

    void Destruct();
};

//  0x6F4D7880.
void CHandleObject::Destruct()
{
    //  +0x18's own address is formed as the bucket-table pointer plus 4
    //  (`lea eax,[esi+4]` off the just-computed table address), not as a
    //  fresh `this+0x18` - naming the table pointer once and indexing off
    //  it is what asks this compiler for that same reuse (the "Naming an
    //  intermediate parks it in a callee-saved register" idiom).
    void* table = (char*)this + 0x14;
    UnlinkHandleObjectBuckets(table);

    ((TSLink<CHandleObject>*)((char*)table + 4))->Unlink();

    //  0x6F4D78CB pushes `aPavcbasemanage` - IDA's own label for a
    //  `PAVCBaseManaged@@`-shaped RTTI type-descriptor string, not
    //  "delete".  It is the debug-only tag AddRef's own note in refcnt.h
    //  already documents for this family - the callee never reads it back.
    if (m_pName)
        SMemFree(m_pName, "PAVCBaseManaged@@", -2, 0);

    CHandleObject stamp;
    *(void**)this = *(void**)&stamp;
}

//  0x6F4E7A40 - the real destructor.  Nine SBaseManagedSlot sub-objects,
//  highest offset first (the shipped order), each unlinked from its own
//  list before its vtable is re-stamped to CBaseManaged's - then a tail
//  jump into CHandleObject's own destructor.
void CCamera::Destruct()
{
    CBaseManaged stamp;
    void* vtbl = *(void**)&stamp;

    //  Nine unrolled blocks, not a loop: the nine offsets are not evenly
    //  strided (0x144, 0x11C, 0xF4, 0xD0, 0xB0, 0x90, 0x70, 0x48, 0x20 -
    //  deltas of -0x28, -0x28, -0x24, -0x20, -0x20, -0x20, -0x28, -0x28),
    //  which is what a static const offset table compiled into a loop
    //  cannot reproduce.
    ((SBaseManagedSlot*)((char*)this + 0x148))->m_vtable = vtbl;
    ((SBaseManagedSlot*)((char*)this + 0x148))->m_link.Unlink();
    ((SBaseManagedSlot*)((char*)this + 0x120))->m_vtable = vtbl;
    ((SBaseManagedSlot*)((char*)this + 0x120))->m_link.Unlink();
    ((SBaseManagedSlot*)((char*)this + 0xF8))->m_vtable = vtbl;
    ((SBaseManagedSlot*)((char*)this + 0xF8))->m_link.Unlink();
    ((SBaseManagedSlot*)((char*)this + 0xD0))->m_vtable = vtbl;
    ((SBaseManagedSlot*)((char*)this + 0xD0))->m_link.Unlink();
    ((SBaseManagedSlot*)((char*)this + 0xB0))->m_vtable = vtbl;
    ((SBaseManagedSlot*)((char*)this + 0xB0))->m_link.Unlink();
    ((SBaseManagedSlot*)((char*)this + 0x90))->m_vtable = vtbl;
    ((SBaseManagedSlot*)((char*)this + 0x90))->m_link.Unlink();
    ((SBaseManagedSlot*)((char*)this + 0x70))->m_vtable = vtbl;
    ((SBaseManagedSlot*)((char*)this + 0x70))->m_link.Unlink();
    ((SBaseManagedSlot*)((char*)this + 0x48))->m_vtable = vtbl;
    ((SBaseManagedSlot*)((char*)this + 0x48))->m_link.Unlink();
    ((SBaseManagedSlot*)((char*)this + 0x20))->m_vtable = vtbl;
    ((SBaseManagedSlot*)((char*)this + 0x20))->m_link.Unlink();

    ((CHandleObject*)this)->Destruct();
}
