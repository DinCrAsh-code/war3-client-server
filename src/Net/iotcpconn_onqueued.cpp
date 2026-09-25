//============================================================================
//  OsNet::IOTCPCONN::OnQueued (0x6F6DA8D0) - vtable slot 15, TCPCONN's
//  `_purecall` override for "start draining the queue".
//
//  Its own translation unit so Send()/OnSendComplete()'s virtual call into
//  it stays real - the same reasoning ostcp_postread.cpp gives for PostRead.
//============================================================================
#include "ostcp.h"

namespace OsNet
{

//----------------------------------------------------------------------------
//  0x6F6DA8D0.
//
//  Caller (Send()/OnSendComplete(), ostcp_send.cpp/ostcp_sendcomplete.cpp)
//  hands this the buffer under the connection lock and does not release it
//  first - see ostcp.h's own slot 15 comment.  This is what actually drops
//  it: mark the buffer in flight, bump the pending-I/O count, release the
//  lock, then try to post the send.
//
//  Two ways to post the same send, mirroring PostRead's receive-side split:
//  WSASend when the extension pointer resolved (g_pfnWSASend), an
//  overlapped WriteFile on the socket handle when it did not.  Both post
//  into `&buf->m_reserved08` as the OVERLAPPED - `OUTPUT`'s own five
//  "unread" reserved dwords turn out to be exactly `sizeof(OVERLAPPED)`
//  bytes, scratch space for precisely this call (see ostcp.h's OUTPUT
//  comment).
//
//  A synchronous failure that is not "the send is pending" closes the
//  connection and drops the pending reference this function just took -
//  the same Close()+ReleasePending() tail PostRead's own failure path has.
//----------------------------------------------------------------------------
void IOTCPCONN::OnQueued(OUTPUT* buf)
{
    buf->m_sendState = 1;
    AddPending();
    LeaveCriticalSection(&m_critSec);

    //  Two ways to post the same send, mirroring PostRead's receive-side
    //  split: WSASend when the extension pointer resolved, an overlapped
    //  WriteFile on the socket handle when it did not.  The dump's own jump
    //  targets show the two success epilogues are not laid out
    //  symmetrically - WSASend's own success jumps forward to share the
    //  same epilogue WriteFile's success/pending checks land on, while
    //  WSASend's own WSA_IO_PENDING check is reached as a fall-through and
    //  gets its own separate, physically duplicated epilogue instead. That
    //  is basic-block layout, not a semantic difference - every code path
    //  here reaches the identical `return`, and this build's own tail
    //  merging collapses it to one physical epilogue regardless of how the
    //  checks are ordered or grouped in source (a `goto`-based rewrite that
    //  tried to force the split was measured and changed nothing - see
    //  funcmap.py's BEHAVIOUR entry).
    if (g_pfnWSASend != 0)
    {
        WSABUF wsabuf;
        DWORD sent = 0;

        wsabuf.len = buf->m_used;
        wsabuf.buf = (char*)buf->m_data;

        if (g_pfnWSASend(m_socket, &wsabuf, 1, &sent, 0,
                         (LPWSAOVERLAPPED)&buf->m_reserved08, 0) != SOCKET_ERROR)
            return;
        if (WSAGetLastError() == WSA_IO_PENDING)
            return;
    }
    else
    {
        if (WriteFile((HANDLE)m_socket, buf->m_data, buf->m_used, 0,
                      (LPOVERLAPPED)&buf->m_reserved08))
            return;
        if (GetLastError() == ERROR_IO_PENDING)
            return;
    }

    Close();
    ReleasePending();
}

}  // namespace OsNet
