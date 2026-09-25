//============================================================================
//  0x6F6DF4D0 - CONNSTATE::FreeAll.  Eight shards, each drained under its own
//  lock: every connection still on the shard is told its refcount hit zero
//  (vtable slot 4) and then freed, and the shard count comes down by one per
//  entry.
//
//  Both loops re-read rather than caching: OnZeroRefCount unlinks, so the
//  inner walk is the same `while ((int)tail > 0)` drain the rest of this
//  module uses, and the outer one steps the list and the lock separately
//  because the two arrays have different strides (0x0C and 0x18).
//============================================================================
#include "ostcp.h"

namespace OsNet
{


void CONNSTATE::FreeAll()
{
    for (int shard = 0; shard < 8; shard++)
    {
        EnterCriticalSection(&m_locks[shard]);

        for (;;)
        {
            NETCONN* conn = (NETCONN*)m_lists[shard].TailLink();
            if ((int)conn <= 0)
                break;

            conn->DeleteSelf(0);   //  slot 4; renamed from OnZeroRefCount, netconn.h
            SMemFree(conn, ".?AVNETCONN@OsNet@@", -2, 0);
            InterlockedDecrement(&m_count);
        }

        LeaveCriticalSection(&m_locks[shard]);
    }
}

}  // namespace OsNet
