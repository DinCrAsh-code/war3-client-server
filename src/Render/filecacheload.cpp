//============================================================================
//  0x6F4C1550 - the file cache's public entry.
//
//  Hand back a file's bytes: out of the cache when the name is cachable and
//  the cache is on, and straight through Storm's own loader (ordinal 279)
//  when it is not.  `takeOwnership` decides whether the caller gets the
//  node's own buffer - in which case the node loses it and is dropped - or a
//  fresh copy with a NUL appended.
//
//  The two `takeOwnership` tests really are two: the shipped code jumps from
//  the first straight past the second into the copy block, which is what an
//  `if (a && b)` followed by `if (!b)` produces once MSVC cross-jumps them.
//============================================================================
#include "filecache.h"

static const char kFileCacheFile[] = ".\\FileCache.cpp";

int __fastcall LoadCachedFile(const char* name, void** buffer, int* length,
                              int takeOwnership)
{
    *buffer = 0;
    if (length != 0)
        *length = 0;

    EnterCritSec(&g_fileCacheLock);

    int loaded = 0;

    if (g_uncachableTable.Ptr(name) == 0 && g_fileCacheEnabled != 0)
    {
        PrefetchNode* node = AcquirePrefetchNode(name);
        if (node != 0)
        {
            if (g_recordPrefetchList != 0 && takeOwnership != 0)
            {
                PrefetchListFile* record = g_prefetchListTable.Ptr(name);
                if (record == 0)
                    record = g_prefetchListTable.New(name, 0, 0);

                record->m_recorded = 1;
            }

            if (takeOwnership == 0)
            {
                void* copy = SMemAlloc(node->m_size + 1, kFileCacheFile,
                                       0x284, 0);
                *buffer = copy;
                memcpy(copy, node->m_data, node->m_size + 1);
            }

            if (length != 0)
                *length = node->m_size;

            DecField0x34_6F4BC660(node);

            if (takeOwnership != 0)
            {
                *buffer = node->m_data;
                node->m_data = 0;
                DropPrefetchNode(node);
            }

            loaded = 1;
        }
    }
    else
    {
        loaded = Storm_279(name, buffer, length, 1, 0);
    }

    LeaveCritSec(&g_fileCacheLock);
    return loaded;
}
