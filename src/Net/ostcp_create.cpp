//============================================================================
//  OsNet::TCPMGR::CreateConnection (0x6F6E34C0) - build the connection kind
//  this manager is configured for.
//
//  Its own translation unit so both derived constructors stay real calls.
//============================================================================
#include "ostcp.h"

namespace OsNet
{

static const char kW32OsTcpCpp[] = ".\\W32\\OsTcp.cpp";

//----------------------------------------------------------------------------
//  0x6F6E34C0 - retn 18h.
//
//  Two placement news over Storm's allocator, and the two sizes are what fix
//  both derived classes' layouts: 0x650 for the completion-port connection and
//  0x630 for the select one, which is also sizeof(TCPCONN) - SLTCPCONN adds no
//  data at all.  The line numbers (0xAB3 and 0xABD) are the shipped
//  __LINE__s and are kept because they are the only thing in the stream that
//  distinguishes the two allocation sites.
//
//  A failed allocation falls straight out with a null: placement new's own
//  test skips the constructor and the expression's value is the null pointer,
//  which is what both `jz` targets in the dump land on.
//----------------------------------------------------------------------------
TCPCONN* TCPMGR::CreateConnection(SOCKET s, CONNNOTIFYPROC proc, void* ctx,
                                  const void* addrs, const void* buf, int len)
{
    if (m_iocp != 0)
        return new (SMemAlloc(sizeof(IOTCPCONN), kW32OsTcpCpp, 0xAB3, 0))
                    IOTCPCONN(this, m_iocp, s, proc, ctx, addrs, buf, len);

    return new (SMemAlloc(sizeof(SLTCPCONN), kW32OsTcpCpp, 0xABD, 0))
                SLTCPCONN(this, s, proc, ctx, addrs, buf, len);
}

}  // namespace OsNet
