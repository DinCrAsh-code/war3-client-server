//============================================================================
//  TAllocatorHashTable's out-of-line members.  Included only by the
//  translation units that own an instantiation, never by a caller:
//  everything in here is a real call in the shipped code and a visible
//  definition would let MSVC inline it.
//
//  The bodies are TSHashTable's (Containers/tshash.inl) wherever the shipped
//  ones are - Initialize, CheckGrow, Rehash and NewNode are the same
//  algorithm at four more addresses - with two differences that belong to
//  the string key: Ptr() hashes the key itself instead of being handed a
//  hash value, and the node's own key is a `const char*` compared through
//  the K policy rather than an object compared with `operator==`.
//============================================================================
#include "allochashtable.h"

//  The __FILE__ Storm's own template header passes: SetKey's two allocator
//  calls tag the key string with it, at lines 0xA38 and 0xA39.
#ifndef ALLOCHASH_HEADER_TAG
#define ALLOCHASH_HEADER_TAG "e:\\Drive1\\temp\\buildwar3x\\Storm\\H\\stpl.h"
#endif
static const char kStormHashHeader[] = ALLOCHASH_HEADER_TAG;

//  Which vtable slot does what.  Only two of them are reached from this
//  closure and neither is declared `virtual` here - see allochashtable.h.
//  Slot 1 allocates a node into a bucket, slot 0 hands one back.

//----------------------------------------------------------------------------
//  0x6F4BF040 / 0x6F4BF0D0 / 0x6F5C8ED0 - find the node for a key.
//
//  Enter on the *masked* Tail(), step on the raw Prev() and test its sign -
//  the shape every TSExplicitList walk in this binary has - and compare the
//  cheap hash before paying for the string.  The bucket expression is
//  written out again in the step because the walk needs the list's link
//  offset; the shipped code reloads m_buckets.m_data and m_mask there where
//  this build hoists both, nothing in the loop writing memory.
//----------------------------------------------------------------------------
template <class T, class K, int BLOCKSIZE>
T* TAllocatorHashTable<T, K, BLOCKSIZE>::Ptr(const char* key)
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
//  0x6F45ADA0 / 0x6F45AE80 - Ptr() with a pointer-equality fast path spliced
//  in ahead of the string compare.  The rest of the walk is identical.
//----------------------------------------------------------------------------
template <class T, class K, int BLOCKSIZE>
T* TAllocatorHashTable<T, K, BLOCKSIZE>::PtrInterned(const char* key)
{
    if (m_mask == 0xFFFFFFFFu)
        return 0;

    unsigned int hash = Storm_590(key);

    int link = (int)m_buckets[hash & m_mask].Tail();
    while (link > 0)
    {
        T* node = (T*)link;

        if (node->m_hashval == hash &&
            (node->m_key == key || K::Compare(node->m_key, key) == 0))
            return node;

        link = (int)m_buckets[hash & m_mask].Prev(node);
    }

    return 0;
}

//----------------------------------------------------------------------------
//  0x6F45B000 - PtrHashOnly: the same walk with the key argument dropped.
//----------------------------------------------------------------------------
template <class T, class K, int BLOCKSIZE>
T* TAllocatorHashTable<T, K, BLOCKSIZE>::PtrHashOnly(unsigned int hashval,
                                                     const K&)
{
    if (m_mask == 0xFFFFFFFFu)
        return 0;

    int link = (int)m_buckets[hashval & m_mask].Tail();
    while (link > 0)
    {
        T* node = (T*)link;

        if (node->m_hashval == hashval)
            return node;

        link = (int)m_buckets[hashval & m_mask].Prev(node);
    }

    return 0;
}

//----------------------------------------------------------------------------
//  0x6F4C01F0 / 0x6F4C0250 - first use of the table: four buckets, and every
//  one of them told where the node's bucket link sits.  The loop runs while
//  i <= m_mask, not while i < the count, and re-reads m_mask every time.
//----------------------------------------------------------------------------
template <class T, class K, int BLOCKSIZE>
void TAllocatorHashTable<T, K, BLOCKSIZE>::Initialize()
{
    m_mask = 3;
    m_buckets.SetCount(4);

    for (unsigned int i = 0; i <= m_mask; i++)
        m_buckets[i].SetLinkOffset(HashLinkOffset());
}

//----------------------------------------------------------------------------
//  0x6F4C0C30 / 0x6F4C0CD0 - decide whether the table has to grow before
//  another node goes into this bucket.  m_probes is a decaying run-length:
//  three come off it on every insert and one goes on per node walked, so a
//  table only doubles once chains stay long.  Returns 1 when it rehashed,
//  because the caller's bucket index is stale after that.
//----------------------------------------------------------------------------
template <class T, class K, int BLOCKSIZE>
int TAllocatorHashTable<T, K, BLOCKSIZE>::CheckGrow(unsigned int index)
{
    if (m_mask >= 0x1FFF)
        return 0;

    if (m_probes > 3)
        m_probes = m_probes - 3;
    else
        m_probes = 0;

    //  `(int)ptr > 0` for the same reason as in Ptr(), above.
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
//  0x6F4C02B0 / 0x6F4C0480 - resize the bucket array to newcount and put
//  every node back.
//
//  The nodes are parked on a local list first.  Its link offset is set with
//  SetLinkOffset() even though it was just constructed, and the compiler can
//  see that the poison value it is born with is not the offset being set, so
//  the test folds away and the UnlinkAll() inside SetLinkOffset() is called
//  unconditionally on an empty list - which is what the shipped code does.
//----------------------------------------------------------------------------
template <class T, class K, int BLOCKSIZE>
void TAllocatorHashTable<T, K, BLOCKSIZE>::Rehash(unsigned int newcount)
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
//  0x6F4C0EF0 / 0x6F4C0F90 - make a node for this hash value.  The node
//  itself comes from vtable slot 1, which is also what links it into its
//  bucket; all this does is pick the bucket, grow the table if the chains
//  have got long, and put the new node at the head of the table-wide list.
//----------------------------------------------------------------------------
template <class T, class K, int BLOCKSIZE>
T* TAllocatorHashTable<T, K, BLOCKSIZE>::NewNode(unsigned int hashval,
                                                 void* arg1, void* arg2)
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
//  0x6F4C0E10 - NewNode's splice for a node that already exists: pick the
//  bucket the (new) hash value names, grow if the chains have got long, and
//  put the node at the head of that bucket and of the table-wide list.  Both
//  splices unlink first, which is what makes this a *move* and not a second
//  insertion.
//----------------------------------------------------------------------------
template <class T, class K, int BLOCKSIZE>
void TAllocatorHashTable<T, K, BLOCKSIZE>::LinkNode(T* ptr,
                                                    unsigned int hashval)
{
    if (m_mask == 0xFFFFFFFFu)
        Initialize();

    unsigned int index = hashval & m_mask;
    if (CheckGrow(index))
        index = hashval & m_mask;

    m_buckets[index].LinkToHead(ptr);
    m_list.LinkToHead(ptr);
}

//----------------------------------------------------------------------------
//  0x6F4C1210 - give a node a new key: move it to the bucket that key hashes
//  to, stamp the hash, and take a private copy of the string.
//
//  The `m_key != key` guard is the shipped code's and it is not paranoia -
//  SetKey is called with the node's *own* key string on the re-use path
//  (0x6F4C1300), where freeing and re-duplicating it would be a use after
//  free.
//----------------------------------------------------------------------------
template <class T, class K, int BLOCKSIZE>
void TAllocatorHashTable<T, K, BLOCKSIZE>::SetKey(T* ptr, const char* key)
{
    unsigned int hash = Storm_590(key);

    LinkNode(ptr, hash);
    ptr->m_hashval = hash;

    if (ptr->m_key != key)
    {
        if (ptr->m_key != 0)
            SMemFree((void*)ptr->m_key, kStormHashHeader, 0xA38, 0);

        ptr->m_key = Storm_507(key, kStormHashHeader, 0xA39);
    }
}

//----------------------------------------------------------------------------
//  0x6F454510 / 0x6F4545D0 (Jass/jassdeclare.cpp's own
//  JassNameTable/JassVarTable instantiations) - NewNode a fresh node for a
//  hash the caller already computed, stamp it, and give it its own copy of
//  `key` the same way SetKey above does - except there is no existing node
//  to unlink first, so this skips straight to the free-then-strdup pair.
//----------------------------------------------------------------------------
template <class T, class K, int BLOCKSIZE>
T* TAllocatorHashTable<T, K, BLOCKSIZE>::NewNodeWithKey(unsigned int hash,
                                                        const char* key,
                                                        void* arg1,
                                                        void* arg2)
{
    T* node = NewNode(hash, arg1, arg2);
    node->m_hashval = hash;

    if (node->m_key != key)
    {
        if (node->m_key != 0)
            SMemFree((void*)node->m_key, kStormHashHeader, 0xA38, 0);

        node->m_key = Storm_507(key, kStormHashHeader, 0xA39);
    }

    return node;
}

//----------------------------------------------------------------------------
//  0x6F454820 / 0x6F4548B0 - hash `name` and forward into NewNodeWithKey
//  above.
//----------------------------------------------------------------------------
template <class T, class K, int BLOCKSIZE>
T* TAllocatorHashTable<T, K, BLOCKSIZE>::Declare(const char* name,
                                                 void* arg1, void* arg2)
{
    unsigned int hash = Storm_590(name);
    return NewNodeWithKey(hash, name, arg1, arg2);
}

//----------------------------------------------------------------------------
//  0x6F4BD640 / 0x6F4CCA70 - take a node out of both of its lists.
//
//  The guard is the *bucket* link's m_prevlink, not m_next: a node that has
//  never been linked has both at zero, and the two links are always spliced
//  and unspliced together, so one test covers both.
//----------------------------------------------------------------------------
template <class T, class K, int BLOCKSIZE>
void TAllocatorHashTable<T, K, BLOCKSIZE>::Unlink(T* ptr)
{
    //  Three spellings of the bucket link were measured against the shipped
    //  `lea eax,[edi+4]` / `push esi` / `mov esi,[eax]` - the field read
    //  directly, a named `TSLink<T>*` inside the guard, and the same pointer
    //  hoisted above it - and all three produce the identical
    //  `mov edx,[esi+4]` / `test` / `lea` here: MSVC propagates the address
    //  away and keeps the successor in a volatile register where the shipped
    //  build spends a second callee-saved one on it.  That one `push esi`
    //  displaces every instruction after it, which is most of this
    //  function's score.
    if (ptr->m_hashlink.m_prevlink != 0)
    {
        ptr->m_hashlink.Unlink();
        ptr->m_listlink.Unlink();
    }
}

//----------------------------------------------------------------------------
//  0x6F4BE220 - unlink a node and hand it back to vtable slot 0.
//
//  The answer is the node the table-wide walk would have continued from,
//  read *before* the unlink; the caller uses it to keep going.  `this` is
//  still in ecx at the Unlink call in the shipped code because nothing
//  between the entry and it writes ecx - MSVC drops the reload for the same
//  reason.
//----------------------------------------------------------------------------
template <class T, class K, int BLOCKSIZE>
T* TAllocatorHashTable<T, K, BLOCKSIZE>::Delete(T* ptr)
{
    int link = (int)m_list.Prev(ptr);
    T* prev = link > 0 ? (T*)link : 0;

    Unlink(ptr);

    typedef void (__thiscall *FreeNodeFn)(void*, T*);
    ((FreeNodeFn)(*(void***)this)[0])(this, ptr);

    return prev;
}
