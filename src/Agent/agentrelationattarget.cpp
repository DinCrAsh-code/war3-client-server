//============================================================================
//  0x6F4A98F0 - is the driven value close enough to its target to stop?
//
//  |value - target| against a global tolerance, with the answer narrowed to
//  a byte and widened again - the same `mov eax,1` / `movzx eax,al` shape
//  CPathClient::CheckSlotDistance has, and for the same reason.
//
//  Its own translation unit: CDrivenRelation::Update calls it for real, and
//  next to it MSVC folds the whole body into the caller's own `fcompp`.
//============================================================================
#include "agentrelation.h"

int CDrivenRelation::AtTarget(const CFloat* value)
{
    int magnitude = (*value - m_target).m_bits & 0x7FFFFFFF;

    int arrived;
    if (*(const float*)&magnitude < g_driveTolerance)
        arrived = 1;
    else
        arrived = 0;

    return (unsigned char)arrived;
}
