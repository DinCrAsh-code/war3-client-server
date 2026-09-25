//============================================================================
//  OsNet::TCPCONN::CanAppendToHead - vtable slot 10, 0x6F6DCF80.
//
//  TCPCONN's own, inherited unchanged by IOTCPCONN.  Called by nothing in
//  either dump; the name and signature are inferred from the body.
//============================================================================
#include "ostcp.h"

namespace OsNet
{

//----------------------------------------------------------------------------
//  0x6F6DCF80 - retn 0.  Under the connection lock: the socket has to still
//  be open, the send queue non-empty, and the head buffer's own send state
//  still 0 ("still open for appending" - see ostcp.h's OUTPUT::m_sendState
//  comment).  `m_sendQueue.Head()` is computed inline rather than through a
//  call - three plain memory reads off the list's own terminator, the same
//  shape `TSList<T,0>::Head()`'s definition (storm.h) compiles to at /O2.
//----------------------------------------------------------------------------
BOOL TCPCONN::CanAppendToHead()
{
    BOOL result = FALSE;
    EnterCriticalSection(&m_critSec);
    if (m_socket != INVALID_SOCKET)
    {
        int headLink = m_sendQueue.HeadLink();
        if (headLink > 0)
            result = (((OUTPUT*)headLink)->m_sendState == 0);
    }
    LeaveCriticalSection(&m_critSec);
    return result;
}

}  // namespace OsNet
