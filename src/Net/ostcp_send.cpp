//============================================================================
//  OsNet::TCPCONN::Send (0x6F6DF040) - hand data to the socket, queueing
//  whatever the socket would not take.
//
//  Its own translation unit so both derived constructors keep their call.
//============================================================================
#include "ostcp.h"

namespace OsNet
{

//----------------------------------------------------------------------------
//  0x6F6DF040 - retn 8.
//
//  `queued` is a genuine `bool`, not an int: the shipped code materialises it
//  with `setnz` off the masked tail pointer and then tests the byte, which is
//  what a bool looks like and not what `if (m_sendQueue.Tail())` compiles to
//  (see docs/msvc-vc8-idioms.md, "`bool` versus `int`").  It is also read
//  twice - once to decide whether to try the socket at all, once after the
//  queue call to decide whether this buffer is the one that has to start the
//  drain - so it cannot be folded into the branch.
//
//  Note the asymmetry the shipped code has and this keeps: a *partial* send
//  advances the cursor and queues the rest, a WSAEWOULDBLOCK queues all of it,
//  and any other socket error closes the connection without queueing
//  anything.
//----------------------------------------------------------------------------
void TCPCONN::Send(const void* buf, int len)
{
    EnterCriticalSection(&m_critSec);

    SOCKET s = m_socket;
    if (s == INVALID_SOCKET)
    {
        LeaveCriticalSection(&m_critSec);
        return;
    }

    //  Named rather than tested in place: the dump materialises the masked
    //  tail pointer with the full setle/sub/and idiom and only then turns it
    //  into the flag, where `m_sendQueue.Tail() != 0` on its own folds the
    //  mask and the test into one `test`.
    OUTPUT* tail = m_sendQueue.Tail();
    bool queued = tail != 0;

    if (!queued)
    {
        int sent = send(s, (const char*)buf, len, 0);
        if (sent != SOCKET_ERROR)
        {
            if (sent >= len)
            {
                LeaveCriticalSection(&m_critSec);
                return;
            }
            buf = (const char*)buf + sent;
            len -= sent;
        }
        else if (WSAGetLastError() != WSAEWOULDBLOCK)
        {
            Close();
            LeaveCriticalSection(&m_critSec);
            return;
        }
    }

    OUTPUT* fresh = Queue(buf, len);
    if (!queued && fresh != 0)
    {
        //  The lock is *handed to* OnQueued, not released here.  The dump
        //  returns straight out of this branch with no LeaveCriticalSection in
        //  front of it, while every other exit goes through the shared one
        //  below - and OnSendComplete (0x6F6E05C0) has exactly the same
        //  asymmetry on its own call to this slot, which is what makes it a
        //  convention rather than an oversight: whoever starts the drain owns
        //  the connection lock until the drain is set up.
        //
        //  This was wrong in the first cut, and the score did not catch it:
        //  unlocking on both paths compiles to the same 88 instructions,
        //  because MSVC tail-duplicates the shared unlock onto the error path
        //  and the totals coincide.  Only reading the two streams
        //  side by side shows the extra call.
        OnQueued(fresh);
        return;
    }

    LeaveCriticalSection(&m_critSec);
}

}  // namespace OsNet
