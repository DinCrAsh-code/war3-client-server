//============================================================================
//  0x6F6DBF50 - TCPMGR::WakeIocp.  Post a null completion packet so a pump
//  blocked in GetQueuedCompletionStatus comes back and re-reads the shutdown
//  flag.  Only when there is a port *and* a worker count - a manager that
//  never started has neither.
//============================================================================
#include "ostcp.h"

namespace OsNet
{


void TCPMGR::WakeIocp()
{
    HANDLE port = m_iocp;

    if (port == 0)
        return;
    if (m_pumpThreads.m_count == 0)
        return;

    PostQueuedCompletionStatus(port, 0, 0, 0);
}

}  // namespace OsNet
