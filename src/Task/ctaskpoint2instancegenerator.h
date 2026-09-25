//============================================================================
//  InstanceGenerator<CTaskPoint2>'s own singleton layout - same split as
//  ctaskpointinstancegenerator.h/ctaskpointgeneratorsingleton.cpp
//  (InstanceGenerator<CTaskPoint>, the reference implementation this file
//  mirrors): the getter (GetCTaskPoint2GeneratorSingleton) stays in its own
//  translation unit, separate from its three callers in
//  taskpoint2instancegenerator.cpp, so this fixed /O2 /Ob2 toolchain (no
//  /GL /LTCG - no cross-TU inlining) keeps emitting a real out-of-line
//  `call` at every one of InstanceGenerator<CTaskPoint2>'s own three
//  vtable-slot call sites, exactly as the shipped binary does.
//
//  This getter (0x6F271AB0) is CTaskPoint2's *own*, not a cross-family
//  shared leaf - confirmed directly, a plain two-instruction
//  `mov eax, offset dword_6FAB49B8 / retn`, its own address, reused only by
//  CTaskPoint2's own three InstanceGenerator slots
//  (CreateInstance/DestroyInstance/ReleaseAll,
//  taskpoint2instancegenerator.cpp).
//
//  0x6FAB49B8 - InstanceGenerator<CTaskPoint2>'s own process-lifetime
//  singleton: a vtable pointer at +0x00 (the static initializer that
//  stamps `??_7?$InstanceGenerator@VCTaskPoint2@@@@6B@` here was not
//  chased - out of this session's scope, same boundary
//  ctaskpointinstancegenerator.h already draws for its own static
//  initializer), immediately followed at +0x04 by an embedded,
//  layout-compatible CDataAllocator instance -
//  "TInstanceAllocator<CTaskPoint2>" in the RTTI hierarchy
//  (agent_worktrees/classes/0x6F92F9E4.json's own `hierarchy`).  Declared
//  `extern` only, matching this repo's established convention for a global
//  whose construction is out of scope - funcmap.DATA binds the symbol to
//  the real, shipped address so these reconstructions read/write the
//  game's own live singleton rather than a private, zero-filled copy
//  (link_check.py's own concern).
//============================================================================
#ifndef CTASKPOINT2INSTANCEGENERATOR_H
#define CTASKPOINT2INSTANCEGENERATOR_H

#include "../Net/dataallocator.h"

struct CTaskPoint2InstanceGenerator
{
    void*          m_vtable;      // +0x00 - ??_7?$InstanceGenerator@VCTaskPoint2@@@@6B@
    CDataAllocator m_allocator;   // +0x04 - TInstanceAllocator<CTaskPoint2>
};

extern CTaskPoint2InstanceGenerator g_ctaskPoint2InstanceGenerator;   // dword_6FAB49B8

//  0x6F271AB0 - `retn 0`, no arguments: return the address of the
//  generator singleton itself.  See ctaskpoint2generatorsingleton.cpp.
CTaskPoint2InstanceGenerator* GetCTaskPoint2GeneratorSingleton();

#endif
