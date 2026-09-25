//============================================================================
//  0x6F49ACA0 - has enough time passed to recompute this kind of path again?
//
//  One stamp per movement kind at m_stamp[kind], one interval per kind in
//  g_repathInterval[].  The first guard is the wrap-around fix-up: a stamp
//  ahead of `now` (which happens when the tick counter wraps, or when a
//  client is reused) is pulled back to exactly one interval ago so that the
//  age below stays sane rather than becoming enormous.
//
//  The age is always written out, whether or not the answer is yes - the
//  callers use it even when they are being told to wait.
//============================================================================
#include "pathmove.h"

int CPathClient::RateLimit(int kind, unsigned int now, unsigned int* ageOut)
{
    if (m_stamp[kind] > now)
        m_stamp[kind] = now - g_repathInterval[kind];

    unsigned int age = now - m_stamp[kind];
    *ageOut = age;
    if (age < g_repathInterval[kind])
        return 0;

    m_stamp[kind] = now;
    return 1;
}
