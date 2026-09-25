//============================================================================
//  TSHashTable_P's out-of-line members - the same five as
//  Containers/allochash.inl plus the two the key *pool* adds.
//
//  Written out rather than shared with TAllocatorHashTable: the shipped
//  binary really does carry two templates here (0x6F861840 stamps
//  `??_7?$TSHashTable_P@UPrefetchListFile@@VHASHKEY_STRI_P@@@@6B@` where its
//  two neighbours stamp `TAllocatorHashTable`), and deriving one from the
//  other would put every shared member's body under the *base's* name in the
//  listing, which is not the name the shipped symbol has.
//============================================================================
#include "allochashtable.h"

//  The __FILE__ the key pool's own header passes Storm's allocator.
#ifndef HASHTABLEP_HEADER_TAG
#define HASHTABLEP_HEADER_TAG \
    "e:\\Drive1\\temp\\buildwar3x\\Storm\\H\\SAPIExtend.h"
#endif
static const char kHashTablePHeader[] = HASHTABLEP_HEADER_TAG;

//  How big one key-pool block is.  A constant of the shipped instantiation,
//  not a template argument: the four bytes at the front of each block are
//  the link to the next one, which is why the first string lands at +4.
enum { kHashTablePBlockSize = 0x2000 };

//----------------------------------------------------------------------------
//  0x6F4BF160 - find the node for a key.  See allochash.inl's Ptr().
//----------------------------------------------------------------------------
template <class T, class K>
T* TSHashTable_P<T, K>::Ptr(const char* key)
{
    if (m_mask == 0xFFFFFFFFu)
        return 0;

    unsigned int hash = Storm_590(key);

    int link = (int)m_buckets[hash & m_mask].Tail();
    while (link > 0)
    {
        T* node = (T*)link;

        if (node->m_hashval == hash && K::Compare(node->m_key, key) == 0)
            return node;

        link = (int)m_buckets[hash & m_mask].Prev(node);
    }

    return 0;
}

//----------------------------------------------------------------------------
//  0x6F4C0650.
//----------------------------------------------------------------------------
template <class T, class K>
void TSHashTable_P<T, K>::Initialize()
{
    m_mask = 3;
    m_buckets.SetCount(4);

    for (unsigned int i = 0; i <= m_mask; i++)
        m_buckets[i].SetLinkOffset(HashLinkOffset());
}

//----------------------------------------------------------------------------
//  0x6F4C0D70.
//----------------------------------------------------------------------------
template <class T, class K>
int TSHashTable_P<T, K>::CheckGrow(unsigned int index)
{
    if (m_mask >= 0x1FFF)
        return 0;

    if (m_probes > 3)
        m_probes = m_probes - 3;
    else
        m_probes = 0;

    for (T* ptr = m_buckets[index].Tail(); (int)ptr > 0;
         ptr = m_buckets[index].Prev(ptr))
    {
        if (++m_probes > 13)
        {
            m_probes = 0;
            Rehash(2 * m_mask + 2);
            return 1;
        }
    }
    return 0;
}

//----------------------------------------------------------------------------
//  0x6F4C06B0.
//----------------------------------------------------------------------------
template <class T, class K>
void TSHashTable_P<T, K>::Rehash(unsigned int newcount)
{
    unsigned int oldcount = m_mask + 1;

    TSExplicitList<T> parked;
    parked.SetLinkOffset(HashLinkOffset());

    for (unsigned int i = 0; i < oldcount; i++)
    {
        for (;;)
        {
            int tail = m_buckets[i].TailLink();
            if (tail <= 0)
                break;

            parked.LinkToHead((T*)tail);
        }
    }

    m_buckets.SetCount(newcount);

    for (unsigned int j = 0; j < newcount; j++)
        m_buckets[j].SetLinkOffset(HashLinkOffset());

    m_mask = newcount - 1;

    for (;;)
    {
        int tail = parked.TailLink();
        if (tail <= 0)
            break;

        T* ptr = (T*)tail;
        m_buckets[ptr->m_hashval & m_mask].LinkToHead(ptr);
    }
}

//----------------------------------------------------------------------------
//  0x6F4C1030.
//----------------------------------------------------------------------------
template <class T, class K>
T* TSHashTable_P<T, K>::NewNode(unsigned int hashval, void* arg1, void* arg2)
{
    if (m_mask == 0xFFFFFFFFu)
        Initialize();

    unsigned int index = hashval & m_mask;
    if (CheckGrow(index))
        index = hashval & m_mask;

    typedef T* (__thiscall *AllocNodeFn)(void*, TSExplicitList<T>*,
                                         void*, void*);
    T* ptr = ((AllocNodeFn)(*(void***)this)[1])(this, &m_buckets[index],
                                                arg1, arg2);
    m_list.LinkToHead(ptr);
    return ptr;
}

//----------------------------------------------------------------------------
//  0x6F4BCA30 - bump-allocate one string out of the key pool.
//
//  Blocks are 8 KB and threaded through their own first dword, so the first
//  string in a block lands at +4 and m_blockUsed starts there.  The
//  fill test is signed (`jl`) in the shipped code, which is what the cast
//  reproduces; the two allocation paths share the `m_currentBlock == 0` test
//  because a table that has never allocated one has to take it anyway.
//----------------------------------------------------------------------------
template <class T, class K>
char* TSHashTable_P<T, K>::AllocString(const char* text)
{
    unsigned int need = Storm_506(text) + 1;

    if (m_currentBlock == 0
        || (int)(m_blockUsed + need) >= kHashTablePBlockSize)
    {
        char* block = (char*)SMemAlloc(kHashTablePBlockSize,
                                       kHashTablePHeader, 0x143, 8);
        if (m_currentBlock == 0)
            m_firstBlock = block;
        else
            *(char**)m_currentBlock = block;

        m_currentBlock = block;
        *(char**)block = 0;
        m_blockUsed = 4;
    }

    unsigned int used = m_blockUsed;
    char* out = m_currentBlock + used;
    m_stringCount++;
    m_blockUsed = used + need;
    Storm_501(out, text, 0x7FFFFFFF);
    return out;
}

//----------------------------------------------------------------------------
//  0x6F4C1510 - NewNode with the key copied into the pool first.  The hash
//  is taken over the *pooled* copy, not over the caller's string.
//----------------------------------------------------------------------------
template <class T, class K>
T* TSHashTable_P<T, K>::New(const char* key, void* arg1, void* arg2)
{
    const char* pooled = AllocString(key);
    unsigned int hash = Storm_590(pooled);

    T* node = NewNode(hash, arg1, arg2);
    node->m_hashval = hash;
    node->m_key = pooled;
    return node;
}
