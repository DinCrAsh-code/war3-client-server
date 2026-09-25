//============================================================================
//  0x6F0118C0 - CRowRangeTable::FindMax.
//============================================================================
#include "rowrangetable.h"

int CRowRangeTable::FindMax(int x, int y, int* out) const
{
    int best = -1;
    int found = 0;

    for (unsigned int i = 0; i < m_count; i++)
    {
        const SRowRange& r = m_ranges[i];
        int xLo = r.m_xLo;
        int yLo = r.m_yLo;
        int yHi = r.m_yHi;
        int xHi = r.m_xHi;

        if (x < xLo)
            continue;
        if (x > xHi)
            continue;
        if (y < yLo)
            continue;
        if (y > yHi)
            continue;

        int value = r.m_data[(xHi - xLo + 1) * (y - yLo) - xLo + x];
        if (value > best)
            best = value;
        found = 1;
    }

    if (found && out)
        *out = best;
    return found;
}
