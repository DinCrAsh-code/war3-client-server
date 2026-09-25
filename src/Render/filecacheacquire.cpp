//============================================================================
//  0x6F4C1400 - FindOrLoadPrefetchNode, waited on and reference-counted: the
//  node may still be loading on another thread when the table hands it back.
//============================================================================
#include "filecache.h"

PrefetchNode* __fastcall AcquirePrefetchNode(const char* name)
{
    int created;

    PrefetchNode* node = FindOrLoadPrefetchNode(name, &created, 0, 0);
    if (node != 0)
    {
        node->WaitReady();
        node->m_useCount += 1;
    }

    return node;
}
