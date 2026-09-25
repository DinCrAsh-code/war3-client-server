//============================================================================
//  0x6F4A9320 - SHandleObjectRangeImpl::AdjustRate: settle the ramp, add
//  `delta` to the rate, snap it to exactly zero once it is within the
//  leftover epsilon of it (the same g_unk6FAAE61C floatnearlyequal.cpp
//  already tests against), and announce the change - always with
//  RampBase()/m_value as the payload (the rate's own new value is never
//  what gets announced) and no specific listener.
//
//  Own translation unit: it calls PrepareRangeUpdate and AnnounceRateChange
//  for real, and both are their own TUs already for the same reason
//  Unit/handleobjectrange.h's own file header gives.
//============================================================================
#include "handleobjectrange.h"

extern const float g_unk6FAAE61C;

void SHandleObjectRangeImpl::AdjustRate(const CFloat* delta)
{
    PrepareRangeUpdate();

    m_rate = m_rate + *delta;

    int magnitude = (m_rate - g_CFloatZero).m_bits & 0x7FFFFFFF;
    if (*(const float*)&magnitude < g_unk6FAAE61C)
        m_rate = g_CFloatZero;

    AnnounceRateChange(&m_value, 0);
}
