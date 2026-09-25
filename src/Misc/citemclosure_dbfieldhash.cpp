//============================================================================
//  0x6F4C9D40 - a fourth `TSHashTable<T,K>::Ptr(const char*)` instantiation
//  (Containers/tshashstr.inl), string-keyed and case-sensitive (the walk's
//  compare is `call Storm_508`, HASHKEY_STR - see allochashtable.h for why
//  that ordinal means case-sensitive and 509 means insensitive).  Reached
//  from a "HDBFIELD"-tagged CDataAllocator::Alloc neighbour in this same
//  closure (sub_6F4C9C00/sub_6F4C9C90, out of this batch's scope) that
//  names the record a "field", distinct from the "HDBENTRY"-tagged,
//  HASHKEY_STRI-compared WEDBEntryHash table
//  Misc/citemclosure_wedbentryhash.cpp reconstructs - two different tables,
//  not two names for one.
//
//  The node's own two fields this walk actually touches are read straight
//  off the dump's own displacements: `cmp [esi], ebx` (the hash) is offset
//  0, `mov eax, [esi+1Ch]` (the key pointer) is offset 0x1C.  A third field
//  is pinned by a *different* caller: `sub_6F4CA0F0` below hands a found
//  record straight to `TRefCnt::AddRef` (0x6F4C40C0, Agent/refcnt.h), which
//  only ever touches `[this+4]`, so the record's own refcount has to sit
//  there - meaning its link-list membership is *not* at the usual +4 this
//  closure's plain, non-refcounted nodes use.
//
//  A later BFS depth (11, over the same CItem vtable slots 32/104
//  frontier) reaches this table's own Initialize() and CheckGrow(), and
//  both settle the one remaining question Ptr() left open: the constant
//  `0Ch` each writes as the bucket link's own offset (Initialize's
//  `SetLinkOffset` loop; CheckGrow never dereferences a node, so it adds no
//  evidence of its own but is the identical generic body regardless) is
//  HashLinkOffset() folding to `&((SDatabaseFieldRecord*)0)->m_hashlink` -
//  so the link sits at +0x0C, eight bytes into what used to be an
//  undifferentiated reserved block.  The remaining 4+8 unread bytes on
//  either side of it stay reserved.
//============================================================================
#include "storm.h"
#include "allochashtable.h"
#include "tslist.inl"
#include "tsarray.inl"
#include "tshashstr.inl"
#include "refcnt.h"

struct SDatabaseFieldRecord
{
    unsigned int                    m_hashval;      // 0x00
    unsigned int                    m_refcount;      // 0x04 - TRefCnt::AddRef's own field
    char                             m_reserved08[4]; // 0x08, unread
    TSLink<SDatabaseFieldRecord>    m_hashlink;      // 0x0C
    char                             m_reserved14[8]; // 0x14, unread
    const char*                      m_key;           // 0x1C
};

template SDatabaseFieldRecord*
TSHashTable<SDatabaseFieldRecord, HASHKEY_STR>::Ptr(const char*);

//----------------------------------------------------------------------------
//  0x6F4CA900 - Initialize(): four buckets, mask 3, every bucket told its
//  link offset (the `0Ch` above).
//----------------------------------------------------------------------------
template void TSHashTable<SDatabaseFieldRecord, HASHKEY_STR>::Initialize();

//----------------------------------------------------------------------------
//  0x6F4CACA0 - CheckGrow(unsigned int index): its own Rehash callee
//  (sub_6F4CA960) is out of this batch's scope, but CheckGrow itself never
//  dereferences a node, so the instantiation is exact regardless - same
//  reasoning as citemclosure_wedbentryhash.cpp's own CheckGrow.
//----------------------------------------------------------------------------
template int TSHashTable<SDatabaseFieldRecord, HASHKEY_STR>::CheckGrow(unsigned int);

//----------------------------------------------------------------------------
//  CItem-vtable-closure batch M (a fourth BFS depth over the same slots
//  32/104 frontier) closes CheckGrow's own Rehash callee above, plus the
//  three TSGrowableArray<TSExplicitList<T>> members Rehash's own
//  m_buckets.SetCount() reaches - the identical generic chain
//  citemclosure_terrain_hashtable2.cpp's own STerrainHashRecord3 bucket
//  array uses, just over this table's node type instead:
//
//      0x6F4CA960  Rehash(unsigned int)       - park every node, resize
//                  m_buckets via SetCount below, re-bucket by hashval&mask.
//                  Containers/tshash.inl's own Rehash.
//      0x6F4CA480  TSGrowableArray<...>::SetCount(unsigned int) - the
//                  bucket array's own SetCount; ComputeChunk/SetAlloc below
//                  are its own two callees when growing.
//      0x6F4C9EF0  TSGrowableArray<...>::SetAlloc(unsigned int) - the
//                  Storm_405/401/403 realloc-or-alloc-and-move.
//      0x6F4C8C60  TSGrowableArray<...>::ComputeChunk(unsigned int) -
//                  sizeof(TSExplicitList<T>) is 12 bytes regardless of T
//                  (the list head/tail/offset triple, never T itself), so
//                  this is the same 17-instruction body every other
//                  TSGrowableArray<TSExplicitList<*> > instantiation in
//                  this repo gets - 256/12 = 21 (0x15), same as this
//                  file's own siblings elsewhere in src/.
//
//  The bucket-array element is TSExplicitList<SDatabaseFieldRecord>, not
//  SDatabaseFieldRecord itself - the array holds one list head per bucket,
//  same shape as every other TSHashTable<T,K> in this repo.
//----------------------------------------------------------------------------
template unsigned int
TSGrowableArray<TSExplicitList<SDatabaseFieldRecord, -572662307> >::ComputeChunk(unsigned int);

template void
TSGrowableArray<TSExplicitList<SDatabaseFieldRecord, -572662307> >::SetAlloc(unsigned int);

template void
TSGrowableArray<TSExplicitList<SDatabaseFieldRecord, -572662307> >::SetCount(unsigned int);

template void TSHashTable<SDatabaseFieldRecord, HASHKEY_STR>::Rehash(unsigned int);

//----------------------------------------------------------------------------
//  0x6F4CA160 - `retn 0`, __fastcall(ecx=self, edx=key): does this object's
//  *first* record table (+0x10 - Misc/citemclosure_database_thunks.cpp's
//  own header names the owning family and why its constructors stay
//  redirects) contain `key`?  Boolified through the classic
//  `neg/sbb/neg` idiom rather than a `setne`.
//----------------------------------------------------------------------------
typedef TSHashTable<SDatabaseFieldRecord, HASHKEY_STR> SDatabaseRecordTable;

int __fastcall DatabaseRecordSet_ContainsFieldKey(void* self, const char* key)
{
    SDatabaseRecordTable* table =
        (SDatabaseRecordTable*)((char*)self + 0x10);
    return table->Ptr(key) != 0;
}

//----------------------------------------------------------------------------
//  0x6F4CA0F0 - `retn 0`, __thiscall(ecx=self) with the key in edx: look the
//  key up in this object's *second* record table (+0x38); on a hit, AddRef
//  it with the same "HDBENTRY" tag its own insert path
//  (Misc/citemclosure_database_thunks.cpp's `SDatabaseEntryTableThunk`)
//  uses and return it.  On a miss, forward to that same insert with -1 (a
//  sentinel this batch's own dumps never resolve further) and return
//  whatever it hands back - so a lookup that fails becomes an insert,
//  which is why this function's own name says "find or insert".
//----------------------------------------------------------------------------
struct SDatabaseEntryTableThunk { void* Insert(int key); };

SDatabaseFieldRecord* __fastcall
DatabaseRecordSet_FindOrInsertEntry(void* self, const char* key)
{
    SDatabaseRecordTable* table =
        (SDatabaseRecordTable*)((char*)self + 0x38);
    SDatabaseFieldRecord* found = table->Ptr(key);
    if (found)
        return (SDatabaseFieldRecord*)((TRefCnt*)found)->AddRef("HDBENTRY");

    return (SDatabaseFieldRecord*)
        ((SDatabaseEntryTableThunk*)self)->Insert(-1);
}
