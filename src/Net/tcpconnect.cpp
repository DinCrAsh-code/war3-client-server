//============================================================================
//  OsNet::TCPCONNECT - the small, standalone object an outbound TCP connect
//  attempt lives in while it is in flight (see osnetfamily.h for the class
//  and its recovered layout).
//
//  Every method here calls at least one helper outside this session's
//  50-address worklist - the select-bucket registration pair
//  (0x6F6E6BC0/0x6F6D9B60, shared with TCPLISTEN/UDPCONN/SLTCPCONN's own
//  slot 2 overrides), the reason-3 announce helper (0x6F6D9BC0) and the
//  shared connect finisher (0x6F6E3880, shared with LOOPCONNECT's and
//  FILECONNECT's own slot 4).  Each is declared as a plain function-pointer
//  cast to its real address - the same "typedef a raw thiscall signature,
//  call through a fixed address" idiom CLAUDE.md's own ApplyOffsetFn example
//  uses for a hand-written vtable slot dispatch - rather than a
//  __declspec(naked) thunk, since none of these are *this class's own*
//  vtable slots or reconstructed functions; they are plain internal calls
//  this class's own reconstructed bodies make.
//============================================================================
#include "osnetfamily.h"

namespace OsNet
{

typedef void (__thiscall *AnnounceReason3Fn)(void* thisObj,
                                             CONNNOTIFYPROC proc,
                                             const void* addrs);
typedef void (__thiscall *SelectInsertFn)(void* mgr, void* connObj);
typedef void (__thiscall *SelectAddFn)(void* mgr, void* connObj, int reason);
typedef void (__thiscall *ConnectFinishTcpFn)(void* obj, int arg);

//----------------------------------------------------------------------------
//  0x6F6D9CF0 - slot 0, retn 8h.  `reason` 1 tail-calls slot 4
//  (OnConnectResult) with `arg`; `reason` 2 tail-calls slot 3 (Abort) with
//  no argument.  Any other reason is a no-op.
//----------------------------------------------------------------------------
void TCPCONNECT::OnConnectEvent(int reason, void* arg)
{
    if (reason == 1)
        OnConnectResult((int)arg);
    else if (reason == 2)
        Abort();
}

//----------------------------------------------------------------------------
//  0x6F6E6C10 - slot 2, retn 4h.  Insert into the read bucket, then the
//  write bucket - a connect attempt watches for both writable (connect
//  completed) and readable (an early close/reset).
//----------------------------------------------------------------------------
void TCPCONNECT::RegisterForSelect(void* mgr)
{
    ((SelectInsertFn)0x6F6E6BC0)(mgr, this);
    ((SelectAddFn)0x6F6D9B60)(mgr, this, 1);
    ((SelectAddFn)0x6F6D9B60)(mgr, this, 2);
}

//----------------------------------------------------------------------------
//  0x6F6D9C60 - slot 3, retn 0.  Build a local sockaddr_in from m_addr/
//  m_port, announce reason 3 through m_notifyProc, then close the socket if
//  it is still open.
//----------------------------------------------------------------------------
void TCPCONNECT::Abort()
{
    struct sockaddr_in local;
    ZeroMemory(&local, sizeof(local));
    local.sin_family = AF_INET;
    local.sin_port   = htons(m_port);
    local.sin_addr.s_addr = m_addr;

    ((AnnounceReason3Fn)0x6F6D9BC0)(this, m_notifyProc, &local);

    if (m_socket != INVALID_SOCKET)
    {
        closesocket(m_socket);
        m_socket = INVALID_SOCKET;
    }
}

//----------------------------------------------------------------------------
//  0x6F6E3EC0 - slot 4, retn 4h.  Tail into the shared connect finisher,
//  reinterpreting `this` as its own first argument and passing the real
//  `this` through as a stack argument - the shape the finisher itself
//  expects (out of this session's scope).
//----------------------------------------------------------------------------
void TCPCONNECT::OnConnectResult(int arg)
{
    ((ConnectFinishTcpFn)0x6F6E3880)((void*)arg, (int)this);
}

}  // namespace OsNet
