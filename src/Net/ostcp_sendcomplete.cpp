//============================================================================
//  OsNet::TCPCONN::OnSendComplete (0x6F6E05C0) - vtable slot 7, the receive
//  path's twin.
//
//  Its own translation unit so the OnQueued dispatch stays a real call.
//============================================================================
#include "ostcp.h"

namespace OsNet
{

static const char kW32OsTcpCpp[]   = ".\\W32\\OsTcp.cpp";
static const char kOutputTypeName[] = ".?AUOUTPUT@OsNet@@";

//----------------------------------------------------------------------------
//  0x6F6E05C0 - retn 0Ch.
//
//  Mark the operation done, then walk the tail of the send queue releasing
//  every buffer that has reached state 2 - payload first with the module's
//  own filename tag, then the buffer itself with the element's RTTI
//  descriptor, exactly as OUTPUTLIST::FreeAll does - and stop at the first
//  one that has not.  If a buffer is still outstanding, hand it to OnQueued
//  to restart the drain.
//
//  **The lock is handed to OnQueued, not released here.**  That branch returns
//  with no LeaveCriticalSection in front of it while the empty-queue exit
//  below has one, and TCPCONN::Send has the same asymmetry on its own call to
//  the same slot.  Two independent functions doing it makes it the
//  convention, not an oversight: whoever starts the drain owns the connection
//  lock until the drain is set up.
//
//  A failed operation (`!ok`) returns before the lock is even taken - there is
//  nothing to release and nothing to restart.  `bytes` is never read; how much
//  went out does not matter here, only that the operation finished.
//----------------------------------------------------------------------------
void TCPCONN::OnSendComplete(IOOP* op, DWORD /*bytes*/, BOOL ok)
{
    if (!ok)
        return;

    EnterCriticalSection(&m_critSec);

    //  Four bytes past the IOOP - see SENDOP in ostcp.h for why that word
    //  belongs to the container and not to IOOP itself.
    ((SENDOP*)op)->m_state = 2;

    for (;;)
    {
        int tail = m_sendQueue.TailLink();
        if (tail <= 0)
            break;

        OUTPUT* buf = (OUTPUT*)tail;
        if (buf->m_sendState != 2)
        {
            //  Still in flight - restart the drain from here, and let
            //  OnQueued take the lock with it.
            OnQueued(buf);
            return;
        }

        if (buf->m_capacity != 0 && buf->m_data != 0)
            SMemFree(buf->m_data, kW32OsTcpCpp, 0x190, 0);

        buf->m_link.Unlink();
        SMemFree(buf, kOutputTypeName, -2, 0);
    }

    LeaveCriticalSection(&m_critSec);
}

}  // namespace OsNet
