//============================================================================
//  0x6F4BEA10 - if the cache is at or over its entry limit, drop the least
//  recently linked node.  The table-wide list is kept most-recent-first (both
//  splices are LinkToHead), so its *tail* is the oldest entry.
//============================================================================
#include "filecache.h"

int TrimPrefetchCache()
{
    if (g_prefetchCount < g_prefetchLimit)
        return 0;

    DropPrefetchNode(g_prefetchTable.m_list.Tail());
    return 1;
}
