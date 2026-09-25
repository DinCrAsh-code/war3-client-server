//============================================================================
//  0x6F7E72E0 - AsyncFileDropPending.  Drop the pending count, and if that
//  was the last one and somebody is waiting on a flush, wake them.
//============================================================================
#include "asyncfile.h"

//  src/Sync/critsec.cpp already reconstructs this one; `volatile long*` is
//  its declaration, and anything else here would be a second symbol.
long __fastcall InterlockedDecrementAt(volatile long* target);   // 0x6F6C4070

void __fastcall AsyncFileDropPending(ASYNCFILE* file)
{
    if (InterlockedDecrementAt((volatile long*)&file->m_pending) != 0)
        return;

    if (file->m_flushEvent == 0)
        return;

    file->m_flushEvent->Set();
}
