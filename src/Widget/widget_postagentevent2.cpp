//============================================================================
//  0x6F26EF10 - CWidget::PostAgentEvent2, from the CUnit vtable closure
//  walk (cunit_agent2_worklist.md).  Byte-for-byte the same shape as
//  CWidget::PostAgentEvent (widget_postagentevent.cpp), one event id
//  higher: 0xD01A2 instead of 0xD01A1.
//============================================================================
#include "widget.h"
#include "observer.h"

void CWidget::PostAgentEvent2(void* target, int direct)
{
    if (direct != 0)
        Dispatch(0xD01A2, 0xD01A2, target);
    else
        CObserver::PostEvent(0xD01A2, target);
}
