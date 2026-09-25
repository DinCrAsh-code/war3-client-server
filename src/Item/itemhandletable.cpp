//============================================================================
//  See itemhandletable.h.
//============================================================================
#include "itemhandletable.h"
#include "agent.h"

//----------------------------------------------------------------------------
//  0x6F2B5AE0 - "item" as a four-byte constant.
//----------------------------------------------------------------------------
unsigned int __fastcall GetItemTypeFourCC()
{
    return 0x6974656D;
}

//----------------------------------------------------------------------------
//  0x6F4266F0 - a second, still-unidentified FourCC constant.
//----------------------------------------------------------------------------
unsigned int __fastcall GetUnknownFourCC_6F4266F0()
{
    return 0x2B67616D;
}

//----------------------------------------------------------------------------
//  0x6F428050 - out-of-range (below 0x100000) returns null without
//  touching m_records at all; in range, reads the AGENT_SLOT-shaped
//  record at (index - 0x100000) and returns its middle field.
//----------------------------------------------------------------------------
CAgent* SItemSlotTableHolder::QueryItemSlot(int index)
{
    if ((unsigned int)index < 0x100000)
        return 0;

    return ((AGENT_SLOT*)m_records)[index - 0x100000].m_agent.m_ptr;
}

//----------------------------------------------------------------------------
//  0x6F4280B0 - the top of one JASS event-context stack.  `retn 4`.
//
//  Two things answer zero and both fall through to the same `xor eax,eax`:
//  an empty stack, and a zero sitting on top of a non-empty one.  Written
//  as "test the count, then the value, then return the value" rather than
//  as a null-coalescing expression, which is the order the shipped code
//  branches in.
//----------------------------------------------------------------------------
int SItemSlotTableHolder::PeekEventSlot(int index)
{
    TSGrowableArray<int>* stack = &m_eventSlots[index];

    unsigned int count = stack->m_count;
    if (count != 0)
    {
        int top = stack->m_data[count - 1];
        if (top != 0)
            return top;
    }

    return 0;
}
