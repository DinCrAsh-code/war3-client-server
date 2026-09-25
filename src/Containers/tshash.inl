//============================================================================
//  TSHashTable's out-of-line members.  Included only by the translation units
//  that own an instantiation of them (tshashtable.cpp, tshashtype.cpp), never
//  by a caller: everything in here is a real call in the shipped code, and a
//  visible definition would let MSVC inline it.
//============================================================================
#include "storm.h"

//  Storm ordinals 590 (string hash) and 508 (bounded, case-sensitive
//  compare), for the string-keyed Ptr() below.  Declared here rather than by
//  including config.h or gamestrings.h: this file is included by the
//  translation units that own an instantiation, and dragging a subsystem
//  header into each of them would be the wrong dependency.  Both spellings
//  match those headers' exactly, which is what keeps the mangled names one
//  symbol (tools/link_check.py, cause 1).
unsigned int __stdcall Storm_590(const char* s);
int __stdcall Storm_508(const char* a, const char* b, unsigned int count);

//----------------------------------------------------------------------------
//  0x6F001EC0 (AGILE_TYPE_DATA) / 0x6F46E720 (AGILE_TYPE_RELATIONSHIP) - find
//  a node.  Walks the bucket from the back through m_prevlink, which is what
//  makes the end of the chain a sign test - hence `(int)ptr > 0` rather than
//  `ptr != 0`: Prev() hands back the field as it stands, and the negative
//  value it ends on is the terminator's complemented address, not null.
//
//  The bucket expression is written out again in the step because the walk
//  needs the list's link offset; the shipped code reloads m_buckets.m_data
//  and m_mask there, where this build hoists both (nothing in the loop
//  writes memory).
//----------------------------------------------------------------------------
template <class T, class K>
T* TSHashTable<T, K>::Ptr(unsigned int hashval, const K& key)
{
    if (m_mask == 0xFFFFFFFF)
        return 0;

    T* ptr = m_buckets[hashval & m_mask].Tail();
    while ((int)ptr > 0)
    {
        if (ptr->m_hashval == hashval && ptr->m_key == key)
            return ptr;
        ptr = m_buckets[hashval & m_mask].Prev(ptr);
    }
    return 0;
}

//----------------------------------------------------------------------------
//  0x6F42CCF0 - the handle-to-agent table's lookup.  Same walk as Ptr(),
//  above, but the shipped code never loads its second argument at all: the
//  match test is `ptr->m_hashval == hashval` alone, with no `&& ptr->m_key
//  == key`.  The one caller in this call tree (sub_6F430C80) still pushes a
//  key argument, and it goes unread there too, so the parameter is provably
//  dead on both sides - most likely `assert(ptr->m_key == key)` in place of
//  Ptr()'s `&&`, which this NDEBUG build compiles to nothing, but nothing in
//  this call tree can confirm that, so the assert is not spelled out here.
//----------------------------------------------------------------------------
template <class T, class K>
T* TSHashTable<T, K>::PtrHashOnly(unsigned int hashval, const K&)
{
    if (m_mask == 0xFFFFFFFF)
        return 0;

    T* ptr = m_buckets[hashval & m_mask].Tail();
    while ((int)ptr > 0)
    {
        if (ptr->m_hashval == hashval)
            return ptr;
        ptr = m_buckets[hashval & m_mask].Prev(ptr);
    }
    return 0;
}

//----------------------------------------------------------------------------
//  0x6F4700B0 - first use of the table: four buckets, and every one of them
//  told where the node's link sits.  The loop runs while i <= m_mask, not
//  while i < the count, and re-reads m_mask every time round.
//----------------------------------------------------------------------------
template <class T, class K>
void TSHashTable<T, K>::Initialize()
{
    m_mask = 3;
    m_buckets.SetCount(4);

    for (unsigned int i = 0; i <= m_mask; i++)
        m_buckets[i].SetLinkOffset(HashLinkOffset());
}

//----------------------------------------------------------------------------
//  0x6F470CC0 - decide whether the table has to grow before another node goes
//  into this bucket.  m_probes is a decaying run-length: three come off it on
//  every insert and one goes on per node walked, so a table only doubles once
//  chains stay long, not the first time one does.  Returns 1 when it rehashed,
//  because the caller's bucket index is stale after that.
//----------------------------------------------------------------------------
template <class T, class K>
int TSHashTable<T, K>::CheckGrow(unsigned int index)
{
    if (m_mask >= 0x1FFF)
        return 0;

    if (m_probes > 3)
        m_probes = m_probes - 3;
    else
        m_probes = 0;

    // `(int)ptr > 0` for the same reason as in Ptr(), above.
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
//  0x6F4701D0 - resize the bucket array to newcount and put every node back.
//
//  The nodes are parked on a local list first.  Its link offset is set with
//  SetLinkOffset() even though it was just constructed, and the compiler can
//  see that the poison value it is born with is not the offset being set, so
//  the test folds away and the UnlinkAll() inside SetLinkOffset() is called
//  unconditionally on an empty list - which is exactly what the shipped code
//  does, and why the call is there at all.
//----------------------------------------------------------------------------
template <class T, class K>
void TSHashTable<T, K>::Rehash(unsigned int newcount)
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
//  0x6F4710A0 - make a node for this hash value.  The node itself comes from
//  the derived class, which is also what links it into its bucket; all this
//  does is pick the bucket, grow the table if the chains have got long, and
//  put the new node at the head of the table-wide list.
//----------------------------------------------------------------------------
template <class T, class K>
T* TSHashTable<T, K>::NewNode(unsigned int hashval, void* arg1, void* arg2)
{
    if (m_mask == 0xFFFFFFFF)
        Initialize();

    unsigned int index = hashval & m_mask;
    if (CheckGrow(index))
        index = hashval & m_mask;

    T* ptr = AllocNode(&m_buckets[index], arg1, arg2);
    m_list.LinkToHead(ptr);
    return ptr;
}

//----------------------------------------------------------------------------
//  0x6F4713D0 - NewNode plus the two fields the table itself owns.
//----------------------------------------------------------------------------
template <class T, class K>
T* TSHashTable<T, K>::New(unsigned int hashval, const K& key,
                          void* arg1, void* arg2)
{
    T* ptr = NewNode(hashval, arg1, arg2);
    ptr->m_hashval = hashval;
    ptr->m_key = key;
    return ptr;
}

//----------------------------------------------------------------------------
//  0x6F367470/0x6F3675B0 - drop every node out of the table.  The all-nodes
//  list gets a real call to its own UnlinkAll() (m_list is a plain
//  TSExplicitList<T>, not tiny enough for /Ob2 to inline a second time);
//  each bucket's own walk is written out here instead of a nested
//  `m_buckets[i].UnlinkAll()` call, because the shipped stream has no call
//  for it - just TSLink<T>::Unlink()'s own small body, which /Ob2 inlines at
//  this call site the same way it does inside TSExplicitList<T>::UnlinkAll()
//  itself (Containers/tslist.inl).
//
//  The `eachInPlace == false` arm calls through this table's own vtable slot
//  0 - the destructor slot, since ~TSHashTable() is declared ahead of
//  AllocNode() - passing the node pointer that TailLink() just handed back
//  as its one stack argument.  Neither of this dump's two call sites (the
//  CommandReg/OrderTypeReg destructors, both `UnlinkAll(true)`) ever takes
//  this arm; it is transcribed from the bytes, not exercised.
//----------------------------------------------------------------------------
template <class T, class K>
void TSHashTable<T, K>::UnlinkAll(bool eachInPlace)
{
    typedef void (__thiscall *DestroySlot0Fn)(void*, void*);

    m_probes = 0;
    m_list.UnlinkAll();

    for (unsigned int i = 0; i < m_buckets.m_count; i++)
    {
        TSExplicitList<T>& bucket = m_buckets.m_data[i];
        int tail = bucket.TailLink();

        while (tail > 0)
        {
            if (eachInPlace)
            {
                TSLink<T>* link = (TSLink<T>*)(bucket.m_linkoffset + tail);
                link->Unlink();
            }
            else
            {
                DestroySlot0Fn slot0 = (DestroySlot0Fn)(*(void***)this)[0];
                slot0(this, (void*)tail);
            }

            tail = bucket.TailLink();
        }
    }
}
