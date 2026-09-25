//============================================================================
//  OsNet::TCPMGR::PumpIocp (0x6F6E3EE0) - the completion-port loop.
//
//  Its own translation unit so OnAcceptComplete stays a real call.
//============================================================================
#include "ostcp.h"

namespace OsNet
{

//----------------------------------------------------------------------------
//  0x6F6E3EE0 - retn 4.
//
//  A time-budgeted drain: keep taking completions off the port until the
//  budget is spent, the module is shutting down, or (for a finite budget) the
//  port hands back a timeout with no operation attached.  An INFINITE budget
//  skips all the timing and never leaves on the timeout path.
//
//  Two details are worth stating because they look like transcription noise
//  and are not:
//
//  * The elapsed-time clock is picked per iteration, not once - both the start
//    reading at the top and every re-reading in the loop test g_osTcpUseQpc
//    afresh.  That is the shipped code; it costs a global read per iteration
//    and it is what the dump has at both sites.
//
//  * The negative test on `elapsed` is real, and it is a signed test on an
//    unsigned variable - which is exactly what the dump has, `jl` and `jnb`
//    on the same register a few instructions apart.  A backwards-stepping
//    performance counter (which is what g_osTcpUseQpc exists to work around
//    in the first place) truncates to a negative, and without the test that
//    would wrap into an enormous unsigned budget and hang the loop.
//
//  The completion key is the connection; a completion with no key is an
//  accept, and the slot it belongs to is recovered by stepping back from the
//  OVERLAPPED to the object that contains it.
//----------------------------------------------------------------------------
void TCPMGR::PumpIocp(DWORD ms)
{
    LARGE_INTEGER start;
    //  Unsigned, and the cast below is to this type rather than to `int`.
    //  That is what the dump's own conversion says: MSVC lowers a float-to-
    //  `int` truncation to a `__ftol2` call, and only a float-to-*unsigned*
    //  one to the inline fnstcw / or 0C00h / fldcw / fistp / fldcw sequence
    //  the dump has.  The signed test on the result is then spelled with an
    //  explicit cast, which is where the `jl` comes from.
    DWORD elapsed = 0;

    start.QuadPart = 0;
    if (ms != INFINITE)
    {
        if (g_osTcpUseQpc != 0)
            QueryPerformanceCounter(&start);
        else
            start.LowPart = GetTickCount();
    }

    for (;;)
    {
        DWORD nbytes;
        ULONG_PTR key;
        OVERLAPPED* ov;

        BOOL ok = GetQueuedCompletionStatus(m_iocp, &nbytes, &key, &ov,
                                            ms - elapsed);
        if (g_osTcpShutdown != 0)
            return;

        if (ms != INFINITE && !ok && ov == 0)
            return;

        if (key != 0)
        {
            //  Spelled out at each use rather than named once: naming it
            //  parks the connection in a callee-saved register and pushes
            //  `this` onto the stack, where the shipped code does the
            //  opposite - `this` stays in a register for the whole loop and
            //  the key is re-read from its own out-slot at each use.  See
            //  docs/msvc-vc8-idioms.md, "Naming an intermediate costs you
            //  the shipped code's register reuse".
            if (ov != 0)
            {
                //  A `switch`, not an if/else chain: the dump dispatches with
                //  the `sub reg,0` / `je` / `sub reg,1` / `jne` ladder MSVC
                //  lowers a small dense switch to, where an if/else chain
                //  would compare against each constant instead.
                switch (((IOOP*)ov)->m_type)
                {
                case kIoOpRecv:
                    ((NETCONN*)key)->OnRecvComplete((IOOP*)ov, nbytes, ok);
                    break;
                case kIoOpSend:
                    ((NETCONN*)key)->OnSendComplete((IOOP*)ov, nbytes, ok);
                    break;
                }
            }
            ((NETCONN*)key)->ReleasePending();
        }
        else if (ov != 0)
        {
            //  A one-case `switch` for the same reason as the two-case one
            //  above: the dump reaches this test with the `sub reg,2`/`jne`
            //  ladder, not with a `cmp`.
            switch (((IOOP*)ov)->m_type)
            {
            case kIoOpAccept:
                //  The accept slot's IOOP sits at +0x08, so the OVERLAPPED
                //  the port handed back is eight bytes into the object.
                OnAcceptComplete((TCPACCEPT*)((char*)ov - 8), ok);
                break;
            }
        }

        if (ms == INFINITE)
            continue;

        if (g_osTcpUseQpc != 0)
        {
            LARGE_INTEGER now;
            QueryPerformanceCounter(&now);

            elapsed = (DWORD)((now.QuadPart - start.QuadPart)
                              * g_qpcMsPerCount);
            if ((int)elapsed < 0)
                return;
        }
        else
        {
            elapsed = GetTickCount() - start.LowPart;
        }

        if (elapsed >= ms)
            return;
    }
}

}  // namespace OsNet
