//============================================================================
//  Net::NetProviderLOOP - vtable slots 2 (0x6F661060, Shutdown) and 3
//  (0x6F6610D0, NotifyStateChange), plus their shared direct callee
//  sub_6F660EA0 (0x6F660EA0, ClearShutdownPending) - one address
//  neighbourhood, the same module.  All three share the module-global lock
//  and reference count netproviderloop_initialize.cpp declares, and reach
//  the already-reconstructed Net::NetProvider::Method_0x2 (netprovider_
//  slot2.cpp) and the PostStatusTarget/OsNetShutdownAlias thunks that
//  unreconstructed_thunks.cpp / osnetaliases.cpp already carry.
//============================================================================
#include "netproviderloop.h"

//  netprovider_slot2.cpp - the base class's own vtable+0x90 "code 4"
//  cleanup, reached here as a plain (non-virtual, per this class family's
//  flat convention) member call on `this` reinterpreted as the base.
namespace Net { class NetProvider { public: int Method_0x2(); }; }

//  osnetaliases.cpp
void __fastcall OsNetShutdownAlias(unsigned int);

//  critsec.cpp
void __fastcall EnterCritSec(CRITICAL_SECTION* cs);
void __fastcall LeaveCritSec(CRITICAL_SECTION* cs);

//  unreconstructed_thunks.cpp - sub_6F65AAF0 (1-byte status write) and
//  sub_6F65AB10 (2-byte status write), both already thunked there.
class PostStatusTarget
{
public:
    void PostProviderStatus(int);
    void PostProviderStatusWord(int);
};

//  netproviderloop_initialize.cpp
extern CRITICAL_SECTION g_netProviderGlobalLock;   // stru_6FACFE48
extern int              g_netProviderInitCount;    // dword_6FACFE44

namespace Net
{

void NetProviderLOOP::NotifyStateChange(int)
{
    if (m_started)
    {
        ClearShutdownPending();
        m_shutdownPending = 1;
        ((PostStatusTarget*)this)->PostProviderStatus(1);
    }
    else
    {
        ((PostStatusTarget*)this)->PostProviderStatus(5);
    }
}

int NetProviderLOOP::Shutdown()
{
    ((Net::NetProvider*)this)->Method_0x2();
    ClearShutdownPending();

    EnterCritSec(&g_netProviderGlobalLock);
    if (!m_started)
    {
        LeaveCritSec(&g_netProviderGlobalLock);
        return 5;
    }

    if (--g_netProviderInitCount == 0)
        OsNetShutdownAlias(1);

    m_started = 0;
    LeaveCritSec(&g_netProviderGlobalLock);
    return 1;
}

}  // namespace Net
