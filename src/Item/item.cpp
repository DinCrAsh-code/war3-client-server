//============================================================================
//  0x6F2B7520 - CItem::CItem().  See item.h for the base/member layout.
//
//  Same unreproducible __except_handler4-shaped SEH frame as every other
//  multi-base/multi-member constructor in this family (widget.cpp,
//  agent_ctor.cpp, docs/msvc-vc8-idioms.md): four sub-constructions after
//  the first (FloatMini/FloatMiniB's own field store, Position's own field
//  block, and the two CAgentTimer members) that the compiler cannot prove
//  won't throw is enough on its own for MSVC to wrap the whole body in it,
//  with no explicit __try/__finally needed in the source.  Not
//  reproducible under this build's fixed /GS- /EHs-c-; the body past the
//  frame furniture - the CSelectable::CSelectable() call, CItem's own
//  vtable store, FloatMini/FloatMiniB's and Position's own inlined field
//  stores, and the two CAgentTimer::CAgentTimer() calls - is the same
//  instructions in the same order.
//
//  CItem's own vtable store is a manual raw-address write, not left to
//  the compiler's automatic re-stamp, and for the same reason
//  CMiscCustom.h's own m_vtable is a plain pointer rather than a real
//  C++ virtual table: this call tree only ever proves CItem *has* a
//  vtable pointer at offset 0, never its real ~110-entry method set
//  (user_knowledge.json's own CItem vtable dump has 110 slots; this
//  reconstruction only ever declares the 8 CAgent contributes).  Unlike
//  CSelectable's own transient vtable store two lines above it - which
//  this constructor immediately overwrites, so whatever the compiler
//  puts there for that half-instant never matters - CItem::CItem() is
//  this call tree's own most-derived, final constructor: nothing else
//  overwrites offset 0 afterwards, and the object goes on to live in the
//  real, unmodified game (rendering, inventory, targeting, ...), which
//  calls through this vtable at slots this reconstruction cannot supply.
//  A real live-gate crash (eip=0x0 - a call through a vtable slot read
//  past the end of an 8-entry array) confirmed this the hard way; writing
//  the real, live vtable address (0x6F9320B4, user_knowledge.json) here
//  instead keeps the same one-store shape while pointing it at the real
//  thing, the same fix CMiscCustom.h documents for the same reason.  The
//  compiler's own automatic stamp (to this build's synthesised, 8-entry
//  ??_7CItem@@6B@) still happens first and is dead by the time this one
//  runs right after it with nothing in between to observe the difference
//  - collapsed away by the same adjacent-store elision already relied on
//  elsewhere in this family (refcnt.h's own note on Position::~Position).
//============================================================================
#include "item.h"

CItem::CItem()
{
    *(void**)this = (void*)0x6F9320B4;
}

//----------------------------------------------------------------------------
//  0x6F2B5B10 - see item.h.
//----------------------------------------------------------------------------
unsigned int CItem::GetAgileTypeId() const
{
    //  The literal, not a call to GetItemTypeFourCC() (itemhandletable.cpp)
    //  - even though it is the exact same constant, calling it here tail-
    //  call-optimises into a `jmp GetItemTypeFourCC` where the shipped
    //  code has the immediate load inline.
    return 0x6974656D;
}

//----------------------------------------------------------------------------
//  0x6F2B76B0 - see item.h.
//----------------------------------------------------------------------------
SHandleWithType* CItem::GetHandleObject()
{
    return (SHandleWithType*)&m_position;
}

//----------------------------------------------------------------------------
//  0x6F2B6130 - see item.h. Clears the "pending" bit at CSelectable's own
//  +0x20 and, if +0x28 (the placement sub-object pointer
//  itemplacementdispatch.cpp already reaches through vtable slots 0x18/
//  0x1C) is set, forwards a fixed flag bit (0x4000000, top-byte-masked) to
//  it directly as a *data* write - not another vtable call
//  (CWar3Image::ClearPendingAndNotify/selectable.cpp); then stores `1`
//  into Position's own +0x7C and, if Position's own +0x78 is a valid
//  handle (not the "-1" sentinel), notifies whatever object that handle
//  names through a genuinely out-of-scope Storm/Terrain-rendering gate
//  (CItem::StoreAndNotifyHandle, itemnotifyhandle.cpp); finally calls this
//  object's own vtable[0x148] (index 82, not identified) with three
//  literal `1` arguments and returns its result.
//----------------------------------------------------------------------------
typedef int (__thiscall *Vtbl0x148Fn)(void*, int, int, int);

void CItem::ClearPendingAndNotify()
{
    CWar3Image::ClearPendingAndNotify();
    StoreAndNotifyHandle(1);
    ((Vtbl0x148Fn)(*(void***)this)[0x148 / 4])(this, 1, 1, 1);
}

//----------------------------------------------------------------------------
//  0x6F2AB680 - see item.h. Reads the world position's y then x (by
//  address) as hardware floats, forwards (a1, a2, y, x) to a ground-height
//  helper (sub_6F0126F0 - deep terrain/collision querying via
//  GetStormSingletonA/B plus a ray-vs-box test, sub_6F0116E0/sub_6F0116E0 -
//  out of this session's scope), adds this object's own vtable[0xE8]
//  (index 58, 0x6F266A30, not identified) result and returns the sum in
//  st0.
//----------------------------------------------------------------------------
typedef float (__thiscall *Vtbl0xE8Fn)(void*);

//  0x6F0126F0 - see item.cpp.  Deep terrain-height/collision query, out of
//  this session's scope; redirected rather than reconstructed.  __fastcall
//  (not thiscall): `flag` is a plain int, not an object pointer - it just
//  happens to share __fastcall's ecx/edx-then-stack shape with the two
//  floats and the trailing zero on the stack.
__declspec(naked) float __fastcall QueryGroundHeightHelper(int flag, void* outPtr,
                                                             float x, float y,
                                                             int unused)
{
    __asm
    {
        mov     eax, 06F0126F0h
        jmp     eax
    }
}

float CWidget::Method_0xE4(FVec3* pos, int a1, int a2, int /*a3*/)
{
    float base = QueryGroundHeightHelper(a1, (void*)a2, pos->m_x, pos->m_y, 0);
    return base + ((Vtbl0xE8Fn)(*(void***)this)[0xE8 / 4])(this);
}
