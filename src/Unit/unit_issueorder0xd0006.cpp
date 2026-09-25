//============================================================================
//  0x6F2A3C90 - CUnit::IssueOrder_0xD0006 (unit.h).  Build order id
//  0xD0006 for this unit's own owning player, no source reference, and
//  start it immediately.  The order id's own meaning is not established
//  (0xD0004 is "stop", per unitordermake.cpp) so it keeps a numeric name.
//============================================================================
#include "unit.h"
#include "unitorder.h"

void CUnit::IssueOrder_0xD0006()
{
    StartOrderNow(MakeOrderAgent(0xD0006, GetOwningPlayerIndex(), 0));
}
