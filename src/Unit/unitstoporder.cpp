//============================================================================
//  0x6F2A4B80 - CUnit::IssueStopOrder: make a "stop" order and submit it.
//
//  Seventeen instructions and three calls, and the whole thing is one
//  statement: SubmitOrder(MakeOrderAgent(851972, GetOwningPlayerIndex(), 0),
//  replaceCurrent, 1).  MSVC's right-to-left argument evaluation is what
//  puts the two constant pushes at the top of the function, well before the
//  order that becomes the first argument exists - which is also why the
//  `push 0` that belongs to MakeOrderAgent's own stack argument sits
//  between them and the virtual call.
//
//  The virtual call is CUnit's slot 59, GetOwningPlayerIndex (+0xEC,
//  0x6F28B1D0 - `mov eax,[ecx+58h]` / `retn`, no arguments).  The three
//  pushes around it are not its arguments: it cleans nothing, and the
//  function's own stack only balances if it does.
//
//  0xD0004 is 851972, the JASS order id "stop"; see unitordermake.cpp.
//
//  Own translation unit: MakeOrderAgent and SubmitOrder are both real calls
//  out of it.
//============================================================================
#include "unit.h"
#include "unitorder.h"

void CUnit::IssueStopOrder(int replaceCurrent)
{
    SubmitOrder(MakeOrderAgent(kUnitOrderStop, GetOwningPlayerIndex(), 0),
                replaceCurrent, 1);
}
