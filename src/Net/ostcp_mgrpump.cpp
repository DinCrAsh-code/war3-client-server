//============================================================================
//  OsNet::TCPMGR::Pump (0x6F6E4070) - decide whether there is a port to pump.
//
//  Its own translation unit so PumpIocp stays a real (tail-)call.
//============================================================================
#include "ostcp.h"

namespace OsNet
{

//----------------------------------------------------------------------------
//  0x6F6E4070 - retn 4, and every one of its three exits is a tail jump: two
//  to Sleep and one to PumpIocp.
//
//  The two Sleep tails are separate in the dump rather than tail-merged, which
//  is why they are written as two explicit statements here: folding them into
//  one trailing Sleep costs the second `jmp`.
//----------------------------------------------------------------------------
void TCPMGR::Pump(DWORD ms)
{
    if (m_pumpThreads.m_count != 0)
    {
        Sleep(ms);
        return;
    }

    if (m_iocp != 0)
    {
        PumpIocp(ms);
        return;
    }

    Sleep(ms);
}

}  // namespace OsNet
