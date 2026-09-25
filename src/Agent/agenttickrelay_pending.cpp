//============================================================================
//  0x6F4AA510 / 0x6F4AA530 - CAgentTickRelay::ReleasePendingA/B (see
//  agenttickrelay.h).  Own TU: Arm() and Tick()/Reload() reach these
//  through real `call` instructions in the target listing rather than
//  inlining them, despite being tiny - the shipped build compiled them
//  separately from their callers.
//============================================================================
#include "agenttickrelay.h"

void __thiscall CAgentTickRelay::ReleasePendingA()
{
    void* node = m_pendingA;
    if (node)
    {
        *(unsigned int*)((char*)node + 0x10) |= 0x10000;
        m_pendingA = 0;
    }
}

void __thiscall CAgentTickRelay::ReleasePendingB()
{
    void* node = m_pendingB;
    if (node)
    {
        *(unsigned int*)((char*)node + 0x10) |= 0x10000;
        m_pendingB = 0;
    }
}
