//============================================================================
//  0x6F49DFB0 - CPathClient::SetRoute: start a fresh order towards `pos`,
//  clearing both point caches first.
//============================================================================
#include "pathmove.h"

void CPathClient::SetRoute(const CGridVec2* pos, int haveGoal)
{
    ResetSearch(-1, 1, 0, 1);

    m_target = *pos;
    m_position = *pos;

    if (haveGoal)
        m_goal = *pos;
}
