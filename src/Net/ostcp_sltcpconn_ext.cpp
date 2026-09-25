//============================================================================
//  OsNet::SLTCPCONN's five newly-discovered vtable overrides (see ostcp.h's
//  own class comment) - OnSelectEvent, RegisterForSelect, DeleteSelf and
//  OnQueued.  A previous session's SLTCPCONN comment said "adds no data at
//  all"; that is still true (see ostcp.h), it simply overrode more of
//  TCPCONN's slots than had been dumped yet.
//============================================================================
#include "ostcp.h"

//  Storm.dll ordinal 403.  Declared here, ahead of `namespace OsNet`,
//  rather than block-scoped below - see fileconn_deleteself.cpp's own
//  comment on this for why.
void __stdcall SMemFree(void* ptr, const char* logfilename, int logline,
                        unsigned int flags);

namespace OsNet
{

typedef void (__thiscall *SelectReasonZeroFn)(void* thisObj);
typedef void (__thiscall *SelectReasonOneFn)(void* thisObj);
typedef void (__thiscall *SelectInsertFn)(void* mgr, void* connObj);
typedef void (__thiscall *SelectAddFn)(void* mgr, void* connObj, int reason);

//----------------------------------------------------------------------------
//  0x6F6DD090 - slot 0, retn 8h.  Two stack arguments; only the second
//  (`reason`) is ever read - `unused0` is dead, the same "two stack args,
//  only the second used" shape UDPCONN::OnReadable has.  A two-way dispatch
//  on `reason`: 0 tail-calls sub_6F6DAAA0 (out of this session's scope,
//  thunked - itself a tail jump into vtable slot 9, Close), 2 calls
//  sub_6F6DCFC0 (also thunked); anything else (including 1) is a no-op.
//----------------------------------------------------------------------------
void SLTCPCONN::OnSelectEvent(int unused0, int reason)
{
    (void)unused0;
    if (reason == 0)
        ((SelectReasonZeroFn)0x6F6DAAA0)(this);
    else if (reason == 2)
        ((SelectReasonOneFn)0x6F6DCFC0)(this);
}

//----------------------------------------------------------------------------
//  0x6F6E6E00 - slot 2, retn 4h.  Registers into the read bucket
//  unconditionally, then the write bucket too when the send queue's own
//  terminator prevlink (+0x70 - m_sendQueue's TSList<OUTPUT,0> internals,
//  see storm.h) is positive - a non-empty queue waiting to drain.
//----------------------------------------------------------------------------
void SLTCPCONN::RegisterForSelect(void* mgr)
{
    ((SelectInsertFn)0x6F6E6BC0)(mgr, this);
    ((SelectAddFn)0x6F6D9B60)(mgr, this, 0);

    if (*(int*)((char*)this + 0x70) > 0)
        ((SelectAddFn)0x6F6D9B60)(mgr, this, 1);
}

//----------------------------------------------------------------------------
//  0x6F6E2620 - slot 4, retn 4h.  Calls `this->TCPCONN::~TCPCONN()` as a
//  real, out-of-line call (0x6F6E0520, the same destructor
//  IOTCPCONN::DeleteSelf calls) - SLTCPCONN declares no destructor of its
//  own, but the call still has to stay real rather than inline, the same
//  reason iotcpconn_deleteself.cpp's own comment gives.
//----------------------------------------------------------------------------
SLTCPCONN* SLTCPCONN::DeleteSelf(int flags)
{
    this->TCPCONN::~TCPCONN();
    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);
    return this;
}

//----------------------------------------------------------------------------
//  0x6F6DAA80 - slot 15, retn 4h.  Just release the connection lock -
//  SLTCPCONN has no overlapped send path to arm, so `buf` goes unread.
//----------------------------------------------------------------------------
void SLTCPCONN::OnQueued(OUTPUT* /*buf*/)
{
    LeaveCriticalSection(&m_critSec);
}

}  // namespace OsNet
