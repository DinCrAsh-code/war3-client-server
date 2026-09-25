//============================================================================
//  0x6F49B8E0 - CPathClient::StageSearch: latch the shared search globals
//  from this client onto the singleton before a search runs.
//============================================================================
#include "pathmove.h"

void CPathClient::StageSearch(CPathTrace* mover)
{
    g_pathQuality = ClassifySlopeAxis(&m_radius);
    g_pPathSearch = PathSearchSingleton();
    g_pPathGrid = PathSearchSingleton()->m_grid;
    g_pPathMover = mover;
}
