//============================================================================
//  InstanceGenerator<CTaskAction>'s own singleton layout - same split as
//  ctaskparaminstancegenerator.h (InstanceGenerator<CTaskParam>), which is
//  itself mirrored from jassfuncinstancegenerator.h: the getter
//  (GetCTaskActionGeneratorSingleton) stays in its own translation unit,
//  separate from its three callers in taskactioninstancegenerator.cpp, so
//  this fixed /O2 /Ob2 toolchain (no /GL /LTCG - no cross-TU inlining)
//  keeps emitting a real out-of-line `call` at every one of
//  InstanceGenerator<CTaskAction>'s own three vtable-slot call sites,
//  exactly as the shipped binary does.
//
//  InstanceGenerator<CTaskAction>::CreateInstance (0x6F291BE0) calls
//  sub_6F271930, not any other class's own getter - its own singleton-
//  getter at its own address, the same two-instruction `mov eax, offset
//  <this T's own singleton> / retn` shape as every other InstanceGenerator
//  in this family.
//
//  0x6FAB4970 - InstanceGenerator<CTaskAction>'s own process-lifetime
//  singleton: a vtable pointer at +0x00 (the static initializer that
//  stamps `??_7?$InstanceGenerator@VCTaskAction@@@@6B@` here was not
//  chased - same out-of-scope boundary
//  ctaskparaminstancegenerator.h/ctaskinstancegenerator.h/
//  jassfuncinstancegenerator.h already draw for their own static
//  initializers), immediately followed at +0x04 by an embedded,
//  layout-compatible CDataAllocator instance - "TInstanceAllocator<CTaskAction>"
//  in the RTTI hierarchy (agent_worktrees/classes/0x6F92F9B4.json's own
//  `hierarchy`).  Declared `extern` only, matching this repo's established
//  convention for a global whose construction is out of scope -
//  funcmap.DATA binds the symbol to the real, shipped address so these
//  reconstructions read/write the game's own live singleton rather than a
//  private, zero-filled copy (link_check.py's own concern).
//============================================================================
#ifndef CTASKACTIONINSTANCEGENERATOR_H
#define CTASKACTIONINSTANCEGENERATOR_H

#include "../Net/dataallocator.h"

struct CTaskActionInstanceGenerator
{
    void*          m_vtable;      // +0x00 - ??_7?$InstanceGenerator@VCTaskAction@@@@6B@
    CDataAllocator m_allocator;   // +0x04 - TInstanceAllocator<CTaskAction>
};

extern CTaskActionInstanceGenerator g_ctaskActionInstanceGenerator;   // dword_6FAB4970

//  0x6F271930 - `retn 0`, no arguments: return the address of the
//  generator singleton itself.  See ctaskactiongeneratorsingleton.cpp.
CTaskActionInstanceGenerator* GetCTaskActionGeneratorSingleton();

#endif
