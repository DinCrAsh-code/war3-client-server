//============================================================================
//  OsNet::LOOPCONN - the outbound side: CanAppendToHead and Send.  See
//  osnetfamily.h and loopconn_lifecycle.cpp's own header comment for the
//  loopback-pair shape and the owner's-own-lock convention.
//
//  A write on one half of a pair lands in the *peer's* own queue - there is
//  no socket to carry the bytes, so "sending" is just handing the buffer
//  straight to whichever LOOPCONN is on the other end.
//============================================================================
#include "osnetfamily.h"

namespace OsNet
{

typedef void* (__thiscall *PeerQueueFn)(void* peer, const void* src,
                                        unsigned int size);

//----------------------------------------------------------------------------
//  0x6F6DCF30 - slot 10, retn 0.  Under the owner's peer-table lock: true
//  when a peer exists and its own queue head is still open for appending
//  (peer->m_queueHeadPrevlink > 0 - the peer-side equivalent of
//  TCPCONN::CanAppendToHead's own `head->m_sendState == 0` test, over a
//  differently-shaped queue element).
//----------------------------------------------------------------------------
BOOL LOOPCONN::CanAppendToHead()
{
    BOOL result = FALSE;
    CRITICAL_SECTION* lock = (CRITICAL_SECTION*)((char*)m_owner + 0x28);
    EnterCriticalSection(lock);
    if (m_peer != 0)
        result = (m_peer->m_queueHeadPrevlink > 0);
    LeaveCriticalSection(lock);
    return result;
}

//----------------------------------------------------------------------------
//  0x6F6E0340 - slot 11, retn 8h.  `len == 0` is a no-op.  Otherwise, under
//  the owner's lock, forward the write into the peer's own queue
//  (0x6F6E0270, out of this session's scope - a LOOPCONN-flavoured
//  Queue(src, size) matching TCPCONN::Queue's own signature) when a peer is
//  present, then signal the owner's wake event once the lock is released -
//  the peer picks the data up off its own thread, this call never blocks
//  on it.
//----------------------------------------------------------------------------
void LOOPCONN::Send(const void* buf, int len)
{
    if (len == 0)
        return;

    CRITICAL_SECTION* lock = (CRITICAL_SECTION*)((char*)m_owner + 0x28);
    EnterCriticalSection(lock);

    LOOPCONN* peer = m_peer;
    if (peer != 0)
        ((PeerQueueFn)0x6F6E0270)(peer, buf, (unsigned int)len);

    LeaveCriticalSection(lock);

    if (peer != 0)
        SetEvent(*(HANDLE*)((char*)m_owner + 0x24));
}

}  // namespace OsNet
