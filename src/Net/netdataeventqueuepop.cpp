//============================================================================
//  0x6F549180 - CNetEventQueue::Pop.  See netdataeventqueue.h for the
//  file-split rationale.
//============================================================================
#include "netdataeventqueue.h"

//  Pop one node: `node` when the caller already has one to remove out of
//  order, otherwise the current tail if the queue is non-empty
//  (TailLink() > 0), otherwise null. DrainNetDataEventQueue (netdataeventqueue
//  drain.cpp) always passes null, so only the tail-or-empty path is ever
//  actually exercised from here. UnlinkAndCount's own return value is not
//  used.
void* CNetEventQueue::Pop(void* node)
{
    void* target = node;
    if (!target)
    {
        int tail = m_list.m_terminator.m_prevlink;
        if (tail <= 0)
            return 0;
        target = (void*)tail;
    }

    UnlinkAndCount(target);
    return target;
}
