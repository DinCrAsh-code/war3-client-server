//============================================================================
//  0x6F4A9940 - NIpse::CPoReThresholdLis::CompareAgainstThreshold: is
//  candidate point `a` or `b` past this class's own m_threshold, with a
//  small dead-zone (g_driveTolerance, Agent/agentrelation.h) around it so
//  the result does not chatter right at the boundary.
//
//  Reached with `this` at the identical +0x50/+0x54 storage
//  NIpse::CDrivenRelation::AtTarget (agentrelationattarget.cpp) already
//  compares as m_target/m_mode - both are the same "|value - point| <
//  tolerance" idiom AtTarget itself establishes, just run twice (once per
//  candidate) and then gated on m_mode (0 = rising edge, non-zero =
//  falling edge) plus this class's own base CAgentRelation::m_registration
//  (+0x1C) to decide whether a point that is already inside the dead zone
//  still counts as "past".
//============================================================================
#include "cporethresholdlis.h"
#include "agentrelation.h"

namespace NIpse {

int CPoReThresholdLis::CompareAgainstThreshold(const CFloat* a, const CFloat* b)
{
    int magB = (*b - m_threshold).m_bits & 0x7FFFFFFF;
    int nearB;
    if (*(const float*)&magB < g_driveTolerance)
        nearB = 1;
    else
        nearB = 0;

    int magA = (*a - m_threshold).m_bits & 0x7FFFFFFF;
    int nearA;
    if (*(const float*)&magA < g_driveTolerance)
        nearA = 1;
    else
        nearA = 0;

    if (m_mode == 0)
    {
        if (!nearB)
        {
            if (!(*(const float*)&b->m_bits > *(const float*)&m_threshold.m_bits))
                return 0;
        }
        else if (((CAgentRelation*)this)->m_registration == 0)
        {
            return 0;
        }

        if (nearA)
            return 1;

        if (*(const float*)&a->m_bits < *(const float*)&m_threshold.m_bits)
            return nearA + 1;   // nearA == 0 here - the shipped body's own "ecx+1" shape
        return 0;
    }

    if (!nearB)
    {
        if (!(*(const float*)&b->m_bits < *(const float*)&m_threshold.m_bits))
            return 0;
    }
    else if (((CAgentRelation*)this)->m_registration == 0)
    {
        return 0;
    }

    if (nearA)
        return 1;

    if (*(const float*)&a->m_bits > *(const float*)&m_threshold.m_bits)
        return 1;
    return 0;
}

}  // namespace NIpse
