//============================================================================
//  OsNet::IOTCPCONN::IOTCPCONN (0x6F6E24F0) - the completion-port connection.
//============================================================================
#include "ostcp.h"

namespace OsNet
{

//----------------------------------------------------------------------------
//  0x6F6E24F0 - retn 20h, returns `this`.
//
//  Order matters here and the dump pins it down.  m_pending is set to 1 and
//  the socket is registered with the port *before* anything else, because from
//  the moment CreateIoCompletionPort succeeds a completion can arrive on
//  another thread; the ReleasePending() at the bottom drops that initial
//  reference, so a connection nobody wanted tears itself down before this
//  constructor returns.
//
//  The failed-registration path passes Shutdown(0), not Shutdown(1) - the
//  owner has not been told about this connection yet, so it must not be told
//  it went away.  The zero it passes is the failed handle itself, which the
//  compiler reuses as the constant rather than materialising one.
//----------------------------------------------------------------------------
IOTCPCONN::IOTCPCONN(TCPMGR* owner, HANDLE iocp, SOCKET s,
                     CONNNOTIFYPROC proc, void* ctx, const void* addrs,
                     const void* buf, int len)
    : TCPCONN(owner, s, proc, ctx, addrs)
{
    m_pending = 1;

    if (CreateIoCompletionPort((HANDLE)m_socket, iocp, (ULONG_PTR)this, 0) == 0)
    {
        Shutdown(0);
        return;
    }

    if (buf != 0 && len != 0)
        Send(buf, len);

    if (NotifyConnected())
    {
        m_owner->MoveConn(this, 1);
        PostRead();
    }

    ReleasePending();
}

}  // namespace OsNet
