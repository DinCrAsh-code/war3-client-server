//============================================================================
//  0x6F7E89D0 (guard) and its body - AsyncFileCancelList.  Drain a whole
//  list of operations back into the owning file's pool.
//
//  The guard at 0x6F7E89D0 is the `if (!ops->empty())` an inlining left in
//  front of the body; IDA gives the two one heading, so they are one
//  function here.
//============================================================================
#include "asyncfilefind.h"

void __fastcall AsyncFileCancelList(void* key, TSExplicitList<ASYNCFILEOP>* ops)
{
    if (ops->m_terminator.m_prevlink <= 0)
        return;

    if (key == 0)
        return;

    AsyncFileLock(0);

    ASYNCFILE* file = AsyncFileFind(key);
    if (file != 0)
    {
        for (;;)
        {
            int node = ops->m_terminator.m_prevlink;
            if (node <= 0)
                break;

            ((ASYNCFILEOP*)node)->m_link.Unlink();
            file->m_pool.Free((void*)node, 0x30, ".?AUASYNCFILEOP@@", -2);
        }
    }

    AsyncFileUnlock(0);
}
