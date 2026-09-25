//============================================================================
//  0x6F283340 - CUnit::NotifyOrderTarget: pop the order at the head of the
//  queue and tell it that it is no longer the one being carried out.
//
//  Four steps, in the shipped order:
//
//    * clear bit 2 of the order's own +0x20 flag word - it stops being the
//      order its target is tracking;
//    * take one off the queue length at +0x1B4;
//    * make the order's own successor (+0x2C) the new head, through
//      SOptionalHandleRefResolver::ResolveChained, which is what re-reads
//      the successor's {handle, typeTag} pair out of the object itself;
//    * if the queue is now empty - the new head unset, or set but no longer
//      resolvable - reset the tail at +0x1A8 to the same all-ones sentinel.
//
//  Then the order is released through its own slot 23, as a tail jump.
//
//  The two -1 stores go high offset first (+0x1AC before +0x1A8), which is
//  what one assignment per word in that order produces; CUnit::
//  PrepareForNewOrder has the identical pair and the identical order.
//
//  Own translation unit: SubmitOrder reaches it with a real call.
//============================================================================
#include "unit.h"
#include "unitorder.h"

void CUnit::NotifyOrderTarget()
{
    SOptionalHandleRef* head = (SOptionalHandleRef*)&m_ref19C;
    if ((head->m_typeTag & (int)head->m_handle) == -1)
        return;

    COrder* order = (COrder*)QueryHandleField0x54(head);
    if (order == 0)
        return;

    order->m_flags20 &= 0xFFFFFFFB;
    m_orderQueueLength += -1;

    void* next = ResolveHandleRefOrNull(&order->m_next);
    ((SOptionalHandleRefResolver*)head)->ResolveChained(next);

    if ((head->m_typeTag & (int)head->m_handle) == -1
        || QueryHandleField0x54(head) == 0)
    {
        m_ref1A8.m_b = 0xFFFFFFFF;
        m_ref1A8.m_t = 0xFFFFFFFF;
    }

    ((OrderReleaseFn)(*(void***)order)[0x5C / 4])(order);
}
