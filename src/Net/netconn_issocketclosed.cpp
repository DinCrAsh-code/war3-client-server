//============================================================================
//  OsNet::NETCONN::IsSocketClosed - vtable slot 1, 0x6F6D9A10.
//
//  NETCONN's own; TCPCONN and IOTCPCONN both inherit it unchanged, and no
//  dump in this repo calls it, so the name is inferred from the body alone -
//  see netconn.h's slot 1 comment.
//============================================================================
#include "netconn.h"

namespace OsNet
{

//----------------------------------------------------------------------------
//  0x6F6D9A10 - retn 0.  Four instructions: `return m_socket ==
//  INVALID_SOCKET;`
//----------------------------------------------------------------------------
BOOL NETCONN::IsSocketClosed()
{
    return m_socket == INVALID_SOCKET;
}

}  // namespace OsNet
