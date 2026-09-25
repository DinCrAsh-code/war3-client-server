//============================================================================
//  0x6F6DA090 - TCPMGR::StopListenThread.  Wake the thread, wait for it,
//  then close all three handles.
//============================================================================
#include "ostcp.h"

namespace OsNet
{

void TCPMGR::StopListenThread()
{
    g_osTcpListenStopping = 1;

    SetEvent(m_listenWakeEvent);
    WaitForSingleObject(m_listenThread, INFINITE);

    CloseHandle(m_listenThread);
    m_listenThread = 0;
    CloseHandle(m_listenDoneEvent);
    m_listenDoneEvent = 0;
    CloseHandle(m_listenWakeEvent);
    m_listenWakeEvent = 0;

    g_osTcpListenStopping = 0;
}

}  // namespace OsNet
