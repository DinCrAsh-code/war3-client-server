//============================================================================
//  0x6F4C1490 - remember that this name is not worth caching, so the next
//  attempt goes straight to Storm's own loader.
//
//  The table gets the node and the key copy written out rather than through
//  SetKey: NewNode has already put the node in the bucket its hash names, so
//  only the duplication is left - which is why the shipped code has
//  0x6F4C0F90 here and not 0x6F4C1210's twin.
//============================================================================
#include "filecache.h"

static const char kStormTemplateHeader[] =
    "e:\\Drive1\\temp\\buildwar3x\\Storm\\H\\stpl.h";

void __fastcall MarkFileUncachable(const char* name)
{
    EnterCritSec(&g_fileCacheLock);

    if (g_uncachableTable.Ptr(name) == 0)
    {
        unsigned int hash = Storm_590(name);

        UncachableNode* node = g_uncachableTable.NewNode(hash, 0, 0);
        node->m_hashval = hash;

        if (node->m_key != name)
        {
            if (node->m_key != 0)
                SMemFree((void*)node->m_key, kStormTemplateHeader, 0xA38, 0);

            node->m_key = Storm_507(name, kStormTemplateHeader, 0xA39);
        }
    }

    LeaveCritSec(&g_fileCacheLock);
}
