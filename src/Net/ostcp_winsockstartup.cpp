//============================================================================
//  0x6F6D9FB0 - OsNetWinsockStartup.  WSAStartup(2.2), then look up the four
//  extension entry points that are not importable: AcceptEx and
//  GetAcceptExSockaddrs out of mswsock.dll, and WSASend/WSARecv/WSAIoctl out
//  of ws2_32.dll when the negotiated version really is 2.x.
//
//  The version gate is done on the two halves of `wVersion` separately -
//  low byte (major) below 1 fails outright, equal to 1 with a high byte
//  (minor) below 1 fails too - which is `wVersion < MAKEWORD(1,1)` written
//  the way a little-endian WORD comparison falls out.
//============================================================================
#include "ostcp.h"

namespace OsNet
{


//  A TCPMGR member, not a free function: the shipped body is `retn 4`
//  against a receiver in ecx and one push - OsNetInitialize calls it with
//  the manager in ecx exactly as it calls the other three starts - and it
//  reads neither.  A `__fastcall` free function taking the flags in ecx
//  would clean four bytes too few (abi_audit.py FATAL).
int TCPMGR::StartWinsock(int flags)
{
    (void)flags;

    if (WSAStartup(0x0202, &g_wsaData) != 0)
        return 0;

    if (LOBYTE(g_wsaData.wVersion) < 1)
        return 0;
    if (LOBYTE(g_wsaData.wVersion) == 1 && HIBYTE(g_wsaData.wVersion) < 1)
        return 0;

    g_hMswsock = LoadLibraryA("mswsock.dll");
    if (g_hMswsock != 0)
    {
        g_pfnAcceptEx = (BOOL (__stdcall*)(SOCKET, SOCKET, void*, DWORD, DWORD, DWORD, LPDWORD, LPOVERLAPPED))
            GetProcAddress(g_hMswsock, "AcceptEx");
        g_pfnGetAcceptExSockaddrs =
            (void (__stdcall*)(void*, DWORD, DWORD, DWORD, struct sockaddr**,
                               int*, struct sockaddr**, int*))
            GetProcAddress(g_hMswsock, "GetAcceptExSockaddrs");
    }

    if (LOBYTE(g_wsaData.wVersion) >= 2)
    {
        g_hWs2_32 = LoadLibraryA("ws2_32.dll");
        if (g_hWs2_32 != 0)
        {
            g_pfnWSASend  = (int (__stdcall*)(SOCKET, LPWSABUF, DWORD, LPDWORD,
                                              DWORD, LPWSAOVERLAPPED, void*))
                            GetProcAddress(g_hWs2_32, "WSASend");
            g_pfnWSARecv  = (int (__stdcall*)(SOCKET, LPWSABUF, DWORD, LPDWORD,
                                              LPDWORD, LPWSAOVERLAPPED, void*))
                            GetProcAddress(g_hWs2_32, "WSARecv");
            g_pfnWSAIoctl = (void*)GetProcAddress(g_hWs2_32, "WSAIoctl");
        }
    }

    return 1;
}

}  // namespace OsNet
