//============================================================================
//  0x6F6E14A0 - TCPLISTEN::CloseSocket.  Drop the socket from the module's
//  select set, close it and mark the slot free again.
//============================================================================
#include "ostcp.h"

namespace OsNet
{

void TCPLISTEN::CloseSocket()
{
    if (m_socket == INVALID_SOCKET)
        return;

    //  0x6F6E1420 - SelectSetUpdate(0), now reconstructed for real in
    //  ostcp_thunks.cpp.  A prior session's free-function
    //  `OsTcpSelectSetUpdate(SOCKET, int)` guess (calling with `this->
    //  m_socket` in ecx) does not match the real callee - its own dump
    //  (0x6F6E14A0) shows `ecx` carried over unchanged from `this`, not
    //  reloaded from +0x04, and the callee's own body indexes [ecx+0x1C]/
    //  [ecx+0x24] as a TCPLISTEN, not a bare socket handle.
    SelectSetUpdate(0);
    closesocket(m_socket);
    m_socket = INVALID_SOCKET;
}

}  // namespace OsNet
