//============================================================================
//  Net::NetProviderLTCP - vtable slot 15, 0x6F661B20.  Try each port in
//  [port, limit) in turn against the real socket-bind pipeline and report
//  the first one that succeeds (or 0 on total failure).  Unlike
//  NetProviderLOOP::BindPort (netproviderloop_bindport.cpp) there is no
//  in-object port table here - each candidate is asked of the OS directly.
//============================================================================
#include "netproviderltcp.h"

//  lpAddend - the module's TCPMGR singleton (OsNet::g_pTcpManager,
//  ostcp.h), null before the module is started.
namespace OsNet { class TCPMGR; }
extern OsNet::TCPMGR* g_pTcpManager;

//  0x6F6E1700 - deep socket/bind/listen pipeline (EnterCriticalSection,
//  CreateIoCompletionPort, setsockopt, htons, bind, listen, closesocket on
//  failure) well below this session's scope; `this` = g_pTcpManager, three
//  stack args, `retn 0Ch` read off the real call site's own push count.
//  Naked redirect - argument count only, not per-argument meaning.
class TcpMgrPortBinder
{
public:
    int BindOnePort(unsigned short port, int target, int arg8);
};
__declspec(naked) int TcpMgrPortBinder::BindOnePort(unsigned short, int, int)
{
    __asm { mov eax, 0x6F6E1700 }
    __asm { jmp eax }
}

//  0x6F6E2A90 - `__fastcall(ecx=hostshort port, edx=target)` plus one stack
//  arg, `retn 4`; forwards into BindOnePort when the module is up, else
//  returns 0 without touching the OS.
__declspec(naked) int __fastcall TryBindPort(unsigned short port, int target, int arg8)
{
    __asm
    {
        mov     eax, g_pTcpManager
        test    eax, eax
        jz      short not_started
        push    esi
        mov     esi, [esp+8]
        push    esi
        push    edx
        push    ecx
        mov     ecx, eax
        call    TcpMgrPortBinder::BindOnePort
        pop     esi
        retn    4
not_started:
        xor     eax, eax
        retn    4
    }
}

namespace Net
{

unsigned short NetProviderLTCP::BindPort(unsigned short port, unsigned short limit, int target)
{
    for (unsigned short candidate = port; candidate < limit; ++candidate)
    {
        if (TryBindPort(candidate, target, 0) != 0)
            return candidate;
    }
    return 0;
}

}  // namespace Net
