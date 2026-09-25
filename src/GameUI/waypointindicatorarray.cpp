//============================================================================
//  0x6F399060 - SWaypointIndicatorArray::SetCount.
//
//  The third instance in this repo of one growable-array body: the same
//  shape as SAgentTimerArray::SetCount (0x6F2FD240,
//  GameUI/agenttimerarraycount.cpp) and as the eight-slot array's own
//  (0x6F39A620), differing only in the element type.  It is *not* Storm's
//  TSGrowableArray::SetCount (Containers/tsarray.inl) - there is no chunk
//  rounding, no shrink-side destruction and no separate grow test:
//
//    * the count is already right - do nothing at all;
//    * the new count is zero - destroy the array through its own destructor
//      and clear all three words, so it goes back to looking freshly
//      constructed;
//    * otherwise resize to exactly `count`, up or down, and construct every
//      element from the old count to the new one.  A shrink makes that
//      loop's bounds cross, so it runs zero times.
//
//  Its one call site is the world frame's constructor asking for 256
//  waypoint indicators (0x6F39AE0F), which is the same 256 x 0x1C the
//  CreateWaypointIndicators loop then walks
//  (GameUI/worldframewaypoints.cpp).
//
//  The null test in front of the element constructor is placement new's, and
//  it is written out here rather than spelled as placement new because the
//  element's constructor is not reconstructed: a redirect cannot be a
//  constructor (MSVC rejects __declspec(naked) on one), so it is declared as
//  an ordinary `Construct` member and the test goes in by hand.
//
//  The two stores of 0 and -1 into the frame's own unwind-state slot, and
//  the prologue, epilogue and funclet round them, are the
//  __except_handler4 frame MSVC builds for a loop that constructs objects -
//  there is no __try in the source - and this repo's fixed /GS- /EHs-c-
//  emits none of it.  That is 30 of the target's 78 instructions.
//============================================================================
#include "gameui.h"

//----------------------------------------------------------------------------
//  The array header.  CWorldFrameWar3 embeds one, and its +0x08 is the
//  m_pWaypoints gameui.h already names.
//----------------------------------------------------------------------------
struct SWaypointIndicatorArray
{
    //  0x6F38EDB0 - destroy every live element and free the block.
    //  `retn 0`; still a redirect.  Spelled as a member and not as
    //  ~SWaypointIndicatorArray for the same reason Construct below is.
    void Destruct();
    //  0x6F396420 - resize the block to exactly `alloc` elements.
    //  `retn 4`; still a redirect.
    void SetAlloc(unsigned int alloc);
    //  0x6F399060 - this file.
    void SetCount(unsigned int count);

    unsigned int         m_alloc;   // +0x00
    unsigned int         m_count;   // +0x04
    SWaypointIndicator*  m_data;    // +0x08
};

//  0x6F38FA30 - one indicator's constructor.  `retn 0`; still a redirect.
void __fastcall ConstructWaypointIndicator(SWaypointIndicator* self);

void SWaypointIndicatorArray::SetCount(unsigned int count)
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
        SWaypointIndicator* element = &m_data[i];
        if (element != 0)
            ConstructWaypointIndicator(element);
    }

    m_count = count;
}
