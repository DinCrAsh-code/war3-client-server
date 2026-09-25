//============================================================================
//  0x6F4C3C40 - CStatus::ENTRYLIST::FreeAll: drain the list, giving each
//  entry's text back to Storm and each entry back to its pool.
//
//  The tail is re-read at the top of every iteration rather than stepped,
//  because unlinking the tail is what the loop does - the raw `TailLink()`
//  and the `jle` on its sign are storm.h's own walk shape.
//============================================================================
#include "cstatus.h"

static const char kStatusHeader[] =
    "e:\\drive1\\temp\\buildwar3x\\engine\\source\\base\\Status.h";

void CStatus::ENTRYLIST::FreeAll()
{
    for (;;)
    {
        int tail = TailLink();
        if (tail <= 0)
            break;

        STATUSENTRY* entry = (STATUSENTRY*)tail;

        if (entry->m_text != 0)
            SMemFree(entry->m_text, kStatusHeader, 0x25, 0);

        entry->m_link.Unlink();
        g_statusEntryPool.Free(entry, 0, 0);
    }
}
