//============================================================================
//  OsNet::IOTCPCONN::DeleteSelf - vtable slot 4, 0x6F6E25F0.
//
//  IOTCPCONN declares no destructor of its own (ostcp.h) - m_pending and
//  m_readOp are plain data with nothing to tear down - so its implicit
//  `~IOTCPCONN()` is nothing but a call into TCPCONN::~TCPCONN(), and the
//  shipped body of this override is bodily identical to TCPCONN::DeleteSelf
//  (tcpconn_deleteself.cpp) for exactly that reason: same real `call` into
//  0x6F6E0520, same `this != 0` test, same free.  Kept in its own
//  translation unit anyway, matching every other slot override in this pass
//  and keeping the "destructor definition not visible here" requirement
//  trivially true (this file only sees ostcp.h's declaration of
//  `~TCPCONN()`, never its body).
//============================================================================
#include "ostcp.h"

//  Storm.dll ordinal 403 - same declaration shape as refcnt_deleteself.cpp's.
void __stdcall SMemFree(void* ptr, const char* logfilename, int logline,
                        unsigned int flags);

static const char kDelete[] = "delete";

namespace OsNet
{

//----------------------------------------------------------------------------
//  0x6F6E25F0 - retn 4.
//----------------------------------------------------------------------------
IOTCPCONN* IOTCPCONN::DeleteSelf(int flags)
{
    this->TCPCONN::~TCPCONN();
    if ((flags & 1) && this != 0)
        SMemFree(this, kDelete, -1, 0);
    return this;
}

}  // namespace OsNet
