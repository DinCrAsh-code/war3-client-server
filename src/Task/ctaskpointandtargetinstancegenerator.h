//============================================================================
//  InstanceGenerator<CTaskPointAndTarget>'s own singleton layout - same
//  split as ctaskpointinstancegenerator.h/ctaskpointgeneratorsingleton.cpp
//  (InstanceGenerator<CTaskPoint>, the reference implementation this file
//  mirrors): the getter (GetCTaskPointAndTargetGeneratorSingleton) stays in
//  its own translation unit, separate from its three callers in
//  taskpointandtargetinstancegenerator.cpp, so this fixed /O2 /Ob2
//  toolchain (no /GL /LTCG - no cross-TU inlining) keeps emitting a real
//  out-of-line `call` at every one of
//  InstanceGenerator<CTaskPointAndTarget>'s own three vtable-slot call
//  sites, exactly as the shipped binary does.
//
//  This getter (0x6F271B30) is CTaskPointAndTarget's *own*, not a
//  cross-family shared leaf - confirmed directly, a plain two-instruction
//  `mov eax, offset dword_6FAB49D0 / retn`, its own address, reused only by
//  CTaskPointAndTarget's own three InstanceGenerator slots
//  (CreateInstance/DestroyInstance/ReleaseAll,
//  taskpointandtargetinstancegenerator.cpp).
//
//  0x6FAB49D0 - InstanceGenerator<CTaskPointAndTarget>'s own
//  process-lifetime singleton: a vtable pointer at +0x00 (the static
//  initializer that stamps
//  `??_7?$InstanceGenerator@VCTaskPointAndTarget@@@@6B@` here was not
//  chased - out of this session's scope, same boundary
//  ctaskpointinstancegenerator.h already draws for its own static
//  initializer), immediately followed at +0x04 by an embedded,
//  layout-compatible CDataAllocator instance -
//  "TInstanceAllocator<CTaskPointAndTarget>" in the RTTI hierarchy
//  (agent_worktrees/classes/0x6F92F9F4.json's own `hierarchy`).  Declared
//  `extern` only, matching this repo's established convention for a global
//  whose construction is out of scope - funcmap.DATA binds the symbol to
//  the real, shipped address so these reconstructions read/write the
//  game's own live singleton rather than a private, zero-filled copy
//  (link_check.py's own concern).
//============================================================================
#ifndef CTASKPOINTANDTARGETINSTANCEGENERATOR_H
#define CTASKPOINTANDTARGETINSTANCEGENERATOR_H

#include "../Net/dataallocator.h"

struct CTaskPointAndTargetInstanceGenerator
{
    void*          m_vtable;      // +0x00 - ??_7?$InstanceGenerator@VCTaskPointAndTarget@@@@6B@
    CDataAllocator m_allocator;   // +0x04 - TInstanceAllocator<CTaskPointAndTarget>
};

extern CTaskPointAndTargetInstanceGenerator g_ctaskPointAndTargetInstanceGenerator;   // dword_6FAB49D0

//  0x6F271B30 - `retn 0`, no arguments: return the address of the
//  generator singleton itself.  See ctaskpointandtargetgeneratorsingleton.cpp.
CTaskPointAndTargetInstanceGenerator* GetCTaskPointAndTargetGeneratorSingleton();

#endif
