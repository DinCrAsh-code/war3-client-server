//============================================================================
//  0x6F2A4BB0 - CUnit::SubmitOrder_0xD0006 (unit.h).  IssueStopOrder's own
//  shape (unitstoporder.cpp) with order id 0xD0006 instead of 0xD0004
//  ("stop") - not otherwise identified.
//============================================================================
#include "unit.h"
#include "unitorder.h"

void CUnit::SubmitOrder_0xD0006(int replaceCurrent)
{
    SubmitOrder(MakeOrderAgent(0xD0006, GetOwningPlayerIndex(), 0),
                replaceCurrent, 1);
}
