//============================================================================
//  OsNet::TCPCONN::Shutdown (0x6F6E00C0) - close a connection down and drop
//  the reference that was keeping it alive.
//
//  Its own translation unit: three different callers reach it with a real
//  call (both derived constructors and IOTCPCONN::ReleasePending).
//============================================================================
#include "ostcp.h"

namespace OsNet
{

//----------------------------------------------------------------------------
//  0x6F6E00C0 - retn 4.
//
//  `notify` is what tells the two failure paths apart from the ordinary one:
//  a connection that never got as far as telling its owner it existed is torn
//  down with notify clear, so the owner never hears about a disconnect it was
//  never told to expect.
//----------------------------------------------------------------------------
void TCPCONN::Shutdown(int notify)
{
    Close();
    m_owner->MoveConn(this, kConnStateNone);

    if (notify)
        NotifyDisconnected();

    ReleaseRaw();
}

}  // namespace OsNet
