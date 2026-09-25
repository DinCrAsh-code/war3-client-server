//============================================================================
//  Net::NetProviderLTCP - vtable slot 1, 0x6F6630A0.  Bring the LAN TCP
//  provider up: its own module-wide critical section and reference count
//  (`stru_6FACFE64`/`dword_6FACFE60` - distinct from NetProviderLOOP's own
//  `stru_6FACFE48`/`dword_6FACFE44` pair, netproviderloop_initialize.cpp)
//  gate a one-shot OsNetInitialize call, then the same generic per-object
//  config copy netproviderloop_initialize.cpp's own CopyConfig thunk
//  reaches (0x6F6602D0 - the same address, called again here) runs.
//============================================================================
#include "netproviderltcp.h"

//  0x6F6602D0 - real now in netprovider_copyconfig.cpp
//  (Net::CopyProviderConfigTarget::CopyConfig) - one shipped address shared
//  by all three providers' own Initialize; see that file's own header.
namespace Net
{
class CopyProviderConfigTarget
{
public:
    void CopyConfig(void* configStruct);
};
}
typedef Net::CopyProviderConfigTarget NetProviderLtcpInitTarget;

//  osnetaliases.cpp - the image's one-instruction alias for OsNetInitialize.
int __fastcall OsNetInitializeAlias(int, unsigned int);

//  critsec.cpp
void __fastcall EnterCritSec(CRITICAL_SECTION* cs);
void __fastcall LeaveCritSec(CRITICAL_SECTION* cs);

//  funcmap.py DATA - this class's own module-global lock and reference
//  count, shared with Shutdown (netproviderltcp_lifecycle.cpp).
extern CRITICAL_SECTION g_netProviderLtcpGlobalLock;   // stru_6FACFE64
extern int              g_netProviderLtcpInitCount;    // dword_6FACFE60

namespace Net
{

unsigned int NetProviderLTCP::Initialize(void* configStruct, int* outStatus)
{
    *outStatus = 0;

    const unsigned int kSuccess = 1;
    EnterCritSec(&g_netProviderLtcpGlobalLock);
    if (m_enabled)
    {
        LeaveCritSec(&g_netProviderLtcpGlobalLock);
        return 6;
    }

    int oldInitCount = g_netProviderLtcpInitCount++;
    if (oldInitCount == 0)
    {
        int initResult = OsNetInitializeAlias(oldInitCount, 3);
        if (!initResult)
        {
            --g_netProviderLtcpInitCount;
            LeaveCritSec(&g_netProviderLtcpGlobalLock);
            return initResult + 3;
        }
    }

    ((NetProviderLtcpInitTarget*)((char*)this + 0x18))->CopyConfig(configStruct);
    m_enabled = kSuccess;
    LeaveCritSec(&g_netProviderLtcpGlobalLock);
    return kSuccess;
}

}  // namespace Net
