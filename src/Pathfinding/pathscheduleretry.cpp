//============================================================================
//  0x6F49B340 - CPathClient::ScheduleRetry: close enough to `to`, with a
//  still-fresh target registration, gets a short fixed retry cooldown;
//  everything else gets a randomized one.
//============================================================================
#include "pathmove.h"
#include "hashstate.h"

static inline float AsHardwareFloat(CFloat value) { return *(const float*)&value; }

extern int __stdcall RandomInt(int range, SHashState* state);

void CPathClient::ScheduleRetry(const CGridVec2* to)
{
    CFloat distSq = CFloatDistanceSquared(m_target, *to);

    int isFar;
    if (AsHardwareFloat(g_pathCloseEnough) > AsHardwareFloat(distSq))
        isFar = 0;
    else
        isFar = 1;

    CGridRegistration* target = g_pPathMover->ResolveTargetHandle();

    if (!isFar && (unsigned int)target->m_visitStamp <= 1)
    {
        m_retries = 2;
        return;
    }

    //  The shipped code stores this alongside an unread `8` (see the write-up
    //  in tools/funcmap.py's BEHAVIOUR entry) rather than folding `+6+1`
    //  into `+7` the way this build's optimiser does from a bare literal.
    int bounds[2] = { 6, 8 };
    m_retries = RandomInt(2, (SHashState*)g_pTimeSync) + bounds[0] + 1;
}
