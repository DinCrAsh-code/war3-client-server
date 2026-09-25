//============================================================================
//  0x6F33C600 - CTimeCover::CTimeCover: the clickable cover over the
//  time-of-day dial, and the two tooltips it carries.
//
//  The class name is the shipped one: the body stamps
//  ??_7CTimeCover@@6B@ at +0x00 and ??_7CTimeCover@@6B@_0 at +0x0B4, which
//  is the two-vtable shape of a CFrame subclass (CLayer at 0, CLayoutFrame
//  at 0xB4) and settles both the name and the base.
//
//  Its own state is three cleared words and two game strings fetched
//  straight into fixed buffers - 0x200 bytes for "TIME_OF_DAY_TOOLTIP" and
//  0x400 for "TIME_OF_DAY_UBERTIP".  The two sizes are the shipped `push`es
//  and are what fixes both buffers' extents.
//
//  Score capped by the same __except_handler4 frame CFrame::CFrame carries
//  (framecore.cpp): the `push -1`, the scope table, the cookie load and
//  `xor esp`, the unwind-state store, the appended funclet that destroys the
//  half-built CFrame base and the `jmp __CxxFrameHandler3` tail are all
//  things this repo's fixed /GS- /EHs-c- cannot emit.
//============================================================================
#include "timeofdayindicator.h"
#include "gamestrings.h"

CTimeCover::CTimeCover(void* parent)
    : CFrame((int)parent, 0, 0)
{
    m_field168 = 0;
    m_field16C = 0;
    m_field170 = 0;

    GetGameString("TIME_OF_DAY_TOOLTIP", m_toolTip, sizeof(m_toolTip));
    GetGameString("TIME_OF_DAY_UBERTIP", m_uberTip, sizeof(m_uberTip));
}
