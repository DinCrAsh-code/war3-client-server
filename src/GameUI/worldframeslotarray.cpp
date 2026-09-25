//============================================================================
//  0x6F39A620 - SWorldFrameSlotArray::SetCount.
//
//  The same growable-array body as SAgentTimerArray::SetCount (0x6F2FD240)
//  and SWaypointIndicatorArray::SetCount (0x6F399060), over a third element
//  type; see either of those files for the four cases and for why it is not
//  Storm's TSGrowableArray::SetCount.
//
//  What is different here is that the element's constructor is *inlined*
//  rather than called: the shipped loop writes -1 into the slot's first
//  word, constructs the CAgentPtr at +0x04 through its own out-of-line
//  constructor (0x6F02F780, Agent/agentregistry.cpp), zeroes the float at
//  +0x08 and the two words at +0x10 and +0x14, and leaves +0x0C alone.  So
//  the element is written here as a struct with an inline Construct rather
//  than as a redirect.
//
//  Its one call site is the world frame's constructor asking for eight
//  slots (0x6F39AE03).  What the eight are is not established anywhere in
//  this tree - the array is filled by code outside it - so the names say
//  what the constructor puts in each field and no more: an id that starts
//  at -1, an agent reference, a float and two words.
//
//  30 of the target's 82 instructions are the __except_handler4 frame this
//  repo's fixed /GS- /EHs-c- cannot emit; see the SetCount entry in
//  tools/funcmap.py's BEHAVIOUR table for the breakdown.
//============================================================================
#include "storm.h"
#include "agent.h"

//----------------------------------------------------------------------------
//  One slot.  0x18 bytes - the stride the shipped loop's `lea eax,[edi+edi*2]`
//  / `lea esi,[ecx+eax*8]` pair computes.
//----------------------------------------------------------------------------
struct SWorldFrameSlotArray;

struct SWorldFrameSlot
{
    int       m_id;         // +0x00  starts at -1, i.e. "no id"
    CAgentPtr m_agent;      // +0x04
    float     m_float08;    // +0x08
    int       m_field0C;    // +0x0C  the constructor does not touch it
    int       m_field10;    // +0x10
    int       m_field14;    // +0x14
};

struct SWorldFrameSlotArray
{
    //  0x6F396170 - destroy every live element and free the block.
    //  `retn 0`; still a redirect.
    void Destruct();
    //  0x6F3962E0 - resize the block to exactly `alloc` elements.
    //  `retn 4`; still a redirect.
    void SetAlloc(unsigned int alloc);
    //  0x6F39A620 - this file.
    void SetCount(unsigned int count);

    unsigned int     m_alloc;   // +0x00
    unsigned int     m_count;   // +0x04
    SWorldFrameSlot* m_data;    // +0x08
};

void SWorldFrameSlotArray::SetCount(unsigned int count)
{
    if (count == m_count)
        return;

    if (count == 0)
    {
        Destruct();
        m_alloc = 0;
        m_count = 0;
        m_data = 0;
        return;
    }

    SetAlloc(count);

    for (unsigned int i = m_count; i < count; i++)
    {
        //  The null test is placement new's own; the shipped code forms the
        //  element address, tests it and skips the whole construction.
        SWorldFrameSlot* slot = &m_data[i];
        if (slot != 0)
        {
            slot->m_id = -1;
            new (&slot->m_agent) CAgentPtr(0);
            slot->m_float08 = 0.0f;
            slot->m_field10 = 0;
            slot->m_field14 = 0;
        }
    }

    m_count = count;
}
