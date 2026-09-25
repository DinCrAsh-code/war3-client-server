//============================================================================
//  0x6F6E4E30 - OsNetShutdown.  The mirror of OsNetInitialize: one reference
//  count per bit, and the stop for a subsystem runs only on the 1 -> 0 edge.
//
//  The order is not the initialize order reversed - it is select, misc,
//  IOCP, Winsock, timer, core - and the whole body is skipped when there is
//  no manager, which is what makes a shutdown before any initialize safe.
//  `mask` is a local whose address escapes into OsNetExpandMask, so the same
//  re-read-after-every-call shape as OsNetInitialize applies here.
//============================================================================
#include "ostcp.h"

namespace OsNet
{

void __fastcall OsNetShutdown(unsigned int mask)
{
    OsNetExpandMask(&mask);
    LazyInitAndEnterDebugLock(&g_osNetLock);

    if (g_pTcpManager != 0)
    {
        if (mask & 2)
        {
            if (--g_osNetRefSelect == 0)
                g_pTcpManager->StopSelectThreads();
        }

        if (mask & 4)
            --g_osNetRefMisc;

        if (mask & 0x20)
        {
            if (--g_osNetRefIocp == 0)
                g_pTcpManager->StopIocp();
        }

        if (mask & 0x10)
        {
            if (--g_osNetRefWinsock == 0)
                OsNetWinsockShutdown();
        }

        if (mask & 1)
        {
            if (--g_osNetRefTimer == 0)
                g_pTcpManager->StopTimerThread();
        }

        if (mask & 8)
        {
            if (--g_osNetRefCore == 0)
            {
                g_pTcpManager->StopListenThread();
                g_pTcpManager->Release();
                g_pTcpManager = 0;

                if (g_osNetStormHandle != 0)
                {
                    Storm_541(g_osNetStormHandle);
                    g_osNetStormHandle = 0;
                }
            }
        }
    }

    LeaveCriticalSection(g_pOsNetLock);
}

}  // namespace OsNet
