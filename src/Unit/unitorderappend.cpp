//============================================================================
//  0x6F2A0650 - CUnit::AppendOrder: put this order on the queue rather than
//  making it the current one.
//
//  The same two refusals StartOrderNow has, and then one of two splices:
//
//    * with an order already running and `afterCurrent` set, the new one
//      goes in directly behind it and inherits its successor; the tail only
//      moves if there was no successor;
//    * otherwise it becomes the head with no successor of its own, and the
//      tail moves only if the tail was empty or no longer resolves.
//
//  Then the length goes up by one and, if no task is running, the order is
//  begun - with the caller's own flag rather than StartOrderNow's fixed 1.
//
//  There is no announcement here and no SEH frame: nothing in this body is
//  destructible, which is why it is the one function in this group that can
//  reach its own instruction count.
//
//  Own translation unit: SubmitOrder calls it for real.
//============================================================================
#include "unit.h"
#include "unitorder.h"

void CUnit::AppendOrder(void* order, int afterCurrent, int flags)
{
    if ((m_flags5C & 0x100) != 0)
        return;

    if (m_orderQueueLength > kOrderQueueLimit)
        return;

    //  Written as the shipped control-flow graph rather than as an
    //  if/else, because that graph is not one: the splice-behind-the-
    //  current-order case is a block *after* the function's own `retn`
    //  that jumps back into the simple path's tail, either to the shared
    //  "make this the tail" call or straight to the length increment.
    //  Three if/else spellings and a two-`goto` one were measured; all of
    //  them make MSVC put the splice inline and the simple path second,
    //  which costs the whole second half of the function.
    SOptionalHandleRef* head = (SOptionalHandleRef*)&m_ref19C;
    SOptionalHandleRef* tail = (SOptionalHandleRef*)&m_ref1A8;
    COrder* fresh = (COrder*)order;
    COrder* current;
    COrder* after;

    if (((int)head->m_handle & head->m_typeTag) != -1)
    {
        current = (COrder*)QueryHandleField0x54(head);
        if (current != 0 && afterCurrent != 0)
            goto splice;
    }

    ((SOptionalHandleRefResolver*)head)->ResolveChained(order);
    ((SOptionalHandleRefResolver*)&fresh->m_next)->ResolveChained(0);

    if ((tail->m_typeTag & (int)tail->m_handle) != -1
        && QueryHandleField0x54(tail) != 0)
        goto counted;

setTail:
    ((SOptionalHandleRefResolver*)tail)->ResolveChained(order);

counted:
    m_orderQueueLength += 1;

    SOptionalHandleRef* task = (SOptionalHandleRef*)&m_ref174;
    if (((int)task->m_handle & task->m_typeTag) == -1
        || QueryHandleField0x54(task) == 0)
        BeginOrder(order, flags);
    return;

splice:
    after = 0;
    if ((current->m_next.m_typeTag & (int)current->m_next.m_handle) != -1)
        after = (COrder*)QueryHandleField0x54(&current->m_next);

    ((SOptionalHandleRefResolver*)&current->m_next)->ResolveChained(order);
    ((SOptionalHandleRefResolver*)&fresh->m_next)->ResolveChained(after);

    if (after != 0)
        goto counted;
    goto setTail;
}
