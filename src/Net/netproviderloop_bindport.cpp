//============================================================================
//  Net::NetProviderLOOP - vtable slot 15, 0x6F6618C0.  Find the first free
//  port in [port, limit) under the +0x684 lock, register it with `target`,
//  and report which port was actually used (or 0 on failure).
//============================================================================
#include "netproviderloop.h"

//  critsec.cpp
void __fastcall EnterCritSec(CRITICAL_SECTION* cs);
void __fastcall LeaveCritSec(CRITICAL_SECTION* cs);

//  Storm.dll ordinal 401.
void* __stdcall SMemAlloc(unsigned int amount, const char* logfilename, int logline, unsigned int flags);

namespace Net
{

unsigned short NetProviderLOOP::BindPort(unsigned short port, unsigned short limit, int target)
{
    EnterCritSec(&m_portLock);

    unsigned short candidate = port;
    if (candidate < limit)
    {
        for (;;)
        {
            LoopbackPortEntry* node = m_ports.Tail();
            while (node && node->m_port != candidate)
                node = (LoopbackPortEntry*)((int)node->m_link.m_prevlink > 0
                                                 ? node->m_link.m_prevlink
                                                 : 0);

            if (!node)
            {
                //  The port is free - register it and stop scanning.
                LoopbackPortEntry* fresh = (LoopbackPortEntry*)SMemAlloc(
                    sizeof(LoopbackPortEntry), "AU_ListenNetProvider", -2, 8);
                if (fresh)
                {
                    fresh->m_link.m_next = 0;
                    fresh->m_link.m_prevlink = 0;
                }
                m_ports.LinkToHead(fresh);
                if (fresh)
                {
                    fresh->m_port   = candidate;
                    fresh->m_target = target;
                }
                break;
            }

            ++candidate;
            if (candidate >= limit)
                break;
        }
    }

    LeaveCritSec(&m_portLock);
    return candidate < limit ? candidate : 0;
}

}  // namespace Net
