//============================================================================
//  0x6F6E3A00 - LISTENSLOTS::ReleaseAll.  Under the one shard's lock, close
//  and free every TCPLISTEN still on the list, decrementing the count once
//  per entry.
//
//  The walk re-reads the tail on every step rather than caching a `next`,
//  because TCPLISTEN::Close() is what unlinks the node it is given - so the
//  loop is `while ((int)tail > 0)`, exactly the shape the other drains in
//  this repo use.
//============================================================================
#include "ostcp.h"

namespace OsNet
{


void LISTENSLOTS::ReleaseAll()
{
    EnterCriticalSection(&m_lock);

    for (;;)
    {
        TCPLISTEN* entry = (TCPLISTEN*)m_list.TailLink();
        if ((int)entry <= 0)
            break;

        entry->Close();
        SMemFree(entry, ".?AUTCPLISTEN@OsNet@@", -2, 0);
        InterlockedDecrement(&m_count);
    }

    LeaveCriticalSection(&m_lock);
}

}  // namespace OsNet
