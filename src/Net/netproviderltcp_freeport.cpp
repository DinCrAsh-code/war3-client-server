//============================================================================
//  Net::NetProviderLTCP - vtable slot 16, 0x6F661B70.  Thin forward into
//  the real unbind/close pipeline; no in-object port table to update (see
//  netproviderltcp_bindport.cpp's own note).
//============================================================================
#include "netproviderltcp.h"

//  lpAddend - the module's TCPMGR singleton (OsNet::g_pTcpManager,
//  ostcp.h), null before the module is started.
namespace OsNet { class TCPMGR; }
extern OsNet::TCPMGR* g_pTcpManager;

//  0x6F6E1890 - deep unbind/listen/closesocket pipeline well below this
//  session's scope; `this` = g_pTcpManager, two stack args, `retn 8` read
//  off the real call site's own push count.  Naked redirect - argument
//  count only, not per-argument meaning.
class TcpMgrPortFreer
{
public:
    void FreeOnePort(int port, int zero);
};
__declspec(naked) void TcpMgrPortFreer::FreeOnePort(int, int)
{
    __asm { mov eax, 0x6F6E1890 }
    __asm { jmp eax }
}

//  0x6F6E2AC0 - `__fastcall(ecx=port, edx=0)`, `retn 0`; forwards into
//  TcpMgrPortFreer::FreeOnePort when the module is up, else a no-op.
__declspec(naked) void __fastcall FreeBoundPort(unsigned short port, int zero)
{
    __asm
    {
        mov     eax, g_pTcpManager
        test    eax, eax
        jz      short not_started
        push    edx
        push    ecx
        mov     ecx, eax
        call    TcpMgrPortFreer::FreeOnePort
not_started:
        retn
    }
}

namespace Net
{

void NetProviderLTCP::FreePort(unsigned short port)
{
    FreeBoundPort(port, 0);
}

}  // namespace Net
