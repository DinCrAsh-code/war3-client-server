//============================================================================
//  0x6F285BC0 - CUnit's vtable slot 40 (+0xA0), SetFlagBit2.  See unit.h.
//
//  CWidget's own body at this slot (0x6F267120, agentwar3_flagbits.cpp) is
//  a plain set-or-clear of bit 1 of CAgentWar3::m_flags.  CUnit's clears
//  it the same way but then has to undo what the bit was gating: drop the
//  selection highlight through its own slot 100 with (0, 0) and run the
//  0x6F333310 teardown.  Setting it is still just the OR.
//
//  Same 0x6F285xxx module as slots 63, 97 and 98 (unit_playertable.cpp);
//  its own translation unit because it shares no callee with them.
//============================================================================
#include "unit.h"

//  0x6F333310 (`retn 0`) - CUnit's own, one step further down.
void CUnit::SetFlagBit2(int on)
{
    if (on != 0)
    {
        m_flags |= 2;
        return;
    }
    m_flags &= ~2u;
    RefreshSelectionHighlight(0, 0);
    OnSelectionBitCleared();
}
