//============================================================================
//  OsNet::NETCONN::Close - vtable slot 9, 0x6F6DA450.
//
//  Same address in all three vtables (NETCONN, TCPCONN, IOTCPCONN), so it is
//  NETCONN's own and never overridden - see netconn.h's slot 9 comment for
//  the callers.  Its own translation unit so the tail call into
//  OnPendingDrained() (a different virtual slot, TCPCONN's real override)
//  stays a real indirect call rather than inlining into whatever picks up
//  TCPCONN's own definition.
//============================================================================
#include "netconn.h"

namespace OsNet
{

//----------------------------------------------------------------------------
//  0x6F6DA450 - retn 0.  Take the connection lock, then tail-jump into slot
//  3 (OnPendingDrained) with the lock still held; that override is what
//  actually closes the socket and releases the lock (see
//  tcpconn_onpendingdrained.cpp).
//----------------------------------------------------------------------------
void NETCONN::Close()
{
    EnterCriticalSection(&m_critSec);
    OnPendingDrained();
}

}  // namespace OsNet
