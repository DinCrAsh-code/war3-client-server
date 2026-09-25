//============================================================================
//  0x6F49DE90 - CPathClient::FinishSearch: the flood's landing spot has to
//  survive one more snap to a real cell; past that, retries are what
//  decide whether this order is done or scheduled again.
//============================================================================
#include "pathmove.h"

int CPathClient::FinishSearch(const CGridVec2* to)
{
    if (SnapToCell(to, 0))
        return 3;

    if (m_retries == 0)
        ScheduleRetry(to);

    if (m_retries == 1)
        return 4;

    ResetSearch(0, 0, 0, 0);
    m_retries -= 1;
    return 1;
}
