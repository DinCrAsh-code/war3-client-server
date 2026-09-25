//============================================================================
//  See itemhandledispatch.cpp.
//============================================================================
#ifndef ITEMHANDLEDISPATCH_H
#define ITEMHANDLEDISPATCH_H

//  Whatever QueryHandleField0x54 (itemhandleresolve.h) hands back - a
//  CUnit, per unitinventory.h's own note (0x6F282xxx module, +0x1F8
//  inventory pointer, vtable+0xB8 GetHandleObject - all CUnit's).  Kept as
//  its own struct/name rather than folded into CUnit directly: both
//  callers (item_remove.cpp, jasssetitemposition_native.cpp) reach this
//  member through a generically-resolved handle object whose real type
//  they have not otherwise proven, so the cast to CUnit* happens inside
//  the .cpp once the body actually needs CUnit's own members rather than
//  at each call site.
struct SItemHandleFieldObject
{
    //  0x6F282E40 - see itemhandledispatch.cpp.  Reconstructed: was a
    //  naked redirect while this session's own callee (sub_6F0F0F80,
    //  CAbilityInventory::DetachAndNotify) was still unreached "boring
    //  deep procedure"; that whole bounded subtree is reconstructed now
    //  (abilityinventorydispatch.cpp and neighbours).
    int DispatchUnknown(void* arg);
};

#endif
