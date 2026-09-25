//============================================================================
//  OsNet::TCPLISTENSELECT - the listening side's own select-driven completion
//  object (distinct from ostcp.h's TCPLISTENER, the plain-data configuration
//  TCPACCEPT reads).  See osnetfamily.h for the class and its layout.
//============================================================================
#include "osnetfamily.h"

namespace OsNet
{

typedef void (__thiscall *SelectInsertFn)(void* mgr, void* connObj);
typedef void (__thiscall *SelectAddFn)(void* mgr, void* connObj, int reason);
typedef void (__thiscall *AcceptFinishFn)(void* owner, void* listener,
                                          int sock, const void* optval);

//----------------------------------------------------------------------------
//  0x6F6E3DD0 - slot 0, retn 8h.  `reason` 0 means "there is a pending
//  accept": accept() it; if this listener is no longer live
//  (m_stillListening cleared), the newly accepted socket is simply closed,
//  otherwise getsockname() records the peer address and the shared
//  AcceptFinisher tail (0x6F6E3830, out of scope) hands the new socket to
//  `owner`.  Any other reason is a no-op.
//----------------------------------------------------------------------------
void TCPLISTENSELECT::OnSelectEvent(int reason, void* owner)
{
    if (reason != 0)
        return;

    struct sockaddr_in addr;
    struct sockaddr_in name;
    int addrlen = sizeof(addr);
    int namelen = sizeof(name);
    ZeroMemory(&addr, sizeof(addr));
    ZeroMemory(&name, sizeof(name));

    int sock = accept(m_listenSocket, (struct sockaddr*)&addr, &addrlen);
    if (sock == INVALID_SOCKET)
        return;

    if (!m_stillListening)
    {
        closesocket(sock);
        return;
    }

    getsockname(sock, (struct sockaddr*)&name, &namelen);

    ((AcceptFinishFn)0x6F6E3830)(owner, this, sock, &addr);
}

//----------------------------------------------------------------------------
//  0x6F6E6BF0 - slot 2, retn 4h.  Registers into the read bucket only - a
//  listening socket is never "writable".
//----------------------------------------------------------------------------
void TCPLISTENSELECT::RegisterForSelect(void* mgr)
{
    ((SelectInsertFn)0x6F6E6BC0)(mgr, this);
    ((SelectAddFn)0x6F6D9B60)(mgr, this, 0);
}

}  // namespace OsNet
