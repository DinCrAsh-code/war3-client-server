//============================================================================
//  0x6F4C1A10 - a small pool-header constructor: clamp `elementSize` up to
//  4 and `capacity` up to 1, store both, and zero the three trailing
//  dwords.  Called from CAgentBase.cpp's own lazy-singleton pool getter
//  (Agent/cagentbaseabs_module_batch2.cpp's `AgentBase_GetOrCreatePool`,
//  0x6F480150, a *different* parallel session's own batch of this same
//  dump, which reaches this address by raw function-pointer cast rather
//  than a symbol call, exactly as that file's own header explains) with
//  `elementSize=0xBC`, `capacity=0x200` - a fixed-block pool sized for
//  0xBC-byte elements, 512 of them.  `this` doubles as the output pointer
//  (moved into `eax` immediately rather than kept in `ecx`, since nothing
//  about it is a real method receiver beyond "where to write").
//
//  A second, unrelated caller turned up in the cnd-ctor-agent-2 session
//  (CNetObserver::CNetObserver, Net/netobserver.cpp - `elementSize=0x1C`,
//  `capacity=8`), so the class itself moved to fixedpoolheader.h to keep
//  one definition rather than two ODR-conflicting locals.
//
//  Own translation unit: this address is far from the rest of this
//  session's own 0x6F4A5xxx-0x6F4A6xxx batch (0x6F4C1xxx), and nothing in
//  this call tree's own dump names a caller inside this batch's scope -
//  its one real caller (0x6F480150) belongs to the other session's file.
//============================================================================
#include "fixedpoolheader.h"

SFixedPoolHeader::SFixedPoolHeader(unsigned int elementSize, unsigned int capacity)
{
    if (elementSize < 4)
        elementSize = 4;
    m_elementSize = elementSize;

    if (capacity < 1)
        capacity = 1;
    m_capacity = capacity;

    m_reserved08 = 0;
    m_reserved0C = 0;
    m_reserved10 = 0;
}
