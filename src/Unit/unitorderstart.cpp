//============================================================================
//  0x6F2A0510 - CUnit::StartOrderNow: make this order the one the unit is
//  carrying out, ahead of everything already queued.
//
//  Two refusals first - orders switched off at +0x5C bit 8, and a queue
//  already at kOrderQueueLimit - and then four links:
//
//    * whatever the tail names gets the new order as its successor;
//    * the tail becomes the new order;
//    * if the head is empty (or names something that no longer resolves)
//      the head becomes the new order too;
//    * the length goes up by one.
//
//  Then one of two things happens, and which one is what makes this
//  "now" rather than "queued": with no task running the order is begun
//  immediately, and with one running only a kEventOrderIssued announcement
//  goes out and the order waits its turn.
//
//  The three handle-pair "is this set" tests are written out rather than
//  pushed through unitorder.h's own helper, because the shipped code does
//  not spell them the same way twice: the tail's is one load and an AND
//  from memory, the head's and the task chain's are two loads and a
//  register AND.  Both operand orders were measured here and MSVC picks
//  its own either way, so the source keeps the shipped reading order and
//  claims nothing more.
//
//  The scoped event puts the tail of this body inside the same
//  unreproducible __except_handler4-shaped frame docs/targets/
//  JASS_Location.md records; nothing else here is out of reach.
//
//  Own translation unit: SubmitOrder calls it for real.
//============================================================================
#include "unit.h"
#include "unitorder.h"

void CUnit::StartOrderNow(void* order)
{
    if ((m_flags5C & 0x100) != 0)
        return;

    if (m_orderQueueLength > kOrderQueueLimit)
        return;

    SOptionalHandleRef* tail = (SOptionalHandleRef*)&m_ref1A8;
    if (((int)tail->m_handle & tail->m_typeTag) != -1)
    {
        COrder* last = (COrder*)QueryHandleField0x54(tail);
        if (last != 0)
            ((SOptionalHandleRefResolver*)&last->m_next)->ResolveChained(order);
    }
    ((SOptionalHandleRefResolver*)tail)->ResolveChained(order);

    SOptionalHandleRef* head = (SOptionalHandleRef*)&m_ref19C;
    if (((int)head->m_handle & head->m_typeTag) == -1
        || QueryHandleField0x54(head) == 0)
        ((SOptionalHandleRefResolver*)head)->ResolveChained(order);

    m_orderQueueLength += 1;

    SOptionalHandleRef* task = (SOptionalHandleRef*)&m_ref174;
    if (((int)task->m_handle & task->m_typeTag) == -1
        || QueryHandleField0x54(task) == 0)
    {
        BeginOrder(order, 1);
        return;
    }

    SOrderEvent issued;
    issued.m_reserved04 = 0;
    issued.m_vtable = &g_vftCEvent;
    issued.m_msgId = kEventOrderIssued;
    issued.m_pOrder = order;
    ((PostAgentEventFn)(*(void***)this)[0x10 / 4])(this, &issued);
}
