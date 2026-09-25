//============================================================================
//  0x6F62BBD0 - CEventTimer::CEventTimer.  Six instructions: the reference
//  count, the vtable and one more word.
//
//  The JASS_PauseGame slice reached this same address from the fade timers
//  and named it SFadeChannel::Reset (GameUI/fadetimer.h); the vtable this
//  body stamps - ??_7CEventTimer@@6B@ - says what the object really is, and
//  the two readings are left to be reconciled rather than one silently
//  rewriting the other.
//============================================================================
#include "frame.h"
#include "framethunks.h"


CEventTimer::CEventTimer()
{
    m_refcount = 0;
    m_14 = 0;
}
