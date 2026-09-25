//============================================================================
//  0x6F2F7170 - CGameUI::GetLastTrackedTarget: the top of the tracked-target
//  history stack, or null when it is empty.  `retn 0`.
//
//  Own translation unit, separate from PopTrackedTarget's own
//  (gameuitrackpop.cpp): the two calls for real in the shipped stream, and
//  this one is thirteen instructions - well inside what /Ob2 inlines into
//  its only caller if the two ever shared a TU.
//
//  The shipped body re-tests `m_trackHistory.m_count` a second time right
//  before indexing (`test eax,eax` / `jz`) even though the first,
//  identical test just above it already ruled out zero - dead code this
//  build's optimiser cannot prove unreachable from the source shape below
//  either, so it survives unreconciled on both sides.  Left exactly as it
//  reads rather than "cleaned up" into a single check, which would drop it
//  and lose the two instructions.
//============================================================================
#include "gameui.h"

STrackTarget* CGameUI::GetLastTrackedTarget()
{
    unsigned int count = m_trackHistory.m_count;
    if (count == 0)
        return 0;

    count = m_trackHistory.m_count;
    if (count == 0)
        return *(STrackTarget**)0;

    return (STrackTarget*)m_trackHistory.m_data[count - 1];
}
