//============================================================================
//  0x6F457630 - JassScopeTable::Release.  Empty a table: unlink its
//  table-wide list, then walk every bucket.  With `keepInBuckets` zero (the
//  shipped body's own stack argument), each bucket's head node is handed to
//  vtable slot 0 (the same free-a-node slot TAllocatorHashTable::Delete
//  uses) until the bucket is empty; nonzero just unlinks each bucket
//  without freeing anything, which is the shape a table being *reset*
//  rather than *destroyed* needs - the nodes stay alive, only their bucket
//  membership is dropped.
//
//  Reached through raw offsets rather than the shared allochash.inl
//  members: this is not one of TAllocatorHashTable<T,K,N>'s own members
//  every other instantiation in this repo shares, so it is declared on the
//  small JassScopeTable subclass (jassscopetable.h) rather than widening
//  Containers/allochashtable.h for all of them.
//============================================================================
#include "jassscopetable.h"
#include "tslist.inl"

typedef void (__thiscall *FreeSymbolNodeFn)(JassScopeTable*, JassSymbolNode*);

void JassScopeTable::Release(int keepInBuckets)
{
    m_list.UnlinkAll();
    m_probes = 0;

    unsigned int count = *(unsigned int*)((char*)this + 0x18);
    TSExplicitList<JassSymbolNode>* buckets =
        *(TSExplicitList<JassSymbolNode>**)((char*)this + 0x1C);

    for (unsigned int i = 0; i < count; i++)
    {
        TSExplicitList<JassSymbolNode>* bucket = &buckets[i];

        if (bucket->m_linkoffset <= 0)
            continue;

        if (!keepInBuckets)
        {
            for (int tail = bucket->TailLink(); tail > 0; tail = bucket->TailLink())
            {
                FreeSymbolNodeFn freeNode = (FreeSymbolNodeFn)(*(void***)this)[0];
                freeNode(this, (JassSymbolNode*)tail);
            }
        }
        else
        {
            bucket->UnlinkAll();
        }
    }
}
