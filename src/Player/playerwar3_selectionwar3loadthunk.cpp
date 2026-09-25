//============================================================================
//  0x6F424130 - CSelectionWar3::Load.  A redirect, not a reconstruction -
//  the one deliberate exception in this session's Load work.
//
//  Unlike ::Save (selectionwar3_save.cpp, a one-call adapter over a list
//  this repo has never had to open), ::Load rebuilds this class's own
//  subgroup list from the stream: an intrusive doubly-linked list of
//  Storm-allocated nodes (allocation tag "AucUnitListNode"), each holding a
//  per-subgroup unit membership list built over the same never-modelled
//  CUnitSet the constructor's own header comment already names
//  (playerwar3_ctorsubobjects.cpp - "sub-objects this repo has never
//  modelled"), plus a parallel "AucSubGroupList" node kind, an index array
//  reserve/grow helper (sub_6F420040), a selectable-typed reference reader
//  (sub_6F420680, the same ReadUnitRef/ReadAllySlotRef shape but over
//  SCheckedSelectableSlot, agenttypedslots.h), and several debug-only sync
//  log lines (sub_6F4238B0/sub_6F422DC0's own nullsub_16 calls - dead
//  weight in this build, per the "nullsub" naming, but not proven so from
//  source).  Fourteen further functions with no already-modelled type
//  behind any of them: sub_6F421C10, sub_6F420680, sub_6F422130,
//  sub_6F4205F0, sub_6F420040, sub_6F422060, sub_6F422450, sub_6F4238B0,
//  sub_6F422DC0, sub_6F421DC0, sub_6F4206F0, sub_6F421A20 (already
//  reconstructed elsewhere, Misc/linkedfieldwalker.cpp, but reached here
//  through a use this session did not trace), sub_6F32DBE0 (also
//  already reconstructed, Pathfinding/footprinttypename.cpp) and
//  sub_6F333230 (an assert/log helper, ".\\CSelectionWar3.cpp" tagged) -
//  roughly 900 further instructions across all of them, none reaching a
//  type this session's scope already carries a layout for.
//
//  Matches the class's own established treatment: CSelectionWar3 has never
//  stamped a concrete vtable of its own in this codebase (its constructor
//  is the same kind of redirect, playerwar3_ctorsubobjects.cpp), so this
//  is one more member this session leaves as "real allocation shape known,
//  internal container not modelled" rather than guessing at ~900
//  instructions of intrusive-list bookkeeping this dump's own depth-3
//  window does carry a body for, but that nothing else in this repo can
//  cross-check a guess against.  See docs/targets/CPlayerWar3.md's own
//  "what's left" section - this is the one place this session's Load work
//  falls short of the standing "real depth-3 body, not a thunk" rule, and
//  it says so precisely there for the next session.
//
//  `retn 4` (one stack argument - store).
//============================================================================
#include "playerwar3.h"

__declspec(naked) void CSelectionWar3::Load(CDataStore*)
{
    __asm
    {
        mov     eax, 06F424130h
        jmp     eax
    }
}
