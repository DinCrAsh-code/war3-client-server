//============================================================================
//  InstanceGenerator<CTaskGhost>'s own singleton layout - same split as
//  ctaskorderparaminstancegenerator.h (InstanceGenerator<CTaskOrderParam>),
//  which is itself mirrored from jassfuncinstancegenerator.h: the getter
//  (GetCTaskGhostGeneratorSingleton) stays in its own translation unit,
//  separate from its three callers in taskghostinstancegenerator.cpp, so
//  this fixed /O2 /Ob2 toolchain (no /GL /LTCG - no cross-TU inlining)
//  keeps emitting a real out-of-line `call` at every one of
//  InstanceGenerator<CTaskGhost>'s own three vtable-slot call sites, exactly
//  as the shipped binary does.
//
//  Confirms ctaskorderparaminstancegenerator.h's own correction directly,
//  one class further down the chain (the last one in this family):
//  InstanceGenerator<CTaskGhost>::CreateInstance (0x6F291CC0) calls
//  sub_6F271CB0, not CTaskOrderParam's sub_6F2719B0 - its own
//  singleton-getter at its own address, the same two-instruction
//  `mov eax, offset <this T's own singleton> / retn` shape.
//
//  0x6FAB4A18 - InstanceGenerator<CTaskGhost>'s own process-lifetime
//  singleton: a vtable pointer at +0x00 (the static initializer that
//  stamps `??_7?$InstanceGenerator@VCTaskGhost@@@@6B@` here was not
//  chased - same out-of-scope boundary
//  ctaskinstancegenerator.h/ctaskorderparaminstancegenerator.h already draw
//  for their own static initializers), immediately followed at +0x04 by
//  an embedded, layout-compatible CDataAllocator instance -
//  "TInstanceAllocator<CTaskGhost>" in the RTTI hierarchy
//  (agent_worktrees/classes/0x6F92FA24.json's own `hierarchy`).  Declared
//  `extern` only, matching this repo's established convention for a
//  global whose construction is out of scope - funcmap.DATA binds the
//  symbol to the real, shipped address so these reconstructions read/write
//  the game's own live singleton rather than a private, zero-filled copy
//  (link_check.py's own concern).
//============================================================================
#ifndef CTASKGHOSTINSTANCEGENERATOR_H
#define CTASKGHOSTINSTANCEGENERATOR_H

#include "../Net/dataallocator.h"

struct CTaskGhostInstanceGenerator
{
    void*          m_vtable;      // +0x00 - ??_7?$InstanceGenerator@VCTaskGhost@@@@6B@
    CDataAllocator m_allocator;   // +0x04 - TInstanceAllocator<CTaskGhost>
};

extern CTaskGhostInstanceGenerator g_ctaskGhostInstanceGenerator;   // dword_6FAB4A18

//  0x6F271CB0 - `retn 0`, no arguments: return the address of the
//  generator singleton itself.  See ctaskghostgeneratorsingleton.cpp.
CTaskGhostInstanceGenerator* GetCTaskGhostGeneratorSingleton();

#endif
