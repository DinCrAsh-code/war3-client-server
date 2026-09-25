//============================================================================
//  InstanceGenerator<CTask>'s own singleton layout - same split as
//  jassfuncinstancegenerator.h/jassfuncgeneratorsingleton.cpp
//  (InstanceGenerator<CJassFunc>, the precedent this whole file mirrors):
//  the getter (GetCTaskGeneratorSingleton) stays in its own translation
//  unit, separate from its three callers in taskinstancegenerator.cpp, so
//  this fixed /O2 /Ob2 toolchain (no /GL /LTCG - no cross-TU inlining)
//  keeps emitting a real out-of-line `call` at every one of
//  InstanceGenerator<CTask>'s own three vtable-slot call sites, exactly as
//  the shipped binary does.
//
//  **Correction to this wave's own starting plan.** The plan handed to this
//  session described sub_6F2717B0 as one leaf "shared by every one of the
//  33 InstanceGenerator<T> vtable slot bodies" across the whole
//  CTaskXxx family. That is not what the disassembly shows: each T has its
//  own singleton-getter at its own address (confirmed directly -
//  InstanceGenerator<CTaskParam>::CreateInstance, 0x6F291BA0, calls
//  sub_6F271830, not sub_6F2717B0; InstanceGenerator<CTaskRotate>'s calls
//  sub_6F2718B0; InstanceGenerator<CTaskAction>'s calls sub_6F271930 - four
//  different addresses for four different T's, each a two-instruction
//  `mov eax, offset <that T's own singleton> / retn` exactly like this
//  one). sub_6F2717B0 is CTask's *own* getter, reused only by CTask's own
//  three InstanceGenerator slots (CreateInstance/DestroyInstance/
//  ReleaseAll, taskinstancegenerator.cpp) - the same one-getter-per-T shape
//  GetJassFuncGeneratorSingleton already established, not a cross-family
//  shared leaf. Later-wave agents reconstructing the other 10
//  InstanceGenerator<CTaskXxx> classes need their own singleton-getter at
//  their own address, following this same pattern - not a call into this
//  one.
//
//  0x6FAB4928 - InstanceGenerator<CTask>'s own process-lifetime singleton:
//  a vtable pointer at +0x00 (the static initializer that stamps
//  `??_7?$InstanceGenerator@VCTask@@@@6B@` here was not chased - out of
//  this wave's scope, same boundary jassfuncinstancegenerator.h already
//  draws for its own static initializer), immediately followed at +0x04 by
//  an embedded, layout-compatible CDataAllocator instance -
//  "TInstanceAllocator<CTask>" in the RTTI hierarchy
//  (agent_worktrees/classes/0x6F92F984.json's own `hierarchy`).  Declared
//  `extern` only, matching this repo's established convention for a global
//  whose construction is out of scope (g_jassFuncInstanceGenerator,
//  jassfuncinstancegenerator.h) - funcmap.DATA binds the symbol to the
//  real, shipped address so these reconstructions read/write the game's
//  own live singleton rather than a private, zero-filled copy
//  (link_check.py's own concern).
//============================================================================
#ifndef CTASKINSTANCEGENERATOR_H
#define CTASKINSTANCEGENERATOR_H

#include "../Net/dataallocator.h"

struct CTaskInstanceGenerator
{
    void*          m_vtable;      // +0x00 - ??_7?$InstanceGenerator@VCTask@@@@6B@
    CDataAllocator m_allocator;   // +0x04 - TInstanceAllocator<CTask>
};

extern CTaskInstanceGenerator g_ctaskInstanceGenerator;   // dword_6FAB4928

//  0x6F2717B0 - `retn 0`, no arguments: return the address of the
//  generator singleton itself.  See ctaskgeneratorsingleton.cpp.
CTaskInstanceGenerator* GetCTaskGeneratorSingleton();

#endif
