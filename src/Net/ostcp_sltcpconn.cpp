//============================================================================
//  OsNet::SLTCPCONN::SLTCPCONN (0x6F6E06B0) - the select-driven connection.
//============================================================================
#include "ostcp.h"

namespace OsNet
{

//----------------------------------------------------------------------------
//  0x6F6E06B0 - retn 1Ch, returns `this`.
//
//  The whole of the derived class is this constructor plus a vftable: it sends
//  whatever initial payload it was given, tells the owner the connection
//  exists, and on the strength of the owner's answer either lists the
//  connection as live (state 1) or tears it straight back down.
//
//  Shutdown(1) rather than Shutdown(0) on the reject path: the owner *was*
//  told about this connection, it just declined it, so it gets the matching
//  disconnect too.
//----------------------------------------------------------------------------
SLTCPCONN::SLTCPCONN(TCPMGR* owner, SOCKET s, CONNNOTIFYPROC proc, void* ctx,
                     const void* addrs, const void* buf, int len)
    : TCPCONN(owner, s, proc, ctx, addrs)
{
    if (buf != 0 && len != 0)
        Send(buf, len);

    //  Tested negatively so the tear-down is the fall-through, which is the
    //  way round the dump branches (`jne` past Shutdown, not `je` past
    //  MoveConn).
    if (!NotifyConnected())
        Shutdown(1);
    else
        m_owner->MoveConn(this, 1);
}

}  // namespace OsNet
