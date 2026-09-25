//============================================================================
//  0x6F6E40C0 - OsNetWinsockShutdown.  Free the two extension libraries (and
//  null the four entry points they provided), WSACleanup, and then - only on
//  a 9x kernel, where the completion-port paths were never used - terminate
//  and reap every thread record the manager still holds.
//
//  The reap loop is written as `for (;;)` over the list *tail* because that
//  is what the shipped stream does: TCPTHREADREC::Destruct unlinks the node
//  it is given, so re-reading the tail each time is the walk, and the loop
//  ends when the tail goes negative (the terminator).
//============================================================================
#include "ostcp.h"

namespace OsNet
{


void __fastcall OsNetWinsockShutdown()
{
    if (g_hWs2_32 != 0)
    {
        g_pfnWSASend = 0;
        g_pfnWSARecv = 0;
        FreeLibrary(g_hWs2_32);
        g_hWs2_32 = 0;
    }

    if (g_hMswsock != 0)
    {
        g_pfnAcceptEx = 0;
        g_pfnGetAcceptExSockaddrs = 0;
        FreeLibrary(g_hMswsock);
        g_hMswsock = 0;
    }

    WSACleanup();

    if (g_osTcpLegacyOs != 0)
        return;

    WaitForSingleObject(g_pTcpManager->m_idleEvent, INFINITE);

    for (;;)
    {
        TCPTHREADREC* rec =
            (TCPTHREADREC*)g_pTcpManager->m_threads.TailLink();
        if ((int)rec <= 0)
            break;

        TerminateThread(rec->m_thread, 0);
        rec->Destruct();
        SMemFree(rec, "delete", -1, 0);
        g_pTcpManager->Release();
    }

    SetEvent(g_pTcpManager->m_idleEvent);
}

}  // namespace OsNet
