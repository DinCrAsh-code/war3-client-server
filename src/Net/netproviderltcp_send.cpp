//============================================================================
//  Net::NetProviderLTCP - vtable slots 4 (0x6F662E10, FlushPendingSend) and
//  5 (0x6F661A20, CancelSend) - one address neighbourhood, the same module,
//  both `retn 0` (no stack arguments - everything either reads comes from
//  `this` alone).  Both take the +0x680 lock, branch on whether the
//  listener thread has a live handle (+0x69C), post a status code, and
//  forward into the real delivery/cancel pipeline; FlushPendingSend also
//  sets the recycle flag (+0x6A8) and CancelSend clears it -
//  StopListenerThread's own header comment already documents that field.
//============================================================================
#include "netproviderltcp.h"

//  critsec.cpp
void __fastcall EnterCritSec(CRITICAL_SECTION* cs);
void __fastcall LeaveCritSec(CRITICAL_SECTION* cs);

//  unreconstructed_thunks.cpp - the same PostStatusTarget family
//  ContainsW3GSBytePacketCheck's own dump already reaches.
class PostStatusTarget
{
public:
    void PostProviderStatusDword(int);
    void ResetRecycleState();
};

//  0x6F65AB30 - real body in its own TU (netproviderltcp_sendstatus.cpp) so
//  this build's own /Ob2 cannot inline it into FlushPendingSend below the
//  way it does when both live in the same TU - see that file's own header.
//  `this` is passed in `ecx` per the thiscall declaration but the shipped
//  body never reads it.
class SendStatusTarget
{
public:
    void PostSendStatus(int code);
};

//  0x6F662780 - the real send pipeline: builds a CDataStoreCache-backed
//  packet from the (data, aux, 0) triple and posts it through the
//  listener's own handle.  A deep, below-scope forward (CDataStoreCache
//  construction, htons, Storm_465, the same class of machinery
//  netproviderltcp_route.cpp's own LtcpRouteDeliver thunk already documents) -
//  a thunk to the real, unhooked body.
struct PendingSendArgs
{
    void* data;
    int   aux;
    int   zero;
};
__declspec(naked) void __fastcall SendViaListener(void* listenHandle, PendingSendArgs* args, int zero)
{
    __asm { mov eax, 0x6F662780 }
    __asm { jmp eax }
}

namespace Net
{

void NetProviderLTCP::FlushPendingSend()
{
    EnterCritSec(&m_lock);
    if (m_listenHandle)
    {
        ((SendStatusTarget*)this)->PostSendStatus(1);
        m_recycleFlag = 1;

        PendingSendArgs args = { m_pendingSendData, m_pendingSendAux, 0 };
        SendViaListener(m_listenHandle, &args, 0);
    }
    else
    {
        ((SendStatusTarget*)this)->PostSendStatus(0xE);
    }
    LeaveCritSec(&m_lock);
}

void NetProviderLTCP::CancelSend()
{
    EnterCritSec(&m_lock);
    if (m_listenHandle)
    {
        m_recycleFlag = 0;
        ((PostStatusTarget*)this)->ResetRecycleState();
        ((PostStatusTarget*)this)->PostProviderStatusDword(1);
    }
    else
    {
        ((PostStatusTarget*)this)->PostProviderStatusDword(0xE);
    }
    LeaveCritSec(&m_lock);
}

}  // namespace Net
