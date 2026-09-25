//============================================================================
//  InstanceGenerator<CTaskRotate>'s own singleton layout - same split as
//  ctaskinstancegenerator.h (InstanceGenerator<CTask>)/
//  ctaskparaminstancegenerator.h (InstanceGenerator<CTaskParam>), both
//  mirrored from jassfuncinstancegenerator.h: the getter
//  (GetCTaskRotateGeneratorSingleton) stays in its own translation unit,
//  separate from its three callers in taskrotateinstancegenerator.cpp, so
//  this fixed /O2 /Ob2 toolchain (no /GL /LTCG - no cross-TU inlining)
//  keeps emitting a real out-of-line `call` at every one of
//  InstanceGenerator<CTaskRotate>'s own three vtable-slot call sites,
//  exactly as the shipped binary does.
//
//  Confirms ctaskinstancegenerator.h's own correction directly, a third
//  time: InstanceGenerator<CTaskRotate>::CreateInstance (0x6F291BC0) calls
//  sub_6F2718B0, not CTask's sub_6F2717B0 or CTaskParam's sub_6F271830 -
//  its own singleton-getter at its own address, the same two-instruction
//  `mov eax, offset <this T's own singleton> / retn` shape.
//
//  0x6FAB4958 - InstanceGenerator<CTaskRotate>'s own process-lifetime
//  singleton: a vtable pointer at +0x00 (the static initializer that
//  stamps `??_7?$InstanceGenerator@VCTaskRotate@@@@6B@` here was not
//  chased - same out-of-scope boundary ctaskinstancegenerator.h/
//  ctaskparaminstancegenerator.h already draw for their own static
//  initializers), immediately followed at +0x04 by an embedded,
//  layout-compatible CDataAllocator instance -
//  "TInstanceAllocator<CTaskRotate>" in the RTTI hierarchy
//  (agent_worktrees/classes/0x6F92F9A4.json's own `hierarchy`).  Declared
//  `extern` only, matching this repo's established convention for a global
//  whose construction is out of scope - funcmap.DATA binds the symbol to
//  the real, shipped address so these reconstructions read/write the
//  game's own live singleton rather than a private, zero-filled copy
//  (link_check.py's own concern).
//============================================================================
#ifndef CTASKROTATEINSTANCEGENERATOR_H
#define CTASKROTATEINSTANCEGENERATOR_H

#include "../Net/dataallocator.h"

struct CTaskRotateInstanceGenerator
{
    void*          m_vtable;      // +0x00 - ??_7?$InstanceGenerator@VCTaskRotate@@@@6B@
    CDataAllocator m_allocator;   // +0x04 - TInstanceAllocator<CTaskRotate>
};

extern CTaskRotateInstanceGenerator g_ctaskRotateInstanceGenerator;   // dword_6FAB4958

//  0x6F2718B0 - `retn 0`, no arguments: return the address of the
//  generator singleton itself.  See ctaskrotategeneratorsingleton.cpp.
CTaskRotateInstanceGenerator* GetCTaskRotateGeneratorSingleton();

#endif
