//============================================================================
//  OsNet::NETCONN::NotifyData (0x6F6DA2C0) - the third and last of the notify
//  trio, and the only one that uses the whole callback prototype.
//
//  Its own translation unit so TCPCONN::OnRecvComplete keeps the call.  The
//  three sit next to each other in the binary - 0x6F6DA240, 0x6F6DA280,
//  0x6F6DA2C0 - which is what one shipped source file of three near-identical
//  wrappers looks like.
//============================================================================
#include "netconn.h"

namespace OsNet
{

//----------------------------------------------------------------------------
//  0x6F6DA2C0 - retn 10h.
//
//  Same seqlock read and same null guard as the other two; the difference is
//  that this one forwards a buffer, a length and an out-slot, and lets the
//  caller override which address pair the callback is told about.  A null
//  `addrs` means "the connection's own", which is the only way this call tree
//  ever reaches it - the override exists for a caller that has a per-packet
//  source address to report, which is not in either dump here.
//
//  The return value is the callback's, and TCPCONN::OnRecvComplete treats
//  zero as fatal: no callback registered means nothing can ever consume the
//  receive buffer, so the connection is closed rather than left to fill up.
//----------------------------------------------------------------------------
int NETCONN::NotifyData(const void* data, unsigned int len,
                        unsigned int* consumed, const void* addrs)
{
    CONNNOTIFYPROC proc;
    void* ctx;

    ReadNotify(&proc, &ctx);
    //  Tested positively so the callback invocation is the fall-through and
    //  the `return 0` sinks to the end, which is the way round the dump
    //  branches.
    if (proc != 0)
    {
        if (addrs == 0)
            addrs = &m_addrs;

        return proc(this, (void*)addrs, 1, ctx, data, len, consumed);
    }
    return 0;
}

}  // namespace OsNet
