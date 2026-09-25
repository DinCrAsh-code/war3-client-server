//============================================================================
//  0x6F7E8340 - ASYNCFILE_QueueOp.  Take a record from the pool, fill in the
//  caller's six words, pick the next unused id and hang the record on that
//  id's bucket.
//
//  Two things here are the shipped code's and look wrong written down:
//
//  * the collision scan reads `op->m_id` - the *stale* id still in the
//    record the pool just handed back - to choose which bucket to search,
//    where the insert below uses the candidate id.  Reproduced as-is; a
//    scan of `candidate & 7` would be a different bucket and a different
//    instruction;
//  * the id counter is only checked for collisions once it has wrapped, so
//    the first four billion operations skip the scan entirely.
//============================================================================
#include "asyncfile.h"

void __fastcall EnterCritSec(CRITICAL_SECTION* cs);   // 0x6F6D83A0
void __fastcall LeaveCritSec(CRITICAL_SECTION* cs);   // 0x6F6D83B0

unsigned int ASYNCFILE::QueueOp(void* callerLock, int a0, int a1, int a2,
                                int a3, int a4, int a5)
{
    ASYNCFILE* file = this;
    ASYNCFILEOP* op = file->m_pool.Alloc();

    op->m_lock = callerLock;
    op->m_arg0 = a0;
    op->m_arg1 = a1;
    op->m_arg2 = a2;
    op->m_arg3 = a3;
    op->m_arg4 = a4;
    op->m_arg5 = a5;
    op->m_state = 0;

    EnterCritSec(&file->m_lock);

    unsigned int id;
    for (;;)
    {
        file->m_nextId = file->m_nextId + 1;
        id = file->m_nextId;
        if (id == 0)
        {
            file->m_idWrapped = 1;
            continue;
        }

        if (file->m_idWrapped == 0)
            break;

        int probe = file->m_buckets[op->m_id & 7].m_terminator.m_prevlink;
        probe = probe > 0 ? probe : 0;
        while (probe > 0)
        {
            if (((ASYNCFILEOP*)probe)->m_id == id)
                break;
            probe = ((ASYNCFILEOP*)probe)->m_link.m_prevlink;
        }
        if (probe <= 0)
            break;
    }

    op->m_id = id;
    op->m_link.Unlink();

    TSExplicitList<ASYNCFILEOP>* bucket = &file->m_buckets[id & 7];
    TSLink<ASYNCFILEOP>* head = bucket->m_terminator.m_next;
    op->m_link.m_next = head;
    op->m_link.m_prevlink = head->m_prevlink;
    head->m_prevlink = (int)op;
    bucket->m_terminator.m_next = &op->m_link;

    LeaveCritSec(&file->m_lock);
    return id;
}
