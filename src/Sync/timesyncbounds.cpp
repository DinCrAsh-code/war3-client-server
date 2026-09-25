#include "timesyncbounds.h"

//----------------------------------------------------------------------------
//  0x6F474250
//----------------------------------------------------------------------------
void SHandleWithType::FlushedOriginDelta(const CWorldVec3* pos, int flag)
{
    CPathTrace* trace = (CPathTrace*)LookupHandle(m_handle, m_typeTag);

    CFloat deltaY = pos->m_y - g_pGameData->m_originY;
    int by = deltaY.m_bits;
    int flushY = ((by - 0x3000000) ^ by) >> 31;
    deltaY = CFloat(~flushY & (by + 0xFD800000 /* see header: literal transliteration */));

    CFloat deltaX = pos->m_x - g_pGameData->m_originX;
    int bx = deltaX.m_bits;
    int flushX = ((bx - 0x3000000) ^ bx) >> 31;
    deltaX = CFloat(~flushX & (bx + 0xFD800000));

    CGridVec2 delta(deltaX, deltaY);
    trace->AddOriginDelta(&delta, flag);
}
