//============================================================================
//  0x6F410A90 - SCheckedAllySlot's constructor (agenttypedslots.h).
//
//  Own translation unit, same reasoning every other constructor in this
//  family gives (see allytypedassign.cpp for the Assign half): the shipped
//  body issues a genuine `call sub_6F40CC60`, so the two must not sit where
//  they could be inlined into each other.
//
//  Unlike its twelve JASS-handle siblings (checkedtimerslot.cpp etc.) this
//  one has no __try/__finally - the "Assign only" family SCheckedAllySlot
//  belongs to has no Release() to call from a __finally, and the shipped
//  body is a plain zero-then-Assign with the same unreproducible
//  __except_handler4-shaped frame every leaf in this family carries
//  (docs/msvc-vc8-idioms.md) but no actual SEH control flow of its own.
//
//  Found by CPlayerWar3::CPlayerWar3() (playerwar3_ctor.cpp), which
//  constructs its own +0x38 field this way with a literal null candidate -
//  the only call site in this session's scope.
//============================================================================
#include "agenttypedslots.h"

SCheckedAllySlot::SCheckedAllySlot(CAgent* candidate)
{
    m_value = 0;
    Assign(candidate);
}
