//============================================================================
//  0x6F306B00 - CCameraWar3::StopAll: stop every camera field where it is.
//
//  Ten slots in the shipped order - 1, 7, 3, 4, 5, 2, 0, 6, 8, 9, which is
//  neither ascending nor the binder's own order and is transcribed rather
//  than tidied - then the pending-update timer.  Cancelling the modifiers
//  first is optional and is what the caller's flag selects.
//
//  Own translation unit: eleven real calls.
//============================================================================
#include "gameui.h"
#include "agenttimer.h"

void CCameraWar3::StopAll(int cancelModifiers)
{
    if (cancelModifiers != 0)
        CancelModifiers();

    StopSlot(1);
    StopSlot(7);
    StopSlot(3);
    StopSlot(4);
    StopSlot(5);
    StopSlot(2);
    StopSlot(0);
    StopSlot(6);
    StopSlot(8);
    StopSlot(9);

    m_updateTimer.Cancel();
}
