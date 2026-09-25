//============================================================================
//  0x6F7E8440 - ASYNCFILE_CancelOp.  Find the operation with this id in its
//  own bucket, unlink it and give the record back to the pool.
//
//  Two LeaveCritSec calls, not one: the shipped stream leaves the lock and
//  returns from inside the not-found arm rather than falling through to a
//  shared tail.
//============================================================================
#include "asyncfile.h"

void __fastcall EnterCritSec(CRITICAL_SECTION* cs);   // 0x6F6D83A0
void __fastcall LeaveCritSec(CRITICAL_SECTION* cs);   // 0x6F6D83B0

void ASYNCFILE::CancelOp(unsigned int id)
{
    ASYNCFILE* file = this;
    EnterCritSec(&file->m_lock);

    int node = file->m_buckets[id & 7].m_terminator.m_prevlink;
    node = node > 0 ? node : 0;

    while (node > 0)
    {
        if (((ASYNCFILEOP*)node)->m_id == id)
        {
            ((ASYNCFILEOP*)node)->m_link.Unlink();
            file->m_pool.Free((void*)node, 0x30, ".?AUASYNCFILEOP@@", -2);
            LeaveCritSec(&file->m_lock);
            return;
        }
        node = ((ASYNCFILEOP*)node)->m_link.m_prevlink;
    }

    LeaveCritSec(&file->m_lock);
}
