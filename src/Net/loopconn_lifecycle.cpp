//============================================================================
//  OsNet::LOOPCONN - the same-process loopback pair (see osnetfamily.h).
//  Teardown: IsSocketClosed, OnPendingDrained, DeleteSelf and Close.
//
//  "LOOP" connections come in pairs - m_peer points at the other half - and
//  every lock here is the *owner's* own critical section at m_owner+0x28,
//  not this->m_critSec: a pair's teardown has to be serialised against the
//  owner's whole peer table, not just one half.
//============================================================================
#include "osnetfamily.h"

//  Storm.dll ordinal 403.  Declared here, ahead of `namespace OsNet`,
//  rather than at namespace scope below - a namespace-scope declaration
//  here mangles as `OsNet::SMemFree` (a symbol nothing provides), not the
//  global Storm import `link_check.py` actually resolves.
void __stdcall SMemFree(void* ptr, const char* logfilename, int logline,
                        unsigned int flags);

namespace OsNet
{

typedef void (__thiscall *UnlinkFromOwnerFn)(void* linkField);
typedef void (__thiscall *MaybeNotifyDisconnectFn)(void* owner, void* conn);
typedef void (__thiscall *DestructLoopConnFn)(void* obj);

//----------------------------------------------------------------------------
//  0x6F6DBF90 - slot 1, retn 0.  Under the owner's peer-table lock: "closed"
//  means "unpaired" - a loopback connection never has a real socket to have
//  closed.
//----------------------------------------------------------------------------
BOOL LOOPCONN::IsSocketClosed()
{
    EnterCriticalSection((CRITICAL_SECTION*)((char*)m_owner + 0x28));
    LOOPCONN* peer = m_peer;
    LeaveCriticalSection((CRITICAL_SECTION*)((char*)m_owner + 0x28));
    return peer == 0;
}

//----------------------------------------------------------------------------
//  0x6F6E2400 - slot 3, retn 0.  Called with the owner's peer-table lock
//  already held (see Close() below).  Unlinks the peer (if any) and this
//  half from whatever list sub_6F6E0B60 walks, notifying each side's owner
//  through sub_6F6DEB80 when it had not already been marked (m_state == 0),
//  releases the lock, signals the owner's wake event, then drops both
//  halves' references - the peer's first, this half's last (a tail call).
//----------------------------------------------------------------------------
void LOOPCONN::OnPendingDrained()
{
    CRITICAL_SECTION* lock = (CRITICAL_SECTION*)((char*)m_owner + 0x28);
    LOOPCONN* peer = m_peer;

    if (peer != 0)
    {
        ((UnlinkFromOwnerFn)0x6F6E0B60)(&peer->m_queueLink);
        peer->m_peer = 0;
        if (peer->m_state == 0)
            ((MaybeNotifyDisconnectFn)0x6F6DEB80)(peer->m_owner, peer);

        ((UnlinkFromOwnerFn)0x6F6E0B60)(&m_queueLink);
        m_peer = 0;
        if (m_state == 0)
            ((MaybeNotifyDisconnectFn)0x6F6DEB80)(m_owner, this);

        LeaveCriticalSection(lock);
        SetEvent(*(HANDLE*)((char*)m_owner + 0x24));

        peer->ReleaseRaw();
        this->ReleaseRaw();
        return;
    }

    LeaveCriticalSection(lock);
}

//----------------------------------------------------------------------------
//  0x6F6E23D0 - slot 4, retn 4h.  Calls a real, out-of-line destructor
//  (0x6F6E0130, out of this session's scope, thunked) rather than inlining
//  one - LOOPCONN has its own fields to tear down, the same reason
//  TCPCONN::DeleteSelf is a real call rather than NETCONN::DeleteSelf's
//  inlined one.
//----------------------------------------------------------------------------
LOOPCONN* LOOPCONN::DeleteSelf(int flags)
{
    ((DestructLoopConnFn)0x6F6E0130)(this);
    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);
    return this;
}

//----------------------------------------------------------------------------
//  0x6F6DBF70 - slot 9, retn 0.  NETCONN::Close's own lock-then-dispatch
//  shape (netconn.h), but against the owner's peer-table lock rather than
//  this->m_critSec.
//----------------------------------------------------------------------------
void LOOPCONN::Close()
{
    CRITICAL_SECTION* lock = (CRITICAL_SECTION*)((char*)m_owner + 0x28);
    EnterCriticalSection(lock);
    OnPendingDrained();
}

}  // namespace OsNet
