//============================================================================
//  OsNet::NETCONN::NotifyConnected (0x6F6DA240) and NotifyDisconnected
//  (0x6F6DA280) - the two calls into whatever registered itself with this
//  connection.
//
//  They are byte-for-byte the same function apart from one pushed immediate
//  (0 against 2), which is worth stating because the obvious refactor - one
//  Notify(int reason) with two thin wrappers - is *not* what shipped: there
//  are two full bodies at two addresses, and a wrapper pair would put a `call`
//  in each of them where the shipped code has the callback invocation itself.
//
//  Kept in one translation unit because neither calls the other; ReadNotify
//  lives in its own (ostcp_notifyread.cpp) because both call it.
//============================================================================
#include "netconn.h"

namespace OsNet
{

//----------------------------------------------------------------------------
//  0x6F6DA240 - reason 0.
//
//  The callback gets both the context value and the address of the local that
//  holds it, which looked redundant until NotifyData (0x6F6DA2C0) turned up
//  and typed the prototype: the seventh argument is the `consumed` out-slot
//  the receive path actually uses, and these two reasons never read it, so
//  they hand it a throwaway address.  One prototype, three reasons.
//----------------------------------------------------------------------------
int NETCONN::NotifyConnected()
{
    CONNNOTIFYPROC proc;
    void* ctx;

    ReadNotify(&proc, &ctx);
    //  Tested positively so the callback invocation is the fall-through and
    //  the `return 0` sinks to the end, which is the way round the dump
    //  branches.
    if (proc != 0)
        return proc(this, &m_addrs, 0, ctx, 0, 0, (unsigned int*)&ctx);
    return 0;
}

//----------------------------------------------------------------------------
//  0x6F6DA280 - reason 2.
//----------------------------------------------------------------------------
int NETCONN::NotifyDisconnected()
{
    CONNNOTIFYPROC proc;
    void* ctx;

    ReadNotify(&proc, &ctx);
    //  Tested positively so the callback invocation is the fall-through and
    //  the `return 0` sinks to the end, which is the way round the dump
    //  branches.
    if (proc != 0)
        return proc(this, &m_addrs, 2, ctx, 0, 0, (unsigned int*)&ctx);
    return 0;
}

}  // namespace OsNet
