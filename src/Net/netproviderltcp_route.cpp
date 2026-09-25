//============================================================================
//  Net::NetProviderLTCP - vtable slot 14, 0x6F661AF0.  Unlike the loopback
//  provider's own RouteToPort (netproviderloop_route.cpp), this class does
//  no port-table lookup of its own here - it just pulls the raw address
//  value out of `addr` and forwards straight into the connection-layer
//  delivery routine, which is where any port-based dispatch actually lives.
//============================================================================
#include "netproviderltcp.h"

unsigned int __fastcall GetAddrIPAndPort(const void* addr, unsigned short* outPort);  // sockaddrinet.cpp

//  lpAddend - the module's TCPMGR singleton (OsNet::g_pTcpManager,
//  ostcp.h), null before the module is started.
namespace OsNet { class TCPMGR; }
extern OsNet::TCPMGR* g_pTcpManager;

//  0x6F6E1EE0 - deep OsTcp connection-lookup and socket-connect pipeline
//  (EnterCriticalSection/LeaveCriticalSection, a Storm_401 allocation
//  tagged `OsNet::TCPCONNECT`'s own vtable, `memcpy`, and a further forward
//  into the socket/ioctlsocket/connect chain netproviderloop_route.cpp's
//  own RouteDeliver already documents as well below this session's scope);
//  `this` = g_pTcpManager, six stack args, `retn 18h` read off the real
//  call site's own push count.  Naked redirect - argument count only, not
//  per-argument meaning.
class TcpMgrConnectDeliverer
{
public:
    void Deliver(int addrValue, const void* addr, int arg4, int arg8, const void* data, unsigned int size);
};
__declspec(naked) void TcpMgrConnectDeliverer::Deliver(int, const void*, int, int, const void*, unsigned int)
{
    __asm { mov eax, 0x6F6E1EE0 }
    __asm { jmp eax }
}

//  0x6F6E2AE0 - `__fastcall(ecx,edx)` plus four stack args, `retn 10h`;
//  forwards straight into TcpMgrConnectDeliverer::Deliver when the module
//  is up, else a no-op.
//
//  The second argument (`edx`) is the caller's own `addr` stack slot,
//  re-read after GetAddrIPAndPort has (conditionally) written 2 bytes of a
//  port value into it - the same argument-slot-reused-as-scratch idiom, kept
//  exactly as the dump shows it rather than "fixed" into a separate local.
__declspec(naked) void __fastcall LtcpRouteDeliver(int addrValue, const void* addr, int arg4, int arg8, const void* data, unsigned int size)
{
    __asm
    {
        mov     eax, g_pTcpManager
        test    eax, eax
        jz      short not_started
        push    esi
        mov     esi, [esp+0Ch]
        push    esi
        mov     esi, [esp+0Ch]
        push    esi
        mov     esi, [esp+0Ch]
        push    esi
        mov     esi, [esp+0Ch]
        push    esi
        push    edx
        push    ecx
        mov     ecx, eax
        call    TcpMgrConnectDeliverer::Deliver
        pop     esi
not_started:
        retn    10h
    }
}

namespace Net
{

void NetProviderLTCP::RouteToPort(const void* addr, int arg4, int arg8, const void* data, unsigned int size)
{
    unsigned int addrValue = GetAddrIPAndPort(addr, (unsigned short*)&addr);
    LtcpRouteDeliver(addrValue, addr, arg4, arg8, data, size);
}

}  // namespace Net
