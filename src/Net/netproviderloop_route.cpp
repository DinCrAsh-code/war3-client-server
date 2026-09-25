//============================================================================
//  Net::NetProviderLOOP - vtable slot 14, 0x6F661840.  Pull the destination
//  port out of `addr`, look it up in the port list under +0x684's lock, and
//  hand the payload off to whatever is registered there.
//============================================================================
#include "osnetfamily.h"
#include "netproviderloop.h"
#include "storm.h"   // placement new

unsigned int __fastcall GetAddrIPAndPort(const void* addr, unsigned short* outPort);  // sockaddrinet.cpp

//  critsec.cpp
void __fastcall EnterCritSec(CRITICAL_SECTION* cs);
void __fastcall LeaveCritSec(CRITICAL_SECTION* cs);

//  lpAddend - the module's TCPMGR singleton (OsNet::g_pTcpManager,
//  ostcp.h), null before the module is started.
namespace OsNet { class TCPMGR; }
extern OsNet::TCPMGR* g_pTcpManager;

//  0x6F6E1E30 - allocate a fresh OsNet::LOOPCONNECT (osnetfamily.h - already
//  a real, vtable-carrying class), stamp its own field4/8/0xC and vtable,
//  fill in the caller's `arg4`/`payload`/`arg8` at +0x1C/+0x20/+0x10
//  (+0x1C/+0x20 are LOOPCONNECT's own named m_notifyProc/m_pending; +0x10
//  sits inside its own opaque m_reserved04 block, untouched by every other
//  LOOPCONNECT user so far), then - only if both `data` and `size` are
//  non-zero - SMemAlloc a `size`-byte copy and memcpy `data` into it,
//  storing the copy and its size at +0x14/+0x18 (both left zero on the
//  no-payload path).  Finally hands the finished record to
//  sub_6F6E1930(this, conn) - `this` is `g_pTcpManager`, a deeper
//  further-subsystem callee (retn 4, reaches SetEvent) out of this
//  session's own scope, left as a naked thunk.  `this`-only-plus-5-stack-
//  args (`retn 14h`, confirmed off the real call site's own push count) -
//  a thiscall member, not `__fastcall`, since none of the five real
//  arguments land in edx.
//
//  What is NOT reproduced: if the initial SMemAlloc fails, the shipped
//  code still unconditionally writes through the null `conn` it just got
//  (`xor esi,esi` then the same +0x1C/+0x20/+0x10 stores run regardless) -
//  reproduced literally below rather than "fixed", since CLAUDE.md's own
//  rule is to match the shipped behaviour, not improve on it.
class TcpMgrRouteDeliverer
{
public:
    void Deliver(int arg4, int payload, int arg8, const void* data, unsigned int size);
};

extern "C" void* memcpy(void*, const void*, unsigned int);
void* __stdcall SMemAlloc(unsigned int amount, const char* logfilename, int logline, unsigned int flags);

//  0x6F6E1930 - already noted above; declared here for extern linkage. The
//  call site is `this`=TCPMGR (TcpMgrRouteDeliverer::Deliver's own `this`),
//  one stack argument (the finished LOOPCONNECT*), `retn 4`.
class TcpMgrDeepPostThunk
{
public:
    void Post(void* conn);
};
__declspec(naked) void TcpMgrDeepPostThunk::Post(void*)
{
    __asm { mov eax, 0x6F6E1930 }
    __asm { jmp eax }
}

void TcpMgrRouteDeliverer::Deliver(int arg4, int payload, int arg8, const void* data, unsigned int size)
{
    void* raw = SMemAlloc(0x24, ".\\W32\\OsTcp.cpp", 0xDC8, 0);
    OsNet::LOOPCONNECT* conn;
    if (raw != 0)
    {
        *(int*)((char*)raw + 4) = -1;
        *(int*)((char*)raw + 8) = 0;
        *(int*)((char*)raw + 0xC) = 0;
        conn = new (raw) OsNet::LOOPCONNECT();
    }
    else
    {
        conn = 0;
    }

    char* rec = (char*)conn;
    *(int*)(rec + 0x1C) = arg4;
    *(int*)(rec + 0x20) = payload;
    *(int*)(rec + 0x10) = arg8;

    if (data != 0 && size != 0)
    {
        void* buf = SMemAlloc(size, ".\\W32\\OsTcp.cpp", 0xDCD, 0);
        *(void**)(rec + 0x14) = buf;
        memcpy(buf, data, size);
        *(unsigned int*)(rec + 0x18) = size;
    }
    else
    {
        *(void**)(rec + 0x14) = 0;
        *(unsigned int*)(rec + 0x18) = 0;
    }

    ((TcpMgrDeepPostThunk*)this)->Post(conn);
}

//  0x6F6E2A60 - `__fastcall(ecx,edx)` plus three stack args, `retn 0Ch`;
//  forwards straight into TcpMgrRouteDeliverer::Deliver when the module is
//  up, else a no-op.
__declspec(naked) void __fastcall RouteDeliver(int arg4, int payload, int arg8, const void* data, unsigned int size)
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
        push    edx
        push    ecx
        mov     ecx, eax
        call    TcpMgrRouteDeliverer::Deliver
        pop     esi
not_started:
        retn    0Ch
    }
}

namespace Net
{

void NetProviderLOOP::RouteToPort(const void* addr, int arg4, int arg8, const void* data, unsigned int size)
{
    unsigned short port;
    GetAddrIPAndPort(addr, &port);

    int payload = 0;
    EnterCritSec(&m_portLock);

    LoopbackPortEntry* node = m_ports.Tail();
    if (node)
    {
        for (;;)
        {
            if (node->m_port == port)
            {
                payload = node->m_target;
                break;
            }
            node = (LoopbackPortEntry*)node->m_link.m_prevlink;
            if ((int)node <= 0)
                break;
        }
    }

    LeaveCritSec(&m_portLock);

    RouteDeliver(arg4, payload, arg8, data, size);
}

}  // namespace Net
