//============================================================================
//  0x6F26F900 - CUnit::FlushPendingRamp.
//
//  Cancel the third CAgentTimer (+0x180) and, if the flag word at +0x280
//  still says a controller ramp is pending, put it back and clear the bit.
//  Two callers reach it: CUnit::FinishCurrentOrder once it has torn the
//  task chain down, and 0x6F2840A0, which is a bare tail jump into it.
//
//  Own translation unit: both calls out of it are real.
//============================================================================
#include "unit.h"

void CUnit::FlushPendingRamp()
{
    m_timer180.Cancel();

    if ((m_flags280 & 0x40) != 0)
    {
        RestartPropertyRamp();
        m_flags280 &= 0xFFFFFFBF;
    }
}
