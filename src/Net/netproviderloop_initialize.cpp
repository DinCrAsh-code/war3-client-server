//============================================================================
//  Net::NetProviderLOOP - vtable slot 1, 0x6F661730.  Bring the loopback
//  provider up: a global, module-wide critical section and reference count
//  (shared with Shutdown/slot 2 and slot 3 in netproviderloop_lifecycle.cpp
//  - all three take the same `stru_6FACFE48`/`dword_6FACFE44` pair) gate a
//  one-shot OsNetInitialize call, then the provider's own per-object config
//  gets copied in.
//============================================================================
#include "netproviderloop.h"

//  0x6F6602D0 - the generic per-object config-copy body, real now in
//  netprovider_copyconfig.cpp (Net::CopyProviderConfigTarget::CopyConfig) -
//  one shipped address shared by all three providers' own Initialize, so
//  the body lives once and every call site declares the same class (same
//  namespace, same name - what the linker binds on) rather than keeping
//  its own redundant naked redirect.
namespace Net
{
class CopyProviderConfigTarget
{
public:
    void CopyConfig(void* configStruct);
};
}
typedef Net::CopyProviderConfigTarget NetProviderLoopInitTarget;

//  osnetaliases.cpp - the image's one-instruction alias for OsNetInitialize.
int __fastcall OsNetInitializeAlias(int, unsigned int);

//  critsec.cpp
void __fastcall EnterCritSec(CRITICAL_SECTION* cs);
void __fastcall LeaveCritSec(CRITICAL_SECTION* cs);

//  funcmap.py DATA - the module-global lock and reference count every one
//  of NetProviderLOOP's start/stop slots (0x6F661060, 0x6F6610D0 as well as
//  this one) shares.
extern CRITICAL_SECTION g_netProviderGlobalLock;   // stru_6FACFE48
extern int              g_netProviderInitCount;    // dword_6FACFE44

namespace Net
{

unsigned int NetProviderLOOP::Initialize(void* configStruct, int* outStatus)
{
    *outStatus = 0;

    const unsigned int kSuccess = 1;
    EnterCritSec(&g_netProviderGlobalLock);
    if (m_started)
    {
        LeaveCritSec(&g_netProviderGlobalLock);
        return 6;
    }

    if (g_netProviderInitCount++ == 0)
    {
        int initResult = OsNetInitializeAlias((int)&g_netProviderGlobalLock, kSuccess);
        if (!initResult)
        {
            --g_netProviderInitCount;
            LeaveCritSec(&g_netProviderGlobalLock);
            return initResult + 3;
        }
    }

    ((NetProviderLoopInitTarget*)((char*)this + 0x18))->CopyConfig(configStruct);
    m_started = kSuccess;
    LeaveCritSec(&g_netProviderGlobalLock);
    return kSuccess;
}

}  // namespace Net
