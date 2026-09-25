//============================================================================
//  0x6F49B470 - CPathClient::SameCellRecently: has `pos` landed in the same
//  shifted cell as this client's own position, or - if not - did the last
//  two stamped events both happen within the last 10 ticks?
//============================================================================
#include "pathmove.h"

int CPathClient::SameCellRecently(const CGridVec2* pos, int shift, int* sameOut)
{
    int posY = CFloatToInt(CFloatFloor(m_position.m_y)) >> shift;
    int posX = CFloatToInt(CFloatFloor(m_position.m_x)) >> shift;
    int toY = CFloatToInt(CFloatFloor(pos->m_y)) >> shift;
    int toX = CFloatToInt(CFloatFloor(pos->m_x)) >> shift;

    if (posX == toX && posY == toY)
    {
        *sameOut = 1;
        return 0;
    }

    unsigned int tick = g_pTimeSync->m_tick;
    if (tick - m_stamp[0] < 10 || tick - m_stamp[1] < 10)
    {
        *sameOut = 0;
        return 1;
    }

    *sameOut = 1;
    return 1;
}
