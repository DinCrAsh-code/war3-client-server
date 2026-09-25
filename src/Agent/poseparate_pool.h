//============================================================================
//  NIpse::CPoSeparate's own static-pool ALLOCATE side. See
//  poseparate_pool_alloc.cpp/poseparate_pool_stampnew.cpp/
//  poseparate_allocateandapply.cpp for the three real, separately-compiled
//  bodies this header declares - kept in three different translation
//  units on purpose: each is a genuine `call` in the shipped stream
//  (Allocate <- StampNew <- CPoSeparate_AllocateAndOptionallyApply), and
//  sharing a TU with its own caller is exactly the trap this repo's own
//  house rules warn about (`/Ob2` silently inlining a small single-call-
//  site body, replacing the real `call` the shipped code has with the
//  callee's own expanded instructions - see popos_apply_slot4.cpp's own
//  history and poposbh_relatedagents.cpp's `ClearBcRegistration` for two
//  prior sessions catching exactly this).
//
//  `CPoSeparate_GetStaticPool()` (0x6F494DC0, Agent/agentbasechain_
//  staticpools.cpp) returns `g_pTimeSync + 0x7F8` - the SAME object this
//  struct is laid out over. Same shape as a sibling class's own pool,
//  Missile/bhpoprojectile_attach.cpp's own `CBhPoProjectilePool` (a
//  `CDataAllocator m_allocator` at +0x00, sized to hand out
//  element-plus-one-header-dword blocks, then a *second*, bespoke
//  free-list cache this code reads directly rather than through
//  `Alloc()`/`Free()` - the two fields sit right after the embedded
//  `CDataAllocator`'s own 0x14-byte body, so they are this pool's own, not
//  more of `CDataAllocator`'s).
//============================================================================
#ifndef POSEPARATE_POOL_H
#define POSEPARATE_POOL_H

#include "poseparate.h"
#include "dataallocator.h"

namespace NIpse {

struct SPoSeparatePool
{
    //  0x6F4958D0 - pop the pool's own fast free list if non-empty,
    //  otherwise grow the underlying CDataAllocator by one element and
    //  stamp a fresh CPoSeparate into it directly (no constructor call -
    //  every touched field is stamped inline, matching this whole
    //  family's own "flat, offset-addressed" convention). `retn 4` -
    //  `this` in ecx, one stack argument this call tree never uses for
    //  anything but CDataAllocator::Alloc's own `zeroMemory` (always 0
    //  from every call site reached here).
    CPoSeparate* Allocate(int zeroMemory);

    //  0x6F495AD0 - Allocate() above, then stamp the two fixed tag dwords
    //  and the two stack arguments into +0x4/+0x8, bump this pool's own
    //  +0x1C counter. `this` (the pool) in ecx, two STACK arguments -
    //  `retn 8`.
    CPoSeparate* StampNew(unsigned int a, unsigned int b);

    CDataAllocator m_allocator;      // +0x00
    void*          m_freeListHead;   // +0x14
    unsigned int   m_liveCount;      // +0x18
};

//  0x6F495C70 - allocate a fresh CPoSeparate (via SPoSeparatePool::
//  StampNew above, against the shared static pool
//  `CPoSeparate_GetStaticPool()`), store it through `*slot`, and - only
//  when `create` is set - call the new object's own `Apply(applyArg)`
//  (vtable slot 3). A genuine `__fastcall` free function (not a member -
//  `slot` is not a `this` pointer, it just happens to land in ecx the same
//  way this whole module's own `PickRegistrationFloatOrDefault_0xC`-style
//  helpers do, Agent/popos_apply_slot4.cpp): `ecx=slot`, `edx=applyArg`,
//  one stack argument `create` - `retn 4`. Reached from Agent/poposcl.h's
//  own CPoPosCl::Slot4/Load helper
//  (`TeardownAndOptionallyRecreatePoSeparate`,
//  Agent/poposcl_relatedagentslot4.cpp), which is the only caller in this
//  call tree.
CPoSeparate* __fastcall CPoSeparate_AllocateAndOptionallyApply(CPoSeparate** slot,
                                                                unsigned int applyArg,
                                                                int create);

}  // namespace NIpse

#endif  // POSEPARATE_POOL_H
