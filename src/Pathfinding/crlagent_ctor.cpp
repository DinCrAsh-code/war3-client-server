//============================================================================
//  0x6F4801E0 - NIpse::CRlAgent::CRlAgent().  See crlagent.h for the full
//  layout/offset-ownership writeup; this file is only the executable body.
//
//  Same SEH-frame mismatch as CAgent::CAgent() (Agent/agent_ctor.cpp) and
//  the whole `__except_handler4`-shaped-frame family docs/msvc-vc8-idioms.md
//  documents: the compiler wraps this constructor in a cookie-XORed,
//  frame-pointer-omitted `__CxxFrameHandler3` frame because a call ahead of
//  it (the CDynTable<CPrRelation*> sub-object constructor below) is not
//  provably non-throwing, with no explicit `__try` needed in the source.
//  That exact frame shape is not reproducible under this build's fixed
//  `/GS- /EHs-c-` with the bundled compiler (see that doc's own section) -
//  the body below matches the dump instruction for instruction past the
//  frame furniture.
//============================================================================
#include "crlagent.h"
#include "CFloat.h"

namespace NIpse {

//  0x6F4800C0 - see crlagent.h's own comment on CDynTable_CPrRelationPtr for
//  why this is called by raw address rather than declared as a second naked
//  thunk (Net/fileconn_deleteself.cpp's DestructFileConnFn/
//  ConnectFinishFileFn already establish the pattern for a real caller of a
//  known-signature, out-of-scope shipped function).  thiscall: the object
//  under construction in ecx, then (right to left) the caller's own arg_C,
//  arg_8, arg_4 and finally a pointer to a {count, capacity} pair - matches
//  the dump's own push order (edi, edi, edi, then the address of the local
//  pair) exactly, since MSVC pushes a normal parameter list right to left.
//  `retn 10h` (4 stack dwords, matching the declared 4 non-`this` params).
typedef void* (__thiscall *ConstructDynTablePrRelationFn)(
        void* self, unsigned int argC, unsigned int arg8, unsigned int arg4,
        SDynTableInitPair* pair);

//  The dump genuinely stamps `[esi]` twice with two different vtable
//  addresses before the constructor returns (base's own vtable, then
//  CRlAgent's own restamp) - a real base-then-derived construction shape
//  the whole codebase's `StampFloatMiniB`-style helpers already reproduce
//  the same way (Unit/unit_ctor.cpp's own file header). A plain `m_vfptr =
//  (void*)A; ... m_vfptr = (void*)B;` of two compile-time-constant
//  literals lets this compiler's dead-store elimination collapse the pair
//  into a single final store (measured: the first write vanished from the
//  listing entirely). Routing both stores through a `volatile` pointer
//  forces the compiler to keep both `mov dword ptr [reg], imm` instructions
//  it already picks for a plain literal store - same instruction shape,
//  just not eligible for eliminating the first one as dead.
static __forceinline void StampVtable(void* pThis, void* vtbl)
{
    *(void* volatile*)pThis = vtbl;
}

CRlAgent::CRlAgent()
{
    //  ---- inlined base chain (CPresence/CPresenceTagged/CPrRelation) ----
    //  Zeroed first (0x480205-0x480216), then CPrRelation's own vtable is
    //  stamped (0x480219) before the rest of the base chain's own fields are
    //  written - see crlagent.h for why every one of these belongs to a base
    //  class, not to CRlAgent itself.
    m_baseReserved04[0] = 0;
    m_baseReserved04[1] = 0;
    m_baseReserved04[2] = 0;
    m_baseReserved04[3] = 0;
    StampVtable(this, (void*)0x6F952004);     // ??_7CPrRelation@NIpse@@6B@

    m_baseReserved1C[0] = 0;   // +0x1C
    m_baseReserved1C[1] = 0;   // +0x20
    m_baseFreeIndex14 = (unsigned int)-1;
    m_baseFreeIndex18 = (unsigned int)-1;
    m_baseReserved1C[2] = 0;   // +0x24 (shares the array with +0x1C/+0x20 above)
    m_baseReserved28 = 0;
    m_baseReserved30[0] = 0;   // +0x30
    m_baseReserved30[1] = 0;   // +0x34
    m_baseReserved30[2] = 0;   // +0x38
    m_baseReserved40 = 0;      // +0x3C is written twice below; +0x40 here
    m_baseDefaultFloat44 = *(const unsigned int*)&g_CFloatZero;  // dereferences the global (Math/CFloat.h)
    m_baseSentinel48 = (unsigned int)-1;
    m_baseReserved4C = 0;
    m_baseListPrev = this;    // +0x2C - empty CLinkedNode-shaped list, self-linked
    m_baseListNext = this;    // +0x3C - same shape

    //  ---- CRlAgent's own construction begins here: restamp own vtable ----
    StampVtable(this, (void*)0x6F95207C);     // ??_7CRlAgent@NIpse@@6B@
    m_ownReserved50 = 0;
    m_ownReserved54 = 0;

    SDynTableInitPair localPair;
    localPair.m_countArg = 0;
    localPair.m_capacity = 8;
    ((ConstructDynTablePrRelationFn)0x6F4800C0)(
            &m_relatedAgents, 0, 0, 0, &localPair);

    //  Sixteen explicit dword stores, not a loop - the dump has none, and a
    //  runtime loop also throws away the "edi is already a live zero"
    //  register fact the straight-line stores below let the compiler reuse
    //  (matching the target's own `mov [reg+N], edi` shape rather than
    //  materialising a fresh zero register).
    unsigned int* tail = (unsigned int*)m_ownReserved78;
    tail[0]  = 0; tail[1]  = 0; tail[2]  = 0; tail[3]  = 0;
    tail[4]  = 0; tail[5]  = 0; tail[6]  = 0; tail[7]  = 0;
    tail[8]  = 0; tail[9]  = 0; tail[10] = 0; tail[11] = 0;
    tail[12] = 0; tail[13] = 0; tail[14] = 0; tail[15] = 0;
}

}  // namespace NIpse
