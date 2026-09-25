//============================================================================
//  0x6F6D84B0 - see evtsched.h.
//============================================================================
#include "evtsched.h"

DWORD __fastcall WaitForEvents(unsigned int count, Event* const* events,
                                BOOL bWaitAll, DWORD dwMilliseconds)
{
    if (count > 64)
        return (DWORD)-1;

    HANDLE handles[64];
    unsigned int n = 0;

    for (unsigned int i = 0; i < count; i++)
    {
        Event* e = events[i];
        if (e && e->m_handle)
            handles[n++] = e->m_handle;
    }

    return WaitForMultipleObjects(n, handles, bWaitAll, dwMilliseconds);
}
