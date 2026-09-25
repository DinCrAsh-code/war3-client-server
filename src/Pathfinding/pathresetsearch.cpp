//============================================================================
//  0x6F49DBE0 - CPathClient::ResetSearch: clear one or both point caches
//  and their cursors, and optionally the search state, the formation lane,
//  and the "moved"/"searched" flag pair.
//
//  `keep` is the last cache slot to leave alone: -1 clears both (0 and 1),
//  otherwise everything from `keep` onward is cleared (in practice `keep`
//  is 0 or 1, since there are only two slots).
//============================================================================
#include "pathmove.h"

void CPathClient::ResetSearch(int keepCache, int reserve, int release,
                              int reset)
{
    unsigned int start = (keepCache == -1) ? 0 : (unsigned int)keepCache;
    unsigned int limit = (keepCache == -1) ? 2 : (unsigned int)keepCache + 1;

    m_flags &= ~kPathFlagValid;

    for (unsigned int i = start; i < limit; i++)
    {
        if (m_cache[i].m_count != 0)
            m_cache[i].EraseRange(0, m_cache[i].m_count);
        m_cursor[i] = (unsigned int)-1;
    }

    if (reserve != 0)
        ClearSearchState();
    if (release != 0)
        ReleaseLane();
    if (reset != 0)
        m_flags &= ~0x30000000;
}
