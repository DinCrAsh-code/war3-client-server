//============================================================================
//  Time synchronisation channel.
//============================================================================
#include "game.h"

//----------------------------------------------------------------------------
//  0x6F483270 - objects flagged as remote read the second channel.
//----------------------------------------------------------------------------
const CSyncChannel* CTimeSync::GetChannel(const CHandleObject* object) const
{
    return (object->m_flags & 0x80000000) == 0 ? &m_localChannel : &m_remoteChannel;
}

//----------------------------------------------------------------------------
//  0x6F483E70 - difference between two samples, corrected for the time the
//  two were taken at.  A difference small enough to be encoding noise is
//  snapped to zero before the correction is applied.
//----------------------------------------------------------------------------
CFloat CSyncChannel::Extrapolate(const CFloat& value, int time,
                                 const CFloat& base, int baseTime) const
{
    CFloat result = value - base;

    int magnitude = result.m_bits & 0x7FFFFFFF;
    if (*(float*)&magnitude < g_CFloatEpsilon)
        result = g_CFloatZero;

    if (time != baseTime)
        result = result + m_rate * CFloatFromInt(time - baseTime);

    return result;
}
