//============================================================================
//  0x6F4FD700 / 0x6F501390 / 0x6F501050 - two of FindModelRecord's own
//  lookup registries (dword_6FAB82B8, unk_6FAB8290) and the first's own
//  Insert.
//
//  Same TAllocatorHashTable<T, HASHKEY_STRI, N> shape Widget/sprresource.h
//  already carries for the three sprite-resource registries - node's
//  m_hashval@0, two TSLink<T> at +4/+0xC, m_key@+0x14, registry's vtable@0,
//  TSExplicitList<T> at +4, m_probes@+0x10, TSGrowableArray<T> at +0x14,
//  m_mask@+0x24 - confirmed the same way sprresource.h's own header comment
//  confirms it: both functions below read the bucket array through
//  [this+0x1C] (TSGrowableArray's own m_data, +0x08 into the +0x14 array)
//  and the mask through [this+0x24], exactly the offsets
//  SSprAnimListRegistry::Find already established.
//
//  Find here is a second Ptr() overload the template carries alongside the
//  single-key one sprresource.h reconstructs: this one takes an
//  already-computed hash (retn 8, two explicit stack pushes at every call
//  site in FindModelRecord's own body) instead of hashing the key itself,
//  so there is no Storm_590 call in either body - the one instruction
//  difference from SSprAnimListRegistry::Find's own shape.
//
//  Two distinct C++ types and not one shared base for the two registries:
//  the shipped binary carries sub_6F4FD700 and sub_6F501390 as two separate
//  function bodies at two separate addresses even though their instructions
//  are identical, which only happens when they are two separate template
//  instantiations - see sprresource.h's own header comment on the same
//  fact for its three registries. Nothing in this closure establishes
//  either registry's real C++ name or what the second (unk_6FAB8290) is
//  a fallback *for*; named positionally.
//
//  m_pModel@+0x18 on the node matches Widget/modelacquire.cpp's own
//  SModelRecord exactly, which is what FindModelRecord's caller
//  (AcquireModelHandle) reads back out of the node this returns.
//============================================================================
#include "storm.h"
#include "allochashtable.h"

struct SModelRecordNode
{
    unsigned int              m_hashval;   // +0x00
    TSLink<SModelRecordNode>  m_hashlink;  // +0x04
    TSLink<SModelRecordNode>  m_listlink;  // +0x0C
    const char*               m_key;       // +0x14
    void*                     m_pModel;    // +0x18 - modelacquire.cpp's SModelRecord::m_pModel
};

struct SModelRecordRegistry
{
    //  0x6F4FD700 - `retn 8`.
    SModelRecordNode* Find(unsigned int hash, const void* key);

    //  0x6F500D10 - NewNode: create the node, pick its bucket (below this
    //  dump's depth cut - it in turn reaches this registry's own
    //  Initialize/CheckGrow/bucket-pick, none of which is reached from
    //  anywhere in this closure) and splice it into the table.  Same
    //  reasoning as Widget/sprresource.h's own LinkNode redirects.
    SModelRecordNode* NewNode(unsigned int hash, void* p1, void* p2);

    //  0x6F501050 - `retn 10h`.  NewNode does the linking; the rest is the
    //  same m_key replace guard as Widget/sprresource.h's own Insert.
    SModelRecordNode* Insert(unsigned int hash, const void* key,
                              void* p1, void* p2);

    void*                                             m_vtable;  // +0x00
    TSExplicitList<SModelRecordNode>                  m_list;    // +0x04
    unsigned int                                      m_probes;  // +0x10
    TSGrowableArray<TSExplicitList<SModelRecordNode> > m_buckets; // +0x14
    unsigned int                                      m_mask;    // +0x24
};

struct SModelRecordAliasRegistry
{
    //  0x6F501390 - identical shape and identical body to
    //  SModelRecordRegistry::Find; see there.
    SModelRecordNode* Find(unsigned int hash, const void* key);

    void*                                             m_vtable;
    TSExplicitList<SModelRecordNode>                  m_list;
    unsigned int                                      m_probes;
    TSGrowableArray<TSExplicitList<SModelRecordNode> > m_buckets;
    unsigned int                                      m_mask;
};

SModelRecordNode* SModelRecordRegistry::Find(unsigned int hash, const void* key)
{
    if (m_mask == 0xFFFFFFFFu)
        return 0;

    int link = (int)m_buckets[hash & m_mask].Tail();
    while (link > 0)
    {
        SModelRecordNode* node = (SModelRecordNode*)link;

        if (node->m_hashval == hash
            && HASHKEY_STRI::Compare(node->m_key, (const char*)key) == 0)
            return node;

        link = (int)m_buckets[hash & m_mask].Prev(node);
    }

    return 0;
}

SModelRecordNode* SModelRecordAliasRegistry::Find(unsigned int hash, const void* key)
{
    if (m_mask == 0xFFFFFFFFu)
        return 0;

    int link = (int)m_buckets[hash & m_mask].Tail();
    while (link > 0)
    {
        SModelRecordNode* node = (SModelRecordNode*)link;

        if (node->m_hashval == hash
            && HASHKEY_STRI::Compare(node->m_key, (const char*)key) == 0)
            return node;

        link = (int)m_buckets[hash & m_mask].Prev(node);
    }

    return 0;
}

SModelRecordNode* SModelRecordRegistry::Insert(unsigned int hash, const void* key,
                                                void* p1, void* p2)
{
    SModelRecordNode* node = NewNode(hash, p1, p2);

    node->m_hashval = hash;

    if (node->m_key != key)
    {
        if (node->m_key != 0)
            SMemFree((void*)node->m_key,
                     "e:\\Drive1\\temp\\buildwar3x\\Storm\\H\\stpl.h", 0xA38, 0);

        node->m_key = Storm_507((const char*)key,
                                "e:\\Drive1\\temp\\buildwar3x\\Storm\\H\\stpl.h", 0xA39);
    }

    return node;
}

#define ADDR_THUNK(sig, addr)                     \
    __declspec(naked) sig                         \
    {                                             \
        __asm { mov eax, addr }                   \
        __asm { jmp eax }                         \
    }

ADDR_THUNK(SModelRecordNode* SModelRecordRegistry::NewNode(unsigned int, void*, void*),
           0x6F500D10)

#undef ADDR_THUNK
