//============================================================================
//  OsNet::LOOPCONNECT - the loopback-pair twin of TCPCONNECT/FILECONNECT
//  (tcpconnect.cpp, fileconnect.cpp).  See osnetfamily.h for the class and
//  its recovered layout.
//============================================================================
#include "osnetfamily.h"

namespace OsNet
{

typedef void (__thiscall *AnnounceReason3Fn)(void* thisObj,
                                             CONNNOTIFYPROC proc,
                                             const void* addrs);
typedef void (__thiscall *ConnectFinishLoopFn)(void* obj, int arg);

//----------------------------------------------------------------------------
//  0x6F6DFF60 - slot 1, retn 0.
//----------------------------------------------------------------------------
BOOL LOOPCONNECT::IsSocketClosed()
{
    return FALSE;
}

//----------------------------------------------------------------------------
//  0x6F6D9C10 - slot 3, retn 0.  Same "zero a local buffer, announce reason
//  3 through the stored proc" shape as TCPCONNECT::Abort - no socket to
//  close, a loopback attempt never has one.
//----------------------------------------------------------------------------
void LOOPCONNECT::Abort()
{
    char local[0x20];
    ZeroMemory(local, sizeof(local));

    ((AnnounceReason3Fn)0x6F6D9BC0)(this, m_notifyProc, local);
}

//----------------------------------------------------------------------------
//  0x6F6E3EA0 - slot 4, retn 4h.  Unlike TCPCONNECT's and FILECONNECT's own
//  slot 4, this one branches first: when m_pending is already zero it
//  dispatches slot 3 (Abort) *through the vtable* rather than the shared
//  finisher (0x6F6E3860, out of this session's scope) - the pairing partner
//  never showed up, so there is nothing left to finish.  The vtable
//  dispatch (rather than a plain `Abort()` call) is what the dump itself
//  does (`mov eax,[ecx]; call [eax+0Ch]`), and is why `arg` goes unused on
//  this path.
//----------------------------------------------------------------------------
void LOOPCONNECT::OnConnectResult(int arg)
{
    if (m_pending == 0)
    {
        typedef void (__thiscall *AbortFn)(void*);
        ((AbortFn)(*(void***)this)[3])(this);
        return;
    }

    ((ConnectFinishLoopFn)0x6F6E3860)((void*)arg, (int)this);
}

}  // namespace OsNet
