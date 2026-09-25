//============================================================================
//  CJassFunc: the JASS native/user function wrapper object.  RTTI-confirmed
//  single-inheritance chain (0x6FA7A014's own type descriptor comment, all
//  four levels at mdisp:0):
//
//    CJassFunc -> CAgentWar3 -> CAgent -> CObserver -> TRefCnt
//
//  Read agentwar3.h/agent.h/observer.h/refcnt.h before touching this file -
//  every offset below is anchored to CAgentWar3's own end (+0x24).
//
//  Its own 30-slot vtable (??_7CJassFunc@@6B@ @ 0x6F942A2C,
//  agent_worktrees/classes/0x6F942A2C.json) stays out of scope, exactly as
//  every prior JASS VM session left it (docs/targets/JassVM.md): none of
//  its 30 real virtual overrides is declared here, including the two-
//  instruction slot 22 (0x6F3B1460, `GetClassName`, returns "CJassFunc")
//  even though it would trivially score - the boundary this repo has kept
//  since the first JASS VM session is about the vtable as a unit, not
//  about any one slot's difficulty, and nothing here disturbs it.
//
//  **Correction to this task's own starting premise.**  The background
//  this session was handed asserted the constructor/destructor addresses
//  were not discoverable ("nothing in agent_worktrees/funcs/ references
//  CJassFunc by name").  That is no longer true of the data actually on
//  disk: `grep -ril jassfunc agent_worktrees/funcs/` finds five hits, and
//  one of them - 0x6F3B9CE0 - is a real, SEH-framed allocate-and-construct
//  method reached from InstanceGenerator<CJassFunc>'s own factory slot
//  (0x6F3BCFF0, jassfunc.cpp) that visibly builds a CJassFunc: it calls
//  CAgent::CAgent() (0x6F001730), inlines CAgentWar3's own trivial
//  constructor (`mov dword ptr [esi+20h], 0` - the exact shape
//  agentwar3.h's own header comment documents for every other call site),
//  stamps `mov dword ptr [esi], offset ??_7CJassFunc@@6B@`, and then zeroes
//  one more field this class alone owns.  Whether the operator re-ran the
//  bulk dump between when this task was written and now, or the original
//  grep simply missed it, is not knowable from here - but the address is
//  real, its bytes are in `agent_worktrees/funcs/0x6F3B9CE0.json`, and it
//  is used below.
//
//  That same constructor is also why it is *not* reconstructed as an
//  ordinary C++ member here: its prologue/epilogue
//  (`push -1 / push offset SEH_6F3B9CE0 / mov eax, fs:0 / ... / mov eax,
//  dword_6FAAE140 / xor eax, esp` on entry, the matching `fs:0` restore
//  and stack-cookie check on exit) is the same `__except_handler4`-shaped
//  SEH frame this repo's fixed `/GS- /EHs-c-` toolchain cannot emit
//  anywhere else either (docs/msvc-vc8-idioms.md; see also
//  JassInstance::Construct/Run and friends in docs/targets/JassVM.md) - so
//  it is kept a `THUNK`, exactly like those, rather than a hand-written
//  body that could only ever diverge on the frame alone.  See
//  `CDataAllocator::ConstructCJassFunc` (dataallocator.h) and jassfunc.cpp.
//
//  No separate out-of-line *destructor* was found anywhere in the
//  currently-reachable data - nothing in `agent_worktrees/funcs/` tears
//  down a CJassFunc specifically.  Teardown for this class, if it happens
//  at all in the available closure, goes through the inherited
//  CAgent::DeleteSelf()/TRefCnt::Release() path instead (both already
//  reconstructed, agent.h/refcnt.h) - CJassFunc's own vtable slot 1
//  (0x6F3BA400) genuinely overrides DeleteSelf for real, but that override
//  is one of the 30 out-of-scope slots, so InstanceGenerator<CJassFunc>::
//  DestroyInstance below calls it purely through the inherited virtual
//  dispatch, never a hand-named CJassFunc::DeleteSelf body.
//
//  **One field beyond CAgentWar3's own +0x24 end is real, not invented.**
//  The same constructor zeroes one more dword right after the vtable
//  stamp (`mov dword ptr [esi+24h], 0`) - a genuine CJassFunc-owned field.
//  Nothing else in the currently-reachable data reads or writes it, so
//  only its existence, offset and zero-at-construction value are
//  established; its real purpose is unknown.  Independent confirmation
//  that CJassFunc really is exactly 0x28 (40) bytes: the generator
//  singleton's own embedded pool is constructed with `elementSize=0x28`
//  (0x6F85D570's own `push 28h` immediate, jassfunc.cpp) - 0x24 (this
//  class's own base, CAgentWar3) + 4 (this one field) = 0x28, matching the
//  shipped pool's own per-element size exactly.
//============================================================================
#ifndef JASSFUNC_H
#define JASSFUNC_H

#include "agentwar3.h"

class CJassFunc : public CAgentWar3
{
public:
    //  No explicit constructor is declared: the only reconstructable
    //  construction path for a CJassFunc is entirely inside the SEH-framed
    //  thunk above (CDataAllocator::ConstructCJassFunc, dataallocator.h) -
    //  there is no separate, out-of-line "CJassFunc::CJassFunc" body for a
    //  hand-written C++ constructor here to stand in for, and declaring
    //  one that this class never actually calls would misrepresent what
    //  was reconstructed.

    //  +0x24 - zeroed by the constructor (see the header comment above);
    //  no other reader or writer of this offset is reachable from any data
    //  currently in `agent_worktrees/` or `asm/`.  Not a
    //  `char m_reservedNN[]` padding gap - it is a real, touched dword -
    //  so it keeps a proper field name rather than the array-gap spelling
    //  the house rules reserve for untouched filler.
    unsigned int m_reserved24;   // +0x24
};

#endif
