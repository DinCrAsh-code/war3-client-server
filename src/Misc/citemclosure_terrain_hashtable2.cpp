//============================================================================
//  A second, distinct string-keyed terrain hash table from the same BFS
//  frontier as Misc/citemclosure_terrain_gates.cpp and
//  citemclosure_terrain_hashtrio.cpp (address neighbourhood 0x6F7B1xxx-
//  0x6F7B2xxx, right by that file's own NTempest-namespaced siblings) -
//  three more generic instantiations: Initialize(), CheckGrow() and the
//  string-keyed Ptr(const char*) (case-insensitive: the walk's compare is
//  `call SStrnICmp`, HASHKEY_STRI).
//
//  No dump in this closure names the owning class or the element type, so
//  the node stays an incomplete placeholder as elsewhere in this file's
//  siblings.  Ptr() is the only member here that dereferences it: the hash
//  sits at offset 0 (`cmp [esi], ebx`) and the key pointer at offset 0x14
//  (`mov eax, [esi+14h]`) - eight bytes closer than
//  citemclosure_dbfieldhash.cpp's own field record, consistent with these
//  being two unrelated tables rather than the same one twice.
//============================================================================
#include "storm.h"
#include "allochashtable.h"
#include "tslist.inl"
#include "tsarray.inl"
#include "tshashstr.inl"

struct STerrainHashRecord3
{
    unsigned int                     m_hashval;       // 0x00
    TSLink<STerrainHashRecord3>      m_hashlink;       // 0x04
    char                             m_reserved0C[8];  // 0x0C, unread
    const char*                      m_key;            // 0x14
};

//  0x6F7B1D10 - Initialize().
template void TSHashTable<STerrainHashRecord3, HASHKEY_STRI>::Initialize();

//  0x6F7B2530 - CheckGrow(unsigned int index).
template int TSHashTable<STerrainHashRecord3, HASHKEY_STRI>::CheckGrow(unsigned int);

//  0x6F7B2750 - Ptr(const char* key).
template STerrainHashRecord3*
TSHashTable<STerrainHashRecord3, HASHKEY_STRI>::Ptr(const char*);

//  0x6F7B1150 - the bucket array's own SetCount, called from Rehash below
//  (`lea ecx,[this+0x14]; call`) to resize `m_buckets` before every parked
//  node is re-bucketed.  Containers/tsarray.inl's own SetCount.
template void TSGrowableArray<TSExplicitList<STerrainHashRecord3> >::SetCount(unsigned int);

//  0x6F7B1F40 - CheckGrow's own Rehash (0x6F7B2530 above already calls it):
//  park every node, resize `m_buckets` via SetCount above, then re-bucket
//  each parked node by `hashval & m_mask`.  Containers/tshash.inl's own
//  Rehash.
template void TSHashTable<STerrainHashRecord3, HASHKEY_STRI>::Rehash(unsigned int);
