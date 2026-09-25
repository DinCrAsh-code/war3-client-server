//============================================================================
//  0x6F7E7D40 - AsyncFileFlush.  Point the file's flush event at a local one
//  and wait on it if anything is still in flight.
//
//  The local Event is constructed unconditionally, before the null check,
//  and destroyed unconditionally at the end: the shipped stream builds it
//  first and its unwind funclet destroys it, which is what puts a __try
//  frame on the shipped function that this build at /EHs-c- has no way to
//  emit.
//============================================================================
#include "asyncfilefind.h"

void __fastcall AsyncFileFlush(ASYNCFILE* file)
{
    Event done(FALSE, FALSE);

    if (file != 0)
    {
        AsyncFileLock(1);

        ASYNCFILE* found = AsyncFileFind(file);
        if (found == 0)
        {
            AsyncFileUnlock(1);
        }
        else
        {
            int waiting = found->m_pending != 0;
            found->m_flushEvent = &done;
            AsyncFileUnlock(1);

            if (waiting)
                done.Wait(INFINITE);
        }
    }
}
