//============================================================================
//  gamemain-batch-3: a refcounted keepalive timer, one instance shared by
//  every caller that wants the blocking-window message pump to keep ticking
//  while it waits.  dword_6FAD13BC is the live-request count; uIDEvent
//  (0x6FAD13B8) is the SetTimer() id, 0 while no timer is armed.  TimerFunc
//  (0x6F6B7FA0) is the WM_TIMER callback itself - address-only here (its
//  own body is out of this session's batch), the same
//  `(FnPtrType)0x6F...` idiom src/GameUI/gameuicursorreset.cpp and friends
//  already use for a callback this call tree never has to execute.
//============================================================================
#include <windows.h>

extern unsigned int g_blockingWindowTimerRefs;   // dword_6FAD13BC
extern UINT_PTR      uIDEvent;                    // 0x6FAD13B8

//----------------------------------------------------------------------------
//  0x6F6B7FC0 - AcquireBlockingWindowTimer.  Bump the refcount; arm the
//  timer only on the 0->1 (well, "still zero") transition, i.e. only while
//  no timer id is already held.
//----------------------------------------------------------------------------
void AcquireBlockingWindowTimer()
{
    g_blockingWindowTimerRefs += 1;

    if (uIDEvent == 0)
        uIDEvent = SetTimer(0, 0, 0x32, (TIMERPROC)0x6F6B7FA0);
}

//----------------------------------------------------------------------------
//  0x6F6B7FF0 - ReleaseBlockingWindowTimer.  Drop the refcount, clamped at
//  zero (a signed decrement that goes negative is pinned back to 0 rather
//  than left negative), and kill the timer only once the count is back at
//  zero.
//----------------------------------------------------------------------------
void ReleaseBlockingWindowTimer()
{
    g_blockingWindowTimerRefs -= 1;

    if ((int)g_blockingWindowTimerRefs < 0)
        g_blockingWindowTimerRefs = 0;

    if (g_blockingWindowTimerRefs > 0)
        return;

    if (uIDEvent != 0)
    {
        KillTimer(0, uIDEvent);
        uIDEvent = 0;
    }
}
