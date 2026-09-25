//============================================================================
//  0x6F2A4A10 - CUnit::PrepareForNewOrder: throw the whole order queue away
//  so that a fresh order can become the only one.
//
//  Cancel what is running, stop the unit moving if it is (+0x5C bit 20), finish the current order, then walk the
//  intrusive queue from +0x19C releasing every order through its own
//  slot 23 - taking each one's successor *before* releasing it, because the
//  release is what makes the object go away.  Head, tail and length are
//  then reset together.
//
//  The four -1 stores all come out of one register: the shipped code
//  materialises -1 once and writes it to +0x19C+4, +0x19C, +0x1AC and
//  +0x1A8 in that order, which is high word before low word in each pair -
//  the same order CUnit::NotifyOrderTarget writes its own two.
//
//  Own translation unit: CancelCurrentOrder, StopMovement and
//  FinishCurrentOrder are all real calls out of it.
//============================================================================
#include "unit.h"
#include "unitorder.h"

void CUnit::PrepareForNewOrder()
{
    CancelCurrentOrder();

    if ((m_flags5C & 0x100000) != 0)
        StopMovement();

    FinishCurrentOrder();

    SOptionalHandleRef* head = (SOptionalHandleRef*)&m_ref19C;
    if ((head->m_typeTag & (int)head->m_handle) != -1)
    {
        COrder* order = (COrder*)QueryHandleField0x54(head);
        while (order != 0)
        {
            COrder* next = (COrder*)ResolveHandleRefOrNull(&order->m_next);
            ((OrderReleaseFn)(*(void***)order)[0x5C / 4])(order);
            order = next;
        }
    }

    head->m_typeTag = -1;
    head->m_handle = 0xFFFFFFFF;
    m_ref1A8.m_b = 0xFFFFFFFF;
    m_ref1A8.m_t = 0xFFFFFFFF;
    m_orderQueueLength = 0;
}
