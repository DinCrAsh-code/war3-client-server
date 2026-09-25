//============================================================================
//  0x6F449BC0 - JassThreadLocal's own +0x48 script array (m_scripts),
//  indexed.  Own TU so jassthreadinstanceslot.cpp's JassThreadGetScript
//  (0x6F449CF0) calls it for real instead of the compiler inlining this
//  one-line body straight through, which is what happened the first draft
//  (0.706 vs the 1.000 a real call scores).
//============================================================================
#include "storm.h"
#include "jassthreadstate.h"

//  0x6F449BC0
void* JassThreadLocal::GetScript(unsigned int index)
{
    return ((void**)m_scripts)[index];
}
