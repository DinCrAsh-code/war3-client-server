//============================================================================
//  0x6F7E7E10 - AsyncFileClose.  Two passes under the registry lock: the
//  first points the file's close event at a local one and asks its transport
//  to shut down, the second - once the transport has answered - dispatches
//  the file's own vtable slot 0 to finish the teardown.
//
//  The two passes take the lock in *different modes* (0 then 1), which is
//  why the lookup is written out twice rather than hoisted.
//============================================================================
#include "asyncfilefind.h"

//: 0x6F6DB280 - retn 0.  Dispatch the transport's vtable slot 9 (+0x24).
void __fastcall NetConnDispatchSlot0x24(void* conn);

void __fastcall AsyncFileClose(ASYNCFILE* file)
{
    Event done(FALSE, FALSE);

    if (file != 0)
    {
        int waiting = 0;

        AsyncFileLock(0);

        ASYNCFILE* found = AsyncFileFind(file);
        if (found == 0)
        {
            AsyncFileUnlock(0);
        }
        else
        {
            found->m_closeEvent = &done;

            void* transport = found->m_transport;
            if (transport != 0)
            {
                waiting = 1;
                NetConnDispatchSlot0x24(transport);
            }

            AsyncFileUnlock(0);

            if (waiting)
                done.Wait(INFINITE);
        }

        AsyncFileLock(1);

        ASYNCFILE* again = AsyncFileFind(file);
        if (again != 0)
        {
            typedef void (__thiscall *AbortFn)(void*, int);
            ((AbortFn)again->m_vtable[0])(again, 1);
        }

        AsyncFileUnlock(1);
    }
}
