//============================================================================
//  0x6F2FB920 - CGameUI::PopTrackedTarget: go back to whatever was tracked
//  before.
//
//  Two halves.  The first re-tracks the top of the history stack - with
//  `recordPrevious` zero, so the target being replaced does not go straight
//  back onto the stack it is being popped off - and the second shortens the
//  stack by one.
//
//  The order is the shipped one and it is not the obvious one: the stack is
//  read (through GetLastTrackedTarget) and re-tracked *before* it is
//  shortened, so SetTrackedTarget still sees the entry it is restoring.  An
//  empty stack hands back a null target, which SetTrackedTarget treats as
//  "follow nothing".
//
//  The shortening is SPointerArray::SetCount written out - the shipped body
//  has the whole of it inline with no call between the copies - and it can
//  only ever shrink here, so the grow arm inside it is dead at this one call
//  site and is still emitted, exactly as the shipped stream has it.  The
//  saturating `count ? count - 1 : 0` is the shipped code's own: popping an
//  empty stack is not an underflow.
//============================================================================
#include "gameui.h"

void CGameUI::PopTrackedTarget()
{
    SetTrackedTarget(GetLastTrackedTarget(), 0);

    unsigned int depth = m_trackHistory.m_count;
    m_trackHistory.SetCount(depth != 0 ? depth - 1 : 0);
}
