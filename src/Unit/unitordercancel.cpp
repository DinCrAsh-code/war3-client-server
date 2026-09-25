//============================================================================
//  0x6F2831A0 - CUnit::CancelCurrentOrder: throw away everything the unit
//  has queued *behind* whatever it is doing now.
//
//  The walk starts at the current order's own successor (+0x2C), not at the
//  head, which is what makes this "cancel the queue" rather than "cancel
//  the order": each order from the second one onwards gets a
//  kEventOrderCancelled announcement naming it and is then released through
//  its own slot 23.  The successor is taken *before* the release, because
//  the release is what makes the object go away.
//
//  Afterwards the tail is set back to the head - a three-word copy, because
//  the word at +0x1A4 goes with the pair in front of it - the length is
//  zeroed, and if the head is still there its own successor link is cut and
//  the length becomes one.  That is the queue reduced to exactly the order
//  that was already running.
//
//  The `event.m_vtable = &g_vftTRefCnt;` at the end of the loop body is not
//  bookkeeping this source invented: it is the shipped ~CEvent's own
//  inlined base-class destructor, one store per iteration, and it is in the
//  instruction stream.  What is *not* reproducible is the SEH frame that
//  destructor lives in - the same __except_handler4-shaped frame
//  docs/targets/JASS_Location.md records for every scoped destructible
//  local in this binary.
//
//  Own translation unit: PrepareForNewOrder and SubmitOrder both call it.
//============================================================================
#include "unit.h"
#include "unitorder.h"

void CUnit::CancelCurrentOrder()
{
    SOptionalHandleRef* head = (SOptionalHandleRef*)&m_ref19C;
    if ((head->m_typeTag & (int)head->m_handle) != -1)
    {
        COrder* current = (COrder*)QueryHandleField0x54(head);
        if (current != 0)
        {
            COrder* order = (COrder*)ResolveHandleRefOrNull(&current->m_next);
            while (order != 0)
            {
                COrder* next =
                    (COrder*)ResolveHandleRefOrNull(&order->m_next);

                SOrderEvent event;
                event.m_reserved04 = 0;
                event.m_vtable = &g_vftCEvent;
                event.m_msgId = kEventOrderCancelled;
                event.m_pOrder = order;
                ((PostAgentEventFn)(*(void***)this)[0x10 / 4])(this, &event);

                ((OrderReleaseFn)(*(void***)order)[0x5C / 4])(order);

                event.m_vtable = &g_vftTRefCnt;

                order = next;
            }
        }
    }

    m_orderQueueLength = 0;
    m_ref1A8 = m_ref19C;
    m_int1B0 = m_int1A4;

    if ((head->m_typeTag & (int)head->m_handle) != -1)
    {
        COrder* current = (COrder*)QueryHandleField0x54(head);
        if (current != 0)
        {
            current->m_next.m_typeTag = -1;
            current->m_next.m_handle = 0xFFFFFFFF;
            m_orderQueueLength += 1;
        }
    }
}
