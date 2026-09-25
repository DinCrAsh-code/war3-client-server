//============================================================================
//  OsNet::IOTCPCONN::AddPending - vtable slot 5, 0x6F6DA8C0.
//
//  nullsub in NETCONN and TCPCONN; this is IOTCPCONN's own override, the
//  exact mirror of ReleasePending (slot 6, ostcp_iotcpconn.cpp), which
//  decrements the same field.  Called by IOTCPCONN::OnQueued
//  (iotcpconn_onqueued.cpp) before an overlapped send goes out, and by
//  PostRead before arming the receive.
//============================================================================
#include "ostcp.h"

namespace OsNet
{

//----------------------------------------------------------------------------
//  0x6F6DA8C0 - retn 0.  `InterlockedIncrement(&m_pending)`.
//----------------------------------------------------------------------------
void IOTCPCONN::AddPending()
{
    InterlockedIncrement(&m_pending);
}

}  // namespace OsNet
