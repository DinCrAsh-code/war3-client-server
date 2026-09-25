//============================================================================
//  CAgentWar3's two tail-jump vtable slots.  Each is a single `jmp` into a
//  shared default in agentdefaults.cpp, which is a different translation
//  unit for exactly that reason: same-TU, MSVC would inline the two-line
//  callee and there would be no jump left to match.
//
//  Both sit in CWidget's own 0x6F2ABxxx/0x6F2ACxxx neighbourhood rather
//  than CAgentWar3's, and both appear at the same index in CWidget's *and*
//  CSelectable's vtables - which is what makes them a shared base's, not
//  CWidget's: CSelectable does not derive from CWidget (selectable.h), so
//  the only class the two can both inherit one function from is this one.
//============================================================================
#include "widget.h"   // these bodies belong to CWar3Image/CWidget, not CAgentWar3

//  From agentdefaults.cpp; CAgent::Method_0x24 is declared in the
//  header and defined there too, for the same reason.

//----------------------------------------------------------------------------
//  0x6F2AB240 - vtable slot 8 (+0x20).
//----------------------------------------------------------------------------
int CWidget::Method_0x20(int context)
{
    return CAgent::Method_0x20(context);
}

//----------------------------------------------------------------------------
//  0x6F2AC330 - vtable slot 9 (+0x24).
//----------------------------------------------------------------------------
int CWidget::Method_0x24(int context)
{
    return CAgent::Method_0x24(context);
}
