//============================================================================
//  0x6F4C1300 - find the cached file for a name, or load it.
//
//  A hit is moved back to the head of both of the table's lists - Unlink
//  then SetKey with the node's *own* key string, which is what makes SetKey's
//  `m_key != key` guard load-bearing: passing the node its own key means
//  the re-hash and the two splices happen and the duplication does not.
//
//  **The shipped build takes its name in `edi` and its `created` flag in
//  `eax`.**  Neither is ever written before it is read, both call sites load
//  exactly those registers, and no MSVC calling convention reaches either -
//  see docs/msvc-vc8-idioms.md, "Arguments in eax and esi".  The parameters
//  are declared in the order the shipped code reads them, so `ecx`/`edx`
//  carry them here and the one caller pays two `mov`s.
//============================================================================
#include "filecache.h"

static const char kStormTemplateHeader[] =
    "e:\\Drive1\\temp\\buildwar3x\\Storm\\H\\stpl.h";

PrefetchNode* __fastcall FindOrLoadPrefetchNode(const char* name, int* created,
                                                void* arg0, void* arg4)
{
    if (created != 0)
        *created = 0;

    PrefetchNode* node = g_prefetchTable.Ptr(name);
    if (node != 0)
    {
        g_prefetchTable.Unlink(node);
        g_prefetchTable.SetKey(node, node->m_key);

        if (created != 0)
            *created = 1;
    }
    else
    {
        TrimPrefetchCache();

        unsigned int hash = Storm_590(name);
        node = g_prefetchTable.NewNode(hash, 0, 0);
        node->m_hashval = hash;

        //  SetKey's tail, written out: NewNode has already put the node in
        //  the right bucket, so only the key copy is left.
        if (node->m_key != name)
        {
            if (node->m_key != 0)
                SMemFree((void*)node->m_key, kStormTemplateHeader, 0xA38, 0);

            node->m_key = Storm_507(name, kStormTemplateHeader, 0xA39);
        }

        node->m_stamp = g_prefetchStamp;
        node->m_arg0 = arg0;
        node->m_arg4 = arg4;

        if (Storm_279(name, &node->m_data, (int*)&node->m_size, 1,
                      &node->m_loaded) == 0)
        {
            g_prefetchTable.Delete(node);
            return 0;
        }

        unsigned int loaded = node->m_size;
        g_prefetchCount += 1;
        g_prefetchBytes += loaded;
        node->m_useCount = 0;
    }

    return node;
}
