//============================================================================
//  0x6F2AB1E0 - CWidget's "tell the world this widget changed" post.  Not
//  a vtable slot: CUnit::Deactivate (unit_deactivate.cpp) reaches it with
//  a direct call, and it is in CWidget's own 0x6F2ABxxx neighbourhood.
//
//  One event id, 0xD01A1, and two ways to deliver it.  With `direct` set
//  it goes through this widget's own CObserver slot 2 (+0x08) with the id
//  twice - once as the low bound and once as the high bound of a
//  single-event range, which is what an event *range* dispatcher looks
//  like when the range is one event wide.  With `direct` clear it is a
//  tail call into CObserver's own post (0x6F62A570), and the two
//  `mov [esp+...]` stores before the jump are MSVC rewriting this
//  function's own incoming argument slots into the callee's - the standard
//  /O2 tail-call shape, not two stores to anything real.
//============================================================================
#include "widget.h"
#include "observer.h"

void CWidget::PostAgentEvent(void* target, int direct)
{
    if (direct != 0)
        Dispatch(0xD01A1, 0xD01A1, target);
    else
        CObserver::PostEvent(0xD01A1, target);
}
