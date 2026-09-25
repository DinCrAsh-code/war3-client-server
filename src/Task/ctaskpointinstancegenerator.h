//============================================================================
//  InstanceGenerator<CTaskPoint>'s own singleton layout - same split as
//  ctaskinstancegenerator.h/ctaskgeneratorsingleton.cpp (InstanceGenerator
//  <CTask>, the reference implementation this file mirrors): the getter
//  (GetCTaskPointGeneratorSingleton) stays in its own translation unit,
//  separate from its three callers in taskpointinstancegenerator.cpp, so
//  this fixed /O2 /Ob2 toolchain (no /GL /LTCG - no cross-TU inlining) keeps
//  emitting a real out-of-line `call` at every one of
//  InstanceGenerator<CTaskPoint>'s own three vtable-slot call sites, exactly
//  as the shipped binary does.
//
//  Per ctaskinstancegenerator.h's own correction to this wave's starting
//  plan: this getter (0x6F271A30) is CTaskPoint's *own*, not a cross-family
//  shared leaf - confirmed directly, a plain two-instruction
//  `mov eax, offset dword_6FAB49A0 / retn`, its own address, reused only by
//  CTaskPoint's own three InstanceGenerator slots
//  (CreateInstance/DestroyInstance/ReleaseAll, taskpointinstancegenerator.cpp).
//
//  0x6FAB49A0 - InstanceGenerator<CTaskPoint>'s own process-lifetime
//  singleton: a vtable pointer at +0x00 (the static initializer that stamps
//  `??_7?$InstanceGenerator@VCTaskPoint@@@@6B@` here was not chased - out of
//  this session's scope, same boundary ctaskinstancegenerator.h already
//  draws for its own static initializer), immediately followed at +0x04 by
//  an embedded, layout-compatible CDataAllocator instance -
//  "TInstanceAllocator<CTaskPoint>" in the RTTI hierarchy
//  (agent_worktrees/classes/0x6F92F9D4.json's own `hierarchy`).  Declared
//  `extern` only, matching this repo's established convention for a global
//  whose construction is out of scope - funcmap.DATA binds the symbol to
//  the real, shipped address so these reconstructions read/write the
//  game's own live singleton rather than a private, zero-filled copy
//  (link_check.py's own concern).
//============================================================================
#ifndef CTASKPOINTINSTANCEGENERATOR_H
#define CTASKPOINTINSTANCEGENERATOR_H

#include "../Net/dataallocator.h"

struct CTaskPointInstanceGenerator
{
    void*          m_vtable;      // +0x00 - ??_7?$InstanceGenerator@VCTaskPoint@@@@6B@
    CDataAllocator m_allocator;   // +0x04 - TInstanceAllocator<CTaskPoint>
};

extern CTaskPointInstanceGenerator g_ctaskPointInstanceGenerator;   // dword_6FAB49A0

//  0x6F271A30 - `retn 0`, no arguments: return the address of the
//  generator singleton itself.  See ctaskpointgeneratorsingleton.cpp.
CTaskPointInstanceGenerator* GetCTaskPointGeneratorSingleton();

#endif
