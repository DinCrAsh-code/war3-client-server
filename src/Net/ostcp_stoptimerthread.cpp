//============================================================================
//  0x6F6E1500 - TCPMGR::StopTimerThread.  Wake and join the timer thread,
//  then drain what it owned: the first CONNSTATE shard's connections and the
//  loopback input list.
//============================================================================
#include "ostcp.h"

namespace OsNet
{

void TCPMGR::StopTimerThread()
{
    g_osTcpTimerStopping = 1;

    SetEvent(m_timerEvent);
    WaitForSingleObject(m_timerThread, INFINITE);
    CloseHandle(m_timerThread);
    m_timerThread = 0;

    m_states[0].FreeAll();
    FreeInputList();

    CloseHandle(m_timerEvent);
    m_timerEvent = 0;

    g_osTcpTimerStopping = 0;
}

}  // namespace OsNet
