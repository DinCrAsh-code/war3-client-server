//============================================================================
//  0x6F6E4190 - TCPMGR::StopIocp.  Raise the module's "IOCP shutting down"
//  flag, wake the port, wait for and close every pump thread, close the port
//  itself and release the listen slots.
//============================================================================
#include "ostcp.h"

namespace OsNet
{


void TCPMGR::StopIocp()
{
    g_osTcpShutdown = 1;

    WakeIocp();

    if (m_iocp != 0)
    {
        JoinPumpThreads();
        CloseHandle(m_iocp);
        m_iocp = 0;
    }

    m_listens.ReleaseAll();
    m_workerCount = 0;

    g_osTcpShutdown = 0;
}

}  // namespace OsNet
