//============================================================================
//  0x6F26F970 - CUnit::Method_0x26F970: build a stack SOrderEvent stamped
//  with message id 0xD01DE and `m_pOrder = this`, and hand it to the
//  object's own slot 4 (+0x10, PostAgentEventFn) - the same "announce
//  yourself" shape CUnit::FinishCurrentOrder uses for kEventOrderFinished
//  (unitorderfinish.cpp), except the fourth word carries the unit itself
//  rather than a CTaskAction.
//
//  Reached from sub_6F284950 (MsgHandler_6F284950, unit_dispatch.cpp,
//  0xD01D4) off CUnit's own slot-3 message dispatcher (0x6F2A7E60).  CUnit
//  batch-3 closure.
//============================================================================
#include "unit.h"
#include "unitorder.h"     // SOrderEvent, g_vftCEvent, PostAgentEventFn

void CUnit::Method_0x26F970()
{
    SOrderEvent evt;
    evt.m_vtable = &g_vftCEvent;
    evt.m_reserved04 = 0;
    evt.m_msgId = 0xD01DE;
    evt.m_pOrder = this;
    ((PostAgentEventFn)(*(void***)this)[0x10 / 4])(this, &evt);
}
