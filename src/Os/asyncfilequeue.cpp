//============================================================================
//  0x6F7E85C0 - AsyncFileQueue.  Look the file up, queue an operation on it,
//  count it as pending and hand it to the transport; back the whole thing
//  out again if the transport refuses.
//
//  The transport call takes six arguments through a __thiscall forwarder
//  (0x6F6DF290) whose second lands in edx, which is why the caller's own
//  edx argument is spilled to a stack slot on entry and read back for it.
//============================================================================
#include "asyncfilefind.h"

//: 0x6F6DF290 - retn 10h.  Submit one operation to the transport.
int __fastcall NetConnSubmitAsyncOp(void* conn, int a1, int a0, int a2,
                                    int a3, unsigned int id);

unsigned int __fastcall AsyncFileQueue(ASYNCFILE* file, int a0, int a1,
                                       int a2, int a3, int a4, int a5)
{
    unsigned int id = 0;

    if (file == 0)
        return 0;

    AsyncFileLock(0);

    ASYNCFILE* found = AsyncFileFind(file);
    if (found != 0 && found->m_transport != 0)
    {
        id = found->QueueOp(0, a1, a0, a2, a3, a4, a5);
        AsyncFileAddPending(found);

        if (!NetConnSubmitAsyncOp(found->m_transport, a0, a1, a2, a3, id))
        {
            found->CancelOp(id);
            AsyncFileDropPending(found);
            id = 0;
        }
    }

    AsyncFileUnlock(0);
    return id;
}
