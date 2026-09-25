//============================================================================
//  0x6F6E6CC0 - TCPMGR::StartIocp.  Create the completion port unless
//  bit 0x40000000 forbids it, decide the worker count, and start the pumps.
//
//  Three flag bits are read here and each does something different: the sign
//  bit makes a missing port fatal, 0x40000000 skips creating one at all, and
//  bit 0 means "the caller will pump it itself", which is what sets the
//  worker count to zero and so leaves TCPMGR::Pump doing the work inline.
//============================================================================
#include "ostcp.h"

namespace OsNet
{

int TCPMGR::StartIocp(int flags)
{
    unsigned int workers = 1;

    if (!(flags & 0x40000000))
        m_iocp = OsNetCreateIocp(&workers);

    if (flags < 0 && m_iocp == 0)
        return 0;

    if (m_iocp != 0 && (flags & 1))
        workers = 0;

    m_workerCount = workers;

    if (m_iocp != 0 && !StartPumpThreads())
        return 0;

    return 1;
}

}  // namespace OsNet
