//============================================================================
//  OsNet::TCPCONN::OnRecvComplete (0x6F6DA720) - vtable slot 8, and the whole
//  of the receive logic.
//
//  This is the function the live trace pointed at: TCPMGR::PumpIocp dispatches
//  a completion whose IOOP type is 0 through vtable+0x20, which lands here,
//  and the `call edx` at 0x6F6DA77F that re-arms the read is the return
//  address 0x6F6DA781 the stack walk showed.
//
//  Its own translation unit so NotifyData and PostRead stay real calls.
//============================================================================
#include "ostcp.h"

namespace OsNet
{

//  Overlapping source and destination is the whole point of the compaction
//  below, so this is memmove and not memcpy.  Declared by hand for the same
//  reason as everywhere else in this module (/Zl keeps <string.h> out), and
//  *not* as an intrinsic: the dump calls the import rather than expanding it.
extern "C" void* __cdecl memmove(void* dst, const void* src,
                                 unsigned int count);

//----------------------------------------------------------------------------
//  0x6F6DA720 - retn 0Ch.
//
//  The receive buffer is a plain sliding window: bytes land at
//  m_recvBuf + m_recvUsed, the owner's callback is offered the whole window
//  and reports back how much of the front it framed off, and whatever is left
//  is shifted down to the base before the next read is armed.
//
//  Three exits are worth naming because two of them close the connection:
//
//  * No callback (NotifyData returned 0) - nothing can ever drain the window,
//    so Close().
//  * The callback consumed *nothing* and the window is full - framing has
//    stalled against a message bigger than 0x5B4, which this buffer can never
//    hold, so Close(). This is the guard that stops a full buffer spinning
//    forever on a re-armed read that can have no room.
//  * The callback consumed nothing but there is still room - normal, just arm
//    another read and let more arrive.
//
//  Note the asymmetry the shipped code has when everything was consumed: it
//  zeroes m_recvUsed and skips the memmove entirely, rather than moving zero
//  bytes. The `op` argument is never read - the buffer is the connection's,
//  not the operation's.
//----------------------------------------------------------------------------
void TCPCONN::OnRecvComplete(IOOP* /*op*/, DWORD bytes, BOOL ok)
{
    if (bytes == 0 || !ok)
        return;

    m_recvUsed += bytes;

    unsigned int consumed = 0;
    if (NotifyData(m_recvBuf, m_recvUsed, &consumed, 0) == 0)
    {
        Close();
        return;
    }

    if (consumed >= m_recvUsed)
    {
        m_recvUsed = 0;
        PostRead();
        return;
    }

    if (consumed != 0)
    {
        memmove(m_recvBuf, m_recvBuf + consumed, m_recvUsed - consumed);
        m_recvUsed -= consumed;
        PostRead();
        return;
    }

    if (m_recvUsed >= kTcpSegment)
    {
        Close();
        return;
    }

    PostRead();
}

}  // namespace OsNet
