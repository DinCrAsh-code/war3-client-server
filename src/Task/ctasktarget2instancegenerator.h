//============================================================================
//  InstanceGenerator<CTaskTarget2>'s own singleton layout - same split as
//  ctasktargetinstancegenerator.h/ctasktargetgeneratorsingleton.cpp
//  (InstanceGenerator<CTaskTarget>, the immediate template sibling this
//  file mirrors): the getter (GetCTaskTarget2GeneratorSingleton) stays in
//  its own translation unit, separate from its three callers in
//  tasktarget2instancegenerator.cpp, so this fixed /O2 /Ob2 toolchain (no
//  /GL /LTCG - no cross-TU inlining) keeps emitting a real out-of-line
//  `call` at every one of InstanceGenerator<CTaskTarget2>'s own three
//  vtable-slot call sites, exactly as the shipped binary does.
//
//  **Confirms ctasktargetinstancegenerator.h's own correction, not a repeat
//  of the plan's original mistake**: this class's singleton-getter is its
//  own, separately-addressed leaf (0x6F271C30, `mov eax, offset
//  dword_6FAB4A00 / retn`) - a different address from CTaskTarget's own
//  0x6F271BB0 - confirmed directly by reading
//  InstanceGenerator<CTaskTarget2>::CreateInstance (0x6F291CA0) itself
//  rather than assuming a shared leaf.
//
//  0x6FAB4A00 - InstanceGenerator<CTaskTarget2>'s own process-lifetime
//  singleton: a vtable pointer at +0x00 (the static initializer that stamps
//  `??_7?$InstanceGenerator@VCTaskTarget2@@@@6B@` here was not chased - out
//  of this wave's scope, same boundary ctasktargetinstancegenerator.h's own
//  static initializer already draws), immediately followed at +0x04 by an
//  embedded, layout-compatible CDataAllocator instance -
//  "TInstanceAllocator<CTaskTarget2>" in the RTTI hierarchy
//  (agent_worktrees/classes/0x6F92FA14.json's own `hierarchy`).  Declared
//  `extern` only, matching this repo's established convention for a global
//  whose construction is out of scope - funcmap.DATA binds the symbol to
//  the real, shipped address so these reconstructions read/write the
//  game's own live singleton rather than a private, zero-filled copy
//  (link_check.py's own concern).
//============================================================================
#ifndef CTASKTARGET2INSTANCEGENERATOR_H
#define CTASKTARGET2INSTANCEGENERATOR_H

#include "../Net/dataallocator.h"

struct CTaskTarget2InstanceGenerator
{
    void*          m_vtable;      // +0x00 - ??_7?$InstanceGenerator@VCTaskTarget2@@@@6B@
    CDataAllocator m_allocator;   // +0x04 - TInstanceAllocator<CTaskTarget2>
};

extern CTaskTarget2InstanceGenerator g_ctaskTarget2InstanceGenerator;   // dword_6FAB4A00

//  0x6F271C30 - `retn 0`, no arguments: return the address of the
//  generator singleton itself.  See ctasktarget2generatorsingleton.cpp.
CTaskTarget2InstanceGenerator* GetCTaskTarget2GeneratorSingleton();

#endif
