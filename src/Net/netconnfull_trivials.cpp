//============================================================================
//  OsNet::NETCONNFULL - the three no-op TCPCONN-shaped setters (slots
//  12-14).  Defined out-of-line rather than inline in osnetfamily.h so
//  they are emitted regardless of whether this build's own callee closure
//  reaches them - this class has no constructor in scope this session, so
//  nothing forces its vtable (and hence its inline virtuals) to be emitted
//  otherwise.
//============================================================================
#include "osnetfamily.h"

namespace OsNet
{

//----------------------------------------------------------------------------
//  0x6F6DE980 - retn 4h.
//----------------------------------------------------------------------------
void NETCONNFULL::SetCoalesce(int coalesce)
{
    (void)coalesce;
}

//----------------------------------------------------------------------------
//  0x6F6DE990 - retn 4h.
//----------------------------------------------------------------------------
BOOL NETCONNFULL::SetBufferSizes(int size)
{
    (void)size;
    return FALSE;
}

//----------------------------------------------------------------------------
//  0x6F6DE9A0 - retn 4h.
//----------------------------------------------------------------------------
int NETCONNFULL::SetRecvTimeout(int ms)
{
    (void)ms;
    return 0;
}

}  // namespace OsNet
