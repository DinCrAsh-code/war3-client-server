//============================================================================
//  OsNet::FILECONNECT - the file-connect twin of TCPCONNECT (tcpconnect.cpp)
//  - a small, standalone object a file-open attempt lives in while it is in
//  flight.  See osnetfamily.h for the class and its recovered layout.
//============================================================================
#include "osnetfamily.h"

namespace OsNet
{

typedef void (__thiscall *AnnounceReason3Fn)(void* thisObj,
                                             CONNNOTIFYPROC proc,
                                             const void* addrs);
typedef void (__thiscall *ConnectFinishFileFn)(void* obj, int arg);

//----------------------------------------------------------------------------
//  0x6F6DEE00 - slot 1, retn 0.
//----------------------------------------------------------------------------
BOOL FILECONNECT::IsPending()
{
    return m_handleOrSentinel == -1;
}

//----------------------------------------------------------------------------
//  0x6F6D9D40 - slot 3, retn 0.  Same "zero a local buffer, announce reason
//  3 through the stored proc" shape as TCPCONNECT::Abort - no socket/handle
//  close here, since FILECONNECT's own m_handleOrSentinel is not a live
//  handle to release at this point (only IsPending's own sentinel test).
//----------------------------------------------------------------------------
void FILECONNECT::Abort()
{
    char local[0x20];
    ZeroMemory(local, sizeof(local));

    ((AnnounceReason3Fn)0x6F6D9BC0)(this, m_notifyProc, local);
}

//----------------------------------------------------------------------------
//  0x6F6E3ED0 - slot 4, retn 4h.  Same shared-finisher tail-call shape as
//  TCPCONNECT::OnConnectResult, into the file-flavoured finisher
//  (0x6F6E38E0, out of this session's scope).
//----------------------------------------------------------------------------
void FILECONNECT::OnConnectResult(int arg)
{
    ((ConnectFinishFileFn)0x6F6E38E0)((void*)arg, (int)this);
}

}  // namespace OsNet
