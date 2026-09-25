//============================================================================
//  0x6F2AB460 - CWidget::NotifyLifeChanged, from the CUnit vtable closure
//  walk (cunit_agent2_worklist.md).  Was a naked redirect.
//
//  The same four-word CEvent record every member of this family stamps
//  (SOrderEvent, unitorder.h; SDayNightEvent, agentdaynight.cpp;
//  PostReloadRequestedEvent, funcmap.py BEHAVIOUR) - vtable, a zeroed word,
//  the message id (0xD01E6 here), and `this` as the fourth word - built on
//  the stack and dispatched through this object's own vtable slot 4.
//
//  0.27ish expected, not 1.000: the shipped body wraps the record in a
//  real, frame-pointer-omitted, cookie-XORed `__except_handler4` frame
//  (this build's fixed `/GS-` cannot reproduce the cookie half of that at
//  all - see docs/msvc-vc8-idioms.md), the same gap
//  PostReloadRequestedEvent (17/45) and CUnit::CancelCurrentOrder (49/105)
//  already carry for a scoped CEvent. Only the frame differs; no branch,
//  constant or dispatched slot does.
//============================================================================
#include "widget.h"
#include "unitorder.h"   // SOrderEvent's shape, g_vftCEvent, PostAgentEventFn

void CWidget::NotifyLifeChanged()
{
    SOrderEvent event;
    event.m_vtable = &g_vftCEvent;
    event.m_reserved04 = 0;
    event.m_msgId = 0xD01E6;
    event.m_pOrder = this;

    ((PostAgentEventFn)(*(void***)this)[0x10 / 4])(this, &event);
}
