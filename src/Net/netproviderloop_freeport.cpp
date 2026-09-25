//============================================================================
//  Net::NetProviderLOOP - vtable slot 16, 0x6F6619B0, plus its direct
//  callee sub_6F6617D0 (0x6F6617D0, LoopbackPortList::RemoveAndFree,
//  netproviderloop.h) - unregister a port and free its node.
//============================================================================
#include "netproviderloop.h"

//  critsec.cpp
void __fastcall EnterCritSec(CRITICAL_SECTION* cs);
void __fastcall LeaveCritSec(CRITICAL_SECTION* cs);

//  Storm.dll ordinal 403.
void __stdcall SMemFree(void* ptr, const char* logfilename, int logline, unsigned int flags);

namespace Net
{

//  0x6F6617D0 - `list->Prev(node)` (masked the same way Tail()/TailLink()
//  are - storm.h's own comment on why the raw sign test survives rather
//  than going through Prev()'s own cast) is read *before* the node is
//  unlinked, then `Link(node)->Unlink()` (the same inlined TSLink::Unlink
//  body sub_6F661370/TSList<T,0>::UnlinkAll's own loop uses) and an
//  SMemFree.
int LoopbackPortList::RemoveAndFree(LoopbackPortEntry* node)
{
    int raw = (int)Prev(node);
    int result = raw > 0 ? raw : 0;

    Link(node)->Unlink();
    SMemFree(node, "AU_ListenNetProvider", -2, 0);

    return result;
}

void NetProviderLOOP::FreePort(unsigned short port)
{
    EnterCritSec(&m_portLock);

    LoopbackPortEntry* node = m_ports.Tail();
    if (node)
    {
        for (;;)
        {
            if (node->m_port == port)
            {
                m_ports.RemoveAndFree(node);
                break;
            }
            node = (LoopbackPortEntry*)node->m_link.m_prevlink;
            if ((int)node <= 0)
                break;
        }
    }

    LeaveCritSec(&m_portLock);
}

}  // namespace Net
