//============================================================================
//  OsNet::TCPCONN::OnPendingDrained - vtable slot 3, 0x6F6DA7D0.
//
//  `_purecall` in NETCONN; this is TCPCONN's own override, inherited
//  unchanged by IOTCPCONN (same address in both dumped vtables).  Its own
//  translation unit, matching every other slot override in this pass.
//============================================================================
#include "ostcp.h"

namespace OsNet
{

//----------------------------------------------------------------------------
//  0x6F6DA7D0 - retn 0.  Close the socket if it is still open, then release
//  the connection lock.  Reached two ways: directly, from ReleasePending()
//  on the transition to zero outstanding I/O (immediately before
//  Shutdown(1)); and as the tail of NETCONN::Close() (slot 9), which takes
//  the same lock and jumps straight here instead of calling it.
//----------------------------------------------------------------------------
void TCPCONN::OnPendingDrained()
{
    if (m_socket != INVALID_SOCKET)
    {
        closesocket(m_socket);
        m_socket = INVALID_SOCKET;
    }
    LeaveCriticalSection(&m_critSec);
}

}  // namespace OsNet
