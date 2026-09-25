//============================================================================
//  InstanceGenerator<CJassFunc>'s own singleton layout - split into its own
//  header/TU pair (this header + jassfuncgeneratorsingleton.cpp) purely so
//  GetJassFuncGeneratorSingleton() stays a real, out-of-line call from
//  every one of its three call sites in jassfunc.cpp: this fixed /O2 /Ob2
//  toolchain has no cross-translation-unit inlining (no /GL /LTCG), so a
//  function only ever gets folded into its callers when it is defined in
//  the *same* TU.  The shipped binary keeps a genuine `call sub_6F3B1470`
//  at all three of InstanceGenerator<CJassFunc>'s own vtable-slot call
//  sites rather than inlining a two-instruction leaf, so reproducing that
//  needs the same same-TU-inlining trap this repo's own jassnodectors.cpp
//  already named and worked around for `DeleteSelf`
//  (jassnodedeleteself.cpp's own header comment) and CWidget/CSelectable's
//  shared vtable slots (widget.h's own note on NotifyRegistered/Dispatch).
//============================================================================
#ifndef JASSFUNCINSTANCEGENERATOR_H
#define JASSFUNCINSTANCEGENERATOR_H

#include "../Net/dataallocator.h"

//  0x6FAB66BC / 0x6FAB66C0 - InstanceGenerator<CJassFunc>'s own
//  process-lifetime singleton: a vtable pointer at +0x00 (stamped by the
//  static initializer at 0x6F85D570, `mov dword_6FAB66BC, offset
//  ??_7?$InstanceGenerator@VCJassFunc@@@@6B@`), immediately followed at
//  +0x04 by an embedded, layout-compatible CDataAllocator instance - the
//  "TInstanceAllocator<CJassFunc>" the RTTI type descriptor names
//  (`.?AV?$TInstanceAllocator@VCJassFunc@@@@`).  Confirmed a real
//  CDataAllocator by construction: 0x6F85D570's own two immediate pushes
//  before constructing it are `push 0x80` (growCount) / `push 0x28`
//  (elementSize) - and 0x28 == sizeof(CAgentWar3) [0x24] + one more
//  CJassFunc-owned dword [0x04], exactly the size jassfunc.h's own field
//  layout gives CJassFunc.
//
//  Not reconstructed here: 0x6F85D570 itself (the static initializer,
//  which also registers an `atexit` teardown, sub_6F869540) and
//  CDataAllocator's own from-scratch constructor (sub_6F4C1A10) are
//  genuinely separate scope from "the 3 InstanceGenerator vtable slots"
//  this session was asked for - pulling either in would mean
//  reconstructing global static-initialization order and an
//  atexit-registered teardown routine this closure has not looked at.
//  Declared `extern` only, matching this repo's own established
//  convention for a global whose *construction* this session leaves
//  unmodelled (see g_jassFramePool, jassframepool.h) - `funcmap.DATA`
//  binds the symbol to the real, shipped address so these three
//  reconstructions read/write the game's own live singleton rather than a
//  private, zero-filled copy (link_check.py's own concern).
struct JassFuncInstanceGenerator
{
    void*          m_vtable;      // +0x00 - ??_7?$InstanceGenerator@VCJassFunc@@@@6B@
    CDataAllocator m_allocator;   // +0x04 - TInstanceAllocator<CJassFunc>
};

extern JassFuncInstanceGenerator g_jassFuncInstanceGenerator;   // dword_6FAB66BC

//  0x6F3B1470 - `retn 0`, no arguments: return the address of the
//  generator singleton itself.  See jassfuncgeneratorsingleton.cpp - kept
//  in its own TU (rather than static in jassfunc.cpp) purely to stay
//  out-of-line, per the header comment above.
JassFuncInstanceGenerator* GetJassFuncGeneratorSingleton();

#endif
