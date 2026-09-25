//============================================================================
//  InstanceGenerator<CTaskOrderParam>'s own singleton layout - same split as
//  ctaskparaminstancegenerator.h (InstanceGenerator<CTaskParam>), which is
//  itself mirrored from jassfuncinstancegenerator.h: the getter
//  (GetCTaskOrderParamGeneratorSingleton) stays in its own translation
//  unit, separate from its three callers in
//  taskorderparaminstancegenerator.cpp, so this fixed /O2 /Ob2 toolchain
//  (no /GL /LTCG - no cross-TU inlining) keeps emitting a real out-of-line
//  `call` at every one of InstanceGenerator<CTaskOrderParam>'s own three
//  vtable-slot call sites, exactly as the shipped binary does.
//
//  Confirms ctaskparaminstancegenerator.h's own correction directly, one
//  class further down the chain: InstanceGenerator<CTaskOrderParam>::
//  CreateInstance (0x6F291C00) calls sub_6F2719B0, not CTaskParam's
//  sub_6F271830 - its own singleton-getter at its own address, the same
//  two-instruction `mov eax, offset <this T's own singleton> / retn`
//  shape.
//
//  0x6FAB4988 - InstanceGenerator<CTaskOrderParam>'s own process-lifetime
//  singleton: a vtable pointer at +0x00 (the static initializer that
//  stamps `??_7?$InstanceGenerator@VCTaskOrderParam@@@@6B@` here was not
//  chased - same out-of-scope boundary
//  ctaskinstancegenerator.h/ctaskparaminstancegenerator.h already draw
//  for their own static initializers), immediately followed at +0x04 by
//  an embedded, layout-compatible CDataAllocator instance -
//  "TInstanceAllocator<CTaskOrderParam>" in the RTTI hierarchy
//  (agent_worktrees/classes/0x6F92F9C4.json's own `hierarchy`).  Declared
//  `extern` only, matching this repo's established convention for a
//  global whose construction is out of scope - funcmap.DATA binds the
//  symbol to the real, shipped address so these reconstructions read/write
//  the game's own live singleton rather than a private, zero-filled copy
//  (link_check.py's own concern).
//============================================================================
#ifndef CTASKORDERPARAMINSTANCEGENERATOR_H
#define CTASKORDERPARAMINSTANCEGENERATOR_H

#include "../Net/dataallocator.h"

struct CTaskOrderParamInstanceGenerator
{
    void*          m_vtable;      // +0x00 - ??_7?$InstanceGenerator@VCTaskOrderParam@@@@6B@
    CDataAllocator m_allocator;   // +0x04 - TInstanceAllocator<CTaskOrderParam>
};

extern CTaskOrderParamInstanceGenerator g_ctaskOrderParamInstanceGenerator;   // dword_6FAB4988

//  0x6F2719B0 - `retn 0`, no arguments: return the address of the
//  generator singleton itself.  See ctaskorderparamgeneratorsingleton.cpp.
CTaskOrderParamInstanceGenerator* GetCTaskOrderParamGeneratorSingleton();

#endif
