//============================================================================
//  0x6F2A4AB0 - CUnit::SubmitOrder: give the unit an order it has just been
//  handed, either replacing whatever it is doing or joining the queue.
//
//  Three tests decide, in this order:
//
//    * +0x5C bit 8 (0x100) - orders are refused outright and nothing else
//      runs;
//    * +0x198, the number of orders already queued.  With work queued the
//      decision is only "replace or append";
//    * with nothing queued, IsOrderAlreadyActive() first, because an order
//      the unit is already carrying out is not worth restarting.
//
//  Everything it calls lives below this dump's depth cut and is declared as
//  a redirect in unitorderthunks.cpp; each one's argument count is read off
//  its own `retn <n>` in the shipped image, which is the only thing that
//  can be got wrong here without the score noticing.
//
//  The two calls to CancelCurrentOrder with no `mov ecx,esi` in front of
//  them are not free functions: the shipped code keeps `this` in ecx from
//  entry (nothing writes ecx before either site) and MSVC drops the
//  redundant copy, which is exactly what it does again here - the third
//  call to the same function, at a point where ecx has been clobbered,
//  does have the copy.
//
//  Own translation unit: CUnit::IssueStopOrder reaches it with a real call.
//============================================================================
#include "unit.h"
#include "unitorder.h"
#include "itemhandleresolve.h"

void CUnit::SubmitOrder(void* order, int replaceCurrent, int flags)
{
    SUnitOrderState* self = (SUnitOrderState*)this;

    if ((self->m_orderFlags & 0x100) != 0)
        return;

    if (self->m_queuedOrderCount > 0)
    {
        if (replaceCurrent)
        {
            CancelCurrentOrder();
            StartOrderNow(order);
            return;
        }

        AppendOrder(order, 1, flags);
        return;
    }

    if (IsOrderAlreadyActive(order))
    {
        if (replaceCurrent)
            CancelCurrentOrder();
        return;
    }

    if (replaceCurrent)
    {
        PrepareForNewOrder();
        StartOrderNow(order);
        return;
    }

    FinishCurrentOrder();

    //  The "not set" sentinel of the embedded {handle, typeTag} pair at
    //  +0x19C: both fields all-ones, tested as one AND against -1 exactly
    //  the way itemhandleresolve.h describes.  Set, resolvable, and with
    //  bit 3 of the resolved object's +0x20 up, the target gets told.
    if ((self->m_orderTargetRef.m_handle &
         (unsigned int)self->m_orderTargetRef.m_typeTag) != 0xFFFFFFFF)
    {
        char* resolved = (char*)QueryHandleField0x54(&self->m_orderTargetRef);
        if (resolved && (resolved[0x20] & 8) != 0)
            NotifyOrderTarget();
    }

    AppendOrder(order, 0, flags);
}
