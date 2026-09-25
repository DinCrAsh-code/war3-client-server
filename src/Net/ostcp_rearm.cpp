//============================================================================
//  OsNet::TCPACCEPT::Rearm (0x6F6DBD20) - put a fresh AcceptEx back on the
//  listener.
//
//  Its own translation unit so FinishAccept keeps the call.
//============================================================================
#include "ostcp.h"

namespace OsNet
{

//----------------------------------------------------------------------------
//  0x6F6DBD20.
//
//  AcceptEx needs the accepting socket to exist before the call, so this
//  creates one and hands it in.  m_addrBuf is where AcceptEx writes the two
//  addresses, 0x20 bytes reserved for each, and no receive data is asked for -
//  which matters, because a non-zero receive length would make the accept wait
//  for the peer's first bytes.
//
//  The WSAGetLastError() whose result goes nowhere is the shipped code's.  It
//  is not dead: the call itself clears the thread's pending Winsock error, and
//  removing it would leave the failure visible to whatever asks next.
//----------------------------------------------------------------------------
void TCPACCEPT::Rearm()
{
    EnterCriticalSection(&m_critSec);

    if (m_listener->m_running != 0)
    {
        if (m_socket == INVALID_SOCKET)
        {
            SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
            m_socket = s;
            if (s != INVALID_SOCKET)
            {
                DWORD received;
                if (!g_pfnAcceptEx(m_listener->m_listenSocket, s, m_addrBuf, 0,
                                   0x20, 0x20, &received, &m_op.m_ov))
                {
                    WSAGetLastError();
                    LeaveCriticalSection(&m_critSec);
                    return;
                }
            }
        }
    }
    else
    {
        SOCKET s = m_socket;
        if (s != INVALID_SOCKET)
        {
            closesocket(s);
            m_socket = INVALID_SOCKET;
        }
    }

    LeaveCriticalSection(&m_critSec);
}

}  // namespace OsNet
