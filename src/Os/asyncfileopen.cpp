//============================================================================
//  0x6F7E89E0 - AsyncFileOpen.  Allocate the ASYNCFILE, register it, ask the
//  transport layer to open `path`, and wait for the answer.
//
//  If the open failed the registry entry is aborted through the file's own
//  vtable slot 0 and null comes back; if it succeeded the open event is
//  cleared so nothing signals a dead stack object later.  Which of those two
//  the second pass does is decided on `m_transport`, not on the result of
//  the open.
//============================================================================
#include "asyncfilefind.h"

static const char kAsyncFileCpp[] = ".\\AsyncFile.cpp";

//: 0x6F7E8200 - retn 0.  Zero the record and build its pool, lock and eight
//: buckets.
ASYNCFILE* __fastcall AsyncFileConstruct(void* block);

//: 0x6F7E79C0 - retn 4.  Put the record on the registry list under the open
//: lock and hand back the key it is found by.
ASYNCFILE* __fastcall AsyncFileRegister(CRITICAL_SECTION* lock, ASYNCFILE* file);

//: 0x6F7E87E0 - the completion callback the transport calls back on.
void __stdcall AsyncFileOpenComplete();

//: 0x6F6E2B40 - retn 8.  Start the open on the transport layer.
void __fastcall OsNetOpenAsyncFile(const char* path, void* callback,
                                   ASYNCFILE* file, int context);

ASYNCFILE* __fastcall AsyncFileOpen(const char* path, int context)
{
    Event ready(FALSE, FALSE);

    void* block = SMemAlloc(0xC8, kAsyncFileCpp, 0x116, 0);
    ASYNCFILE* built = block ? AsyncFileConstruct(block) : 0;

    built->m_openEvent = &ready;

    ASYNCFILE* file = AsyncFileRegister(&g_asyncFileOpenLock, built);

    OsNetOpenAsyncFile(path, (void*)AsyncFileOpenComplete, file, context);
    ready.Wait(INFINITE);

    if (file != 0)
    {
        AsyncFileLock(1);

        ASYNCFILE* found = AsyncFileFind(file);
        if (found != 0)
        {
            if (found->m_transport == 0)
            {
                typedef void (__thiscall *AbortFn)(void*, int);
                file = 0;
                ((AbortFn)found->m_vtable[0])(found, 1);
            }
            else
            {
                found->m_openEvent = 0;
            }
        }

        AsyncFileUnlock(1);
    }

    return file;
}
