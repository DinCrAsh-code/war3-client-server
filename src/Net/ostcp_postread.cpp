//============================================================================
//  OsNet::IOTCPCONN::PostRead (0x6F6DA990) - arm the next receive.
//
//  Its own translation unit so IOTCPCONN's constructor keeps the call.
//============================================================================
#include "ostcp.h"

namespace OsNet
{

//  The CRT intrinsic, recognised by name/signature under /O2 without
//  <string.h> (which /Zl keeps out of this build).  At this size it expands
//  to six inline dword stores, which is what the dump has.
extern "C" void* __cdecl memset(void* dst, int value, unsigned int count);
#pragma intrinsic(memset)

//----------------------------------------------------------------------------
//  0x6F6DA990.
//
//  Two ways to arm the same receive on the same handle: WSARecv when the
//  extension pointer was resolved, and an overlapped ReadFile on the socket
//  handle when it was not.  Both post into the same IOOP, whose m_type is
//  left at 0 by the wipe above them - which is what makes the completion come
//  back through OnRecvComplete rather than OnSendComplete.
//
//  The failure tail is a tail call: ReleasePending() (vtable slot 6) is the
//  last thing the function does and the dump reaches it with a `jmp`, not a
//  `call` - it could not arm the read, so it drops the outstanding-I/O
//  reference it was going to consume.
//----------------------------------------------------------------------------
void IOTCPCONN::PostRead()
{
    AddPending();

    IOOP* op = &m_readOp;
    memset(op, 0, sizeof(*op));

    if (g_pfnWSARecv != 0)
    {
        WSABUF wsabuf;
        DWORD received = 0;
        DWORD flags = 0;

        wsabuf.len = kTcpSegment - m_recvUsed;
        wsabuf.buf = m_recvBuf + m_recvUsed;

        if (g_pfnWSARecv(m_socket, &wsabuf, 1, &received, &flags,
                         (LPWSAOVERLAPPED)op, 0) != SOCKET_ERROR)
            return;
        if (WSAGetLastError() == WSA_IO_PENDING)
            return;
    }
    else
    {
        if (ReadFile((HANDLE)m_socket, m_recvBuf + m_recvUsed,
                     kTcpSegment - m_recvUsed, 0, (LPOVERLAPPED)op))
            return;
        if (GetLastError() == ERROR_IO_PENDING)
            return;
    }

    Close();
    ReleasePending();
}

}  // namespace OsNet
