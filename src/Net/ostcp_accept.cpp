//============================================================================
//  OsNet::TCPACCEPT::FinishAccept (0x6F6DBDB0) - collect the socket an
//  AcceptEx just produced.
//
//  Its own translation unit so TCPMGR::OnAcceptComplete keeps the call.
//============================================================================
#include "ostcp.h"

namespace OsNet
{

//: SO_UPDATE_ACCEPT_CONTEXT.  Spelled out rather than pulled from mswsock.h
//: so this file needs nothing beyond winsock2.h - the option is what makes an
//: AcceptEx socket behave like one returned by accept(), by copying the
//: listening socket's properties onto it.
const int kSoUpdateAcceptContext = 0x700B;

//----------------------------------------------------------------------------
//  0x6F6DBDB0 - retn 8.  Returns the accepted socket or INVALID_SOCKET.
//
//  `outAddrs` is 32 bytes: the remote sockaddr_in first, then the local one.
//  That order is the one thing here that is easy to get backwards - the two
//  out-pointers GetAcceptExSockaddrs fills come back local-first, and the
//  copies below take the *remote* one to offset 0.
//
//  Whichever way it exits, the slot is left with no socket and a fresh accept
//  posted: the shipped code clears m_socket and calls Rearm() on both the
//  accepted and the abandoned path.
//----------------------------------------------------------------------------
SOCKET TCPACCEPT::FinishAccept(void* outAddrs, int accepted)
{
    EnterCriticalSection(&m_critSec);

    if (m_listener->m_running == 0)
    {
        closesocket(m_socket);
        m_socket = INVALID_SOCKET;
        LeaveCriticalSection(&m_critSec);
        return INVALID_SOCKET;
    }

    SOCKET s = m_socket;
    if (accepted)
    {
        struct sockaddr* local;
        int locallen;
        struct sockaddr* remote;
        int remotelen;

        g_pfnGetAcceptExSockaddrs(m_addrBuf, 0, 0x20, 0x20,
                                  &local, &locallen, &remote, &remotelen);

        ((struct sockaddr_in*)outAddrs)[0] = *(struct sockaddr_in*)remote;
        ((struct sockaddr_in*)outAddrs)[1] = *(struct sockaddr_in*)local;

        setsockopt(s, SOL_SOCKET, kSoUpdateAcceptContext,
                   (const char*)&m_listener->m_listenSocket, sizeof(SOCKET));
    }
    else
    {
        closesocket(s);
        s = INVALID_SOCKET;
    }

    m_socket = INVALID_SOCKET;
    Rearm();
    LeaveCriticalSection(&m_critSec);
    return s;
}

}  // namespace OsNet
