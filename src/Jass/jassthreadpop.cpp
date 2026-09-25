//============================================================================
//  0x6F449CE0 - decrement the running JASS thread's own live-pushed count.
//  Own translation unit: 0x6F449xxx is a separate module neighbourhood from
//  jassthreadstate.cpp's 0x6F44B2xx (JassThreadSleep/JassThreadSyncStart),
//  even though all three reach the identical GetThreadLocalSlot(kThreadLocalJass)
//  pattern - see jassthreadstate.h for m_threadCount's own field note.
//============================================================================
#include "jassthreadstate.h"
#include "gamecontext.h"    // GetThreadLocalSlot - 0x6F4C34D0

//----------------------------------------------------------------------------
//  0x6F449CE0 - `retn 0`, no arguments at all (not even __fastcall's ecx).
//----------------------------------------------------------------------------
void JassThreadPop()
{
    JassThreadLocal* jass =
        (JassThreadLocal*)GetThreadLocalSlot(kThreadLocalJass);

    jass->m_threadCount--;
}
