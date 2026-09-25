//============================================================================
//  OsNet::UDPCONN - a connectionless (SOCK_DGRAM) socket wrapper,
//  `: public NETCONN`.  See osnetfamily.h for the class.
//============================================================================
#include "osnetfamily.h"

void __stdcall SErrSetLastError(unsigned int code);
//  Storm.dll ordinal 403.  Declared here, ahead of `namespace OsNet`,
//  rather than block-scoped below - see fileconn_deleteself.cpp's own
//  comment on this for why.
void __stdcall SMemFree(void* ptr, const char* logfilename, int logline,
                        unsigned int flags);

namespace OsNet
{

typedef void (__thiscall *SelectInsertFn)(void* mgr, void* connObj);
typedef void (__thiscall *SelectAddFn)(void* mgr, void* connObj, int reason);
typedef void (__thiscall *FillDefaultAddrFn)(void* thisConn, void* outAddr);
typedef void (__thiscall *DestructUdpConnFn)(void* obj);

//----------------------------------------------------------------------------
//  0x6F6DA680 - slot 0, retn 8h.  `reason != 0` is a no-op.  `reason == 0`:
//  recvfrom() into a 0x5B4-byte local buffer, then NotifyData with the
//  sender's own address as the per-packet override.  Compiled with a /GS
//  stack cookie this build's fixed /GS- can never reproduce - see
//  docs/msvc-vc8-idioms.md, so an exact match is not attainable here
//  regardless of source shape.
//----------------------------------------------------------------------------
void UDPCONN::OnReadable(int /*unused0*/, int reason)
{
    if (reason != 0)
        return;

    struct sockaddr_in from;
    ((FillDefaultAddrFn)0x6F6DA410)(this, &from);

    //  0x5B4 - the same TCP-segment-sized buffer TCPCONN::m_recvBuf uses
    //  (ostcp.h's own kTcpSegment), reused here as a plain local rather than
    //  pulling in ostcp.h for one constant.
    char buf[0x5B4];
    int fromlen = sizeof(from);
    int bytes = recvfrom(m_socket, buf, sizeof(buf), 0,
                         (struct sockaddr*)&from, &fromlen);
    if (bytes <= 0)
        return;

    unsigned int consumed = 0;
    NotifyData(buf, (unsigned int)bytes, &consumed, &from);
}

//----------------------------------------------------------------------------
//  0x6F6E6DE0 - slot 2, retn 4h.  Registers into the read bucket only, same
//  shape as TCPLISTEN::RegisterForSelect.
//----------------------------------------------------------------------------
void UDPCONN::RegisterForSelect(void* mgr)
{
    ((SelectInsertFn)0x6F6E6BC0)(mgr, this);
    ((SelectAddFn)0x6F6D9B60)(mgr, this, 0);
}

//----------------------------------------------------------------------------
//  0x6F6DA650 - slot 3, retn 0.  Close the socket if still open, then
//  release the connection lock - UDPCONN's own OnPendingDrained override
//  (TCPCONN::OnPendingDrained's own position and shape, minus the notify -
//  a datagram socket never has a peer to tell).
//----------------------------------------------------------------------------
void UDPCONN::OnPendingDrained()
{
    if (m_socket != INVALID_SOCKET)
    {
        closesocket(m_socket);
        m_socket = INVALID_SOCKET;
    }
    LeaveCriticalSection(&m_critSec);
}

//----------------------------------------------------------------------------
//  0x6F6E2490 - slot 4, retn 4h.  Calls a real, out-of-line destructor
//  (0x6F6E0440, out of this session's scope, thunked).
//----------------------------------------------------------------------------
UDPCONN* UDPCONN::DeleteSelf(int flags)
{
    ((DestructUdpConnFn)0x6F6E0440)(this);
    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);
    return this;
}

//----------------------------------------------------------------------------
//  0x6F6DA5F0 - slot 10, retn 10h.  See the slot 10 comment in
//  osnetfamily.h for the parameter shapes.
//----------------------------------------------------------------------------
void UDPCONN::Send(const void* buf, int len, const void* destAddrs, int count)
{
    if (destAddrs == 0)
    {
        SErrSetLastError(0x57);
        return;
    }

    if (count == 0)
        return;

    const char* entry = (const char*)destAddrs + count * 0x10;
    int remaining = count;
    do
    {
        entry -= 0x10;
        sendto(m_socket, (const char*)buf, len, 0,
              (const struct sockaddr*)entry, 0x10);
        remaining -= 1;
    } while (remaining != 0);
}

}  // namespace OsNet
