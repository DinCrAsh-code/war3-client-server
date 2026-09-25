//============================================================================
//  OsNet::TCPCONN::Queue (0x6F6DEEC0) - append to the outbound queue.
//
//  Its own translation unit so TCPCONN::Send (ostcp_send.cpp) and both derived
//  constructors keep their calls.
//============================================================================
#include "ostcp.h"

namespace OsNet
{

//  The CRT intrinsic, recognised by name/signature under /O2 without
//  <string.h> (which /Zl keeps out of this build).
extern "C" void* __cdecl memcpy(void* dst, const void* src, unsigned int count);
#pragma intrinsic(memcpy)

static const char kW32OsTcpCpp[] = ".\\W32\\OsTcp.cpp";

//----------------------------------------------------------------------------
//  0x6F6DEEC0 - retn 8.  Returns the buffer the data ended up in.
//
//  Three things about this function are load-bearing and would be "cleaned up"
//  by the next reader otherwise:
//
//  * The staleness check at the top reads the *tail* link raw and dereferences
//    it as a buffer after one signed test.  Ten minutes since the connection
//    was created and two minutes since the oldest queued buffer was written
//    means the peer has stopped draining, and the connection is closed rather
//    than queued to.
//
//  * `buf` is left pointing at the head buffer even when the head could not be
//    topped up (sealed, or already full).  It is only reset to null when there
//    was no head at all.  So a caller that queues nothing new still gets the
//    head back, and that is what TCPCONN::Send's own null test is filtering.
//
//  * The new buffer is used unconditionally after `new (raw) OUTPUT()`.  The
//    null test MSVC emits in front of the constructor is placement new's, not
//    the source's - there is no second test, and a failed allocation walks
//    into a null store here exactly as it does in the shipped binary.  See
//    docs/msvc-vc8-idioms.md, "Placement new".
//----------------------------------------------------------------------------
OUTPUT* TCPCONN::Queue(const void* src, unsigned int size)
{
    DWORD now = GetTickCount();

    int tail = m_sendQueue.TailLink();
    if (tail > 0
        && now - m_createTick > 600000
        && now - ((OUTPUT*)tail)->m_tick > 120000)
    {
        Close();
        return 0;
    }

    OUTPUT* buf;
    unsigned int remaining;

    //  The `goto`s are the shipped control flow.  The dump reaches the
    //  top-up by a *forward* branch, with the block itself sitting past the
    //  allocation path and branching back into it, and `remaining` set from
    //  `size` afresh at each of the two entries into that join - which is
    //  what the labels below are.  This toolchain still lays the top-up out
    //  as the fall-through whichever way the source is written (a plain
    //  if/else, an inverted if/else and this were all measured: 41, 36 and 46
    //  of 131 respectively), so the shape only recovers part of it, but it is
    //  the best of the three and it is the one the dump's own branch
    //  directions describe.  Nothing about what runs changes between them:
    //  every path is the same sequence of reads, writes and calls in the same
    //  order.
    int head = m_sendQueue.HeadLink();
    if (head > 0)
        goto topup;

    buf = 0;

resize:
    remaining = size;

append:
    if (remaining != 0)
    {
        OUTPUT* fresh =
            new (SMemAlloc(sizeof(OUTPUT), kW32OsTcpCpp, 0x1228, 0)) OUTPUT();
        buf = fresh;

        fresh->m_sendState = 0;
        fresh->m_tick = now;
        fresh->m_used = remaining;

        unsigned int capacity = remaining > kTcpSegment ? remaining
                                                        : kTcpSegment;
        fresh->m_capacity = capacity;
        fresh->m_data = SMemAlloc(capacity, kW32OsTcpCpp, 0x122D, 0);
        memcpy(fresh->m_data, src, remaining);

        m_sendQueue.LinkToHead(fresh);
    }

    return buf;

topup:
    buf = (OUTPUT*)head;
    if (buf->m_sendState != 0)
        goto resize;
    if (buf->m_used >= buf->m_capacity)
        goto resize;

    remaining = size;
    {
        unsigned int space = buf->m_capacity - buf->m_used;
        if (space > remaining)
            space = remaining;

        memcpy((char*)buf->m_data + buf->m_used, src, space);
        buf->m_used += space;
        src = (const char*)src + space;
        remaining -= space;
    }
    goto append;
}

}  // namespace OsNet
