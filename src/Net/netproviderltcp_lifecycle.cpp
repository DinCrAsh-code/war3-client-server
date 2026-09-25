//============================================================================
//  Net::NetProviderLTCP - vtable slot 2, 0x6F661F80.  Bring the provider
//  down: the same base-interface "code 4" notify (0x6F65BE40 - already
//  reconstructed as Net::NetProvider::Method_0x2, netprovider_slot2.cpp,
//  and reached the identical way NetProviderLOOP::Shutdown reaches it,
//  netproviderloop_lifecycle.cpp) plus stopping the listener thread, then
//  this class's own module-global lock/refcount dance
//  (netproviderltcp_initialize.cpp's own Initialize uses the same pair)
//  gating a one-shot OsNetShutdown call.
//============================================================================
#include "netproviderltcp.h"

//  netprovider_slot2.cpp
namespace Net { class NetProvider { public: int Method_0x2(); }; }

//  osnetaliases.cpp
void __fastcall OsNetShutdownAlias(unsigned int);

//  critsec.cpp
void __fastcall EnterCritSec(CRITICAL_SECTION* cs);
void __fastcall LeaveCritSec(CRITICAL_SECTION* cs);

//  netproviderltcp_initialize.cpp
extern CRITICAL_SECTION g_netProviderLtcpGlobalLock;   // stru_6FACFE64
extern int              g_netProviderLtcpInitCount;    // dword_6FACFE60

namespace Net
{

int NetProviderLTCP::Shutdown()
{
    ((Net::NetProvider*)this)->Method_0x2();
    StopListenerThread();

    EnterCritSec(&g_netProviderLtcpGlobalLock);
    if (!m_enabled)
    {
        LeaveCritSec(&g_netProviderLtcpGlobalLock);
        return 5;
    }

    if (--g_netProviderLtcpInitCount == 0)
        OsNetShutdownAlias(3);

    m_enabled = 0;
    LeaveCritSec(&g_netProviderLtcpGlobalLock);
    return 1;
}

}  // namespace Net
