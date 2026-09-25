//============================================================================
//  SJassThread's own out-of-line body, 0x6F448A80 - a separate module
//  neighbourhood from jassthreadstate.cpp (0x6F44B2F0's JassThreadSleep
//  inlines these same three field writes directly into the *running*
//  thread rather than calling this).  See jassthreadstate.h for the class
//  and its other four accessors, all of which fit in the header.
//============================================================================
#include "jassthreadstate.h"

//----------------------------------------------------------------------------
//  0x6F448A80 - `retn 8`, `this` in ecx.  Three plain stores.
//----------------------------------------------------------------------------
void SJassThread::Sleep(const CFloat* seconds, int kind)
{
    m_sleeping     = 1;
    m_sleepSeconds = *seconds;
    m_sleepKind    = kind;
}
