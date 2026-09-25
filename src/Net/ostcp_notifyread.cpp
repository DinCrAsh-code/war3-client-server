//============================================================================
//  OsNet::NETCONN::ReadNotify (0x6F6DA110) - the seqlock reader that both
//  notifiers go through.
//
//  Its own translation unit so NotifyConnected/NotifyDisconnected
//  (ostcp_notify.cpp) keep the real `call` the shipped code has instead of
//  inlining this loop twice.
//============================================================================
#include "netconn.h"

namespace OsNet
{

//----------------------------------------------------------------------------
//  0x6F6DA110 - retn 8.
//
//  m_notifySeq is deliberately *not* volatile, and that is what the shipped
//  instruction stream is: MSVC hoists the inner loop's load, so the "spin
//  while odd" test re-tests a value in a register and only the outer
//  `seq != m_notifySeq` re-reads memory.  Both retries land on the same
//  label, which is why one `jnz` target serves the two tests.  Written with
//  the load volatile the loop would reload inside the spin and the shape
//  would not match - and would also be a different program, so this is the
//  reconstruction, not a workaround.
//----------------------------------------------------------------------------
void NETCONN::ReadNotify(CONNNOTIFYPROC* outProc, void** outCtx)
{
    int seq;

    do
    {
        do
            seq = m_notifySeq;
        while ((seq & 1) != 0);

        *outProc = m_notifyProc;
        *outCtx = m_notifyCtx;
    }
    while (seq != m_notifySeq);
}

}  // namespace OsNet
