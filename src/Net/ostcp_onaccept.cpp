//============================================================================
//  OsNet::TCPMGR::OnAcceptComplete (0x6F6E37D0) - an accept completion came
//  back from the port; turn it into a connection.
//
//  Its own translation unit so TCPMGR::PumpIocp keeps the call.
//============================================================================
#include "ostcp.h"

namespace OsNet
{

//----------------------------------------------------------------------------
//  0x6F6E37D0 - retn 8.
//
//  The 32-byte address pair goes straight from the accept slot into the new
//  connection, which copies it into NETCONN::m_addrs and hands its address to
//  the owner's callback from then on.  The callback pair itself comes off the
//  listener, so every connection accepted through one listener is born
//  notifying the same place.
//
//  Nothing is sent on the new connection here: the `buf`/`len` pair
//  CreateConnection takes for an initial write is passed null/zero.
//----------------------------------------------------------------------------
void TCPMGR::OnAcceptComplete(TCPACCEPT* slot, int accepted)
{
    char addrs[0x20];

    SOCKET s = slot->FinishAccept(addrs, accepted);
    if (s == INVALID_SOCKET)
        return;

    CreateConnection(s, slot->m_listener->m_notifyProc,
                     slot->m_listener->m_notifyCtx, addrs, 0, 0);
}

}  // namespace OsNet
