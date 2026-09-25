//============================================================================
//  0x6F6E1570 - TCPMGR::StopSelectThreads.  Wake and join the listen thread,
//  the select thread and the notify thread in that order, then walk the
//  listen slots closing every socket still open.
//
//  The socket walk is a SLOTITER over m_listens: the iterator holds the lock
//  of the shard it is on, so the walk is `Next()` / act / `Next()` with the
//  Enter before the first step and the Leave computed from the shard the
//  iterator finished on, and it stops when it comes back to the node it
//  started at.
//============================================================================
#include "ostcp.h"

namespace OsNet
{

void TCPMGR::StopSelectThreads()
{
    m_listenStopping = 1;
    SetEvent(m_listenWakeEvent);
    WaitForSingleObject(m_listenDoneEvent, INFINITE);

    g_osTcpSelectStopping = 1;
    SetEvent(m_pollEvent);

    if (m_selectThread != 0)
    {
        SetThreadPriority(m_selectThread, THREAD_PRIORITY_NORMAL);
        WaitForSingleObject(m_selectThread, INFINITE);
        m_selectThread = 0;
    }

    if (m_pollThread != 0)
    {
        WaitForSingleObject(m_pollThread, INFINITE);
        m_pollThread = 0;
    }

    if (m_iocp == 0)
        JoinPumpThreads();

    CloseHandle(m_pollEvent);
    m_pollEvent = 0;

    SLOTITER walk;
    walk.m_owner = &m_listens;
    walk.m_current = 0;
    walk.m_next = 0;
    walk.m_start = 0;
    walk.m_shard = 0;

    EnterCriticalSection(&m_listens.m_lock);
    walk.Next();

    if (walk.m_current != 0)
    {
        for (;;)
        {
            ((TCPLISTEN*)walk.m_current)->CloseSocket();
            walk.Next();
            if (walk.m_current == walk.m_start)
                break;
            if (walk.m_current == 0)
                break;
        }
    }

    LeaveCriticalSection(&walk.m_owner->m_lock);

    g_osTcpSelectStopping = 0;
}

}  // namespace OsNet
