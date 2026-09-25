//============================================================================
//  OsNet::TCPCONN::TCPCONN (0x6F6E04A0) - the base of both connection kinds.
//
//  Its own translation unit so both derived constructors keep the real call
//  to it that the shipped code has.
//============================================================================
#include "ostcp.h"

namespace OsNet
{

//----------------------------------------------------------------------------
//  0x6F6E04A0 - retn 14h, returns `this` in eax.
//
//  Nothing here is the connection's own state beyond the queue: the five
//  arguments go straight to NETCONN, and the body is the two socket options
//  every connection in this module is born with - keepalive on, and
//  non-blocking, which is what makes Send()'s WSAEWOULDBLOCK path the normal
//  case rather than an error path.
//----------------------------------------------------------------------------
TCPCONN::TCPCONN(TCPMGR* owner, SOCKET s, CONNNOTIFYPROC proc, void* ctx,
                 const void* addrs)
    : NETCONN(owner, s, proc, ctx, addrs)
{
    m_recvUsed = 0;

    BOOL keepalive = TRUE;
    setsockopt(m_socket, SOL_SOCKET, SO_KEEPALIVE,
               (const char*)&keepalive, sizeof(keepalive));

    u_long nonblocking = 1;
    ioctlsocket(m_socket, FIONBIO, &nonblocking);
}

}  // namespace OsNet
