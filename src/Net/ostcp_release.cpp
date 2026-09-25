//============================================================================
//  OsNet::IOTCPCONN::ReleasePending (0x6F6E0670) - drop one outstanding-I/O
//  reference, tearing the connection down on the last one.
//
//  Its own translation unit so IOTCPCONN's constructor keeps the call.
//============================================================================
#include "ostcp.h"

namespace OsNet
{

//----------------------------------------------------------------------------
//  0x6F6E0670.
//
//  The lock is *not* released on the tear-down path, and that is the shipped
//  code, not a transcription slip: the early `return` below is a separate
//  epilogue in the dump with no LeaveCriticalSection in front of it, while the
//  other path has one.  It is survivable only because the object is on its way
//  out - Shutdown() drops the last reference, and ~NETCONN deletes the
//  critical section outright rather than leaving anything to wait on it.
//----------------------------------------------------------------------------
void IOTCPCONN::ReleasePending()
{
    EnterCriticalSection(&m_critSec);

    if (InterlockedDecrement(&m_pending) == 0)
    {
        OnPendingDrained();
        Shutdown(1);
        return;
    }

    LeaveCriticalSection(&m_critSec);
}

}  // namespace OsNet
