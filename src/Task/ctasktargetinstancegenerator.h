//============================================================================
//  InstanceGenerator<CTaskTarget>'s own singleton layout - same split as
//  ctaskinstancegenerator.h/ctaskgeneratorsingleton.cpp
//  (InstanceGenerator<CTask>, the reference implementation this file
//  mirrors): the getter (GetCTaskTargetGeneratorSingleton) stays in its own
//  translation unit, separate from its three callers in
//  tasktargetinstancegenerator.cpp, so this fixed /O2 /Ob2 toolchain (no
//  /GL /LTCG - no cross-TU inlining) keeps emitting a real out-of-line
//  `call` at every one of InstanceGenerator<CTaskTarget>'s own three
//  vtable-slot call sites, exactly as the shipped binary does.
//
//  **Confirms ctaskinstancegenerator.h's own correction, not a repeat of the
//  plan's original mistake**: this class's singleton-getter is its own,
//  separately-addressed leaf (0x6F271BB0, `mov eax, offset dword_6FAB49E8 /
//  retn`) - a different address from CTask's own 0x6F2717B0 - confirmed
//  directly by reading InstanceGenerator<CTaskTarget>::CreateInstance
//  (0x6F291C80) itself rather than assuming a shared leaf.
//
//  0x6FAB49E8 - InstanceGenerator<CTaskTarget>'s own process-lifetime
//  singleton: a vtable pointer at +0x00 (the static initializer that stamps
//  `??_7?$InstanceGenerator@VCTaskTarget@@@@6B@` here was not chased - out
//  of this wave's scope, same boundary jassfuncinstancegenerator.h/
//  ctaskinstancegenerator.h's own static initializers already draw),
//  immediately followed at +0x04 by an embedded, layout-compatible
//  CDataAllocator instance - "TInstanceAllocator<CTaskTarget>" in the RTTI
//  hierarchy (agent_worktrees/classes/0x6F92FA04.json's own `hierarchy`).
//  Declared `extern` only, matching this repo's established convention for
//  a global whose construction is out of scope - funcmap.DATA binds the
//  symbol to the real, shipped address so these reconstructions read/write
//  the game's own live singleton rather than a private, zero-filled copy
//  (link_check.py's own concern).
//============================================================================
#ifndef CTASKTARGETINSTANCEGENERATOR_H
#define CTASKTARGETINSTANCEGENERATOR_H

#include "../Net/dataallocator.h"

struct CTaskTargetInstanceGenerator
{
    void*          m_vtable;      // +0x00 - ??_7?$InstanceGenerator@VCTaskTarget@@@@6B@
    CDataAllocator m_allocator;   // +0x04 - TInstanceAllocator<CTaskTarget>
};

extern CTaskTargetInstanceGenerator g_ctaskTargetInstanceGenerator;   // dword_6FAB49E8

//  0x6F271BB0 - `retn 0`, no arguments: return the address of the
//  generator singleton itself.  See ctasktargetgeneratorsingleton.cpp.
CTaskTargetInstanceGenerator* GetCTaskTargetGeneratorSingleton();

#endif
