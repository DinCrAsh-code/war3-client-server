//============================================================================
//  Three more NTempest/Storm terrain-rendering internals from the same BFS
//  frontier Misc/citemclosure_terrain_gates.cpp already documents (CItem
//  vtable slots 32/104, depth 8 this time) - three parallel instantiations
//  of the same generic machinery Containers/tslist.inl, tsarray.inl and
//  tshash.inl already carry for the relationship table and the font cache:
//  a TSExplicitList<T>::UnlinkAll, the TSGrowableArray<TSExplicitList<T>>
//  bucket array's SetCount, and the owning TSHashTable<T,K>::Rehash that
//  calls both.  The shipped link had no /OPT:ICF, so one template over
//  three element types is three sets of functions rather than one shared
//  body - the same fact GameUI/gameuifontcachelist.cpp's own comment
//  records for a fourth instantiation of the list half alone.
//
//  None of the three T's is named by anything in this closure - no RTTI
//  descriptor, no vtable stamp, no caller reachable from this dump names
//  the owning class - so, like Misc/citemclosure_terrain_gates.cpp's own
//  STerrainListNode0/1, each is an incomplete placeholder.  Rehash is the
//  one member here that actually dereferences a node (`ptr->m_hashval`),
//  and all three trees agree that field sits at offset 0 - the constant
//  `4` each Rehash writes into the parked list's link offset is
//  HashLinkOffset() folding to `&((T*)0)->m_hashlink`, so m_hashlink
//  follows immediately at +4, the "plain node, no vtable" case
//  Storm/storm.h's own HashLinkOffset() comment names.  Nothing else here
//  reads T, so nothing else needs to be named.
//============================================================================
#include "storm.h"
#include "tslist.inl"
#include "tsarray.inl"
#include "tshash.inl"

struct STerrainHashRecord0 { unsigned int m_hashval; TSLink<STerrainHashRecord0> m_hashlink; };
struct STerrainHashRecord1 { unsigned int m_hashval; TSLink<STerrainHashRecord1> m_hashlink; };
struct STerrainHashRecord2 { unsigned int m_hashval; TSLink<STerrainHashRecord2> m_hashlink; };

//  Storm's key policies read K only through the template identity - Rehash
//  never calls Compare() - so any policy the table could plausibly use
//  works; HASHKEY_STR is the one already declared for this codebase's other
//  string-keyed tables (Containers/allochashtable.h).
struct SPlaceholderKey0;
struct SPlaceholderKey1;
struct SPlaceholderKey2;

//----------------------------------------------------------------------------
//  0x6F74B1E0 / 0x6F75BE00 / 0x6F7604D0 - record 0's list, bucket array and
//  owning table.
//----------------------------------------------------------------------------
template void TSExplicitList<STerrainHashRecord0>::UnlinkAll();
template void TSGrowableArray<TSExplicitList<STerrainHashRecord0> >::SetCount(unsigned int);
template void TSHashTable<STerrainHashRecord0, SPlaceholderKey0>::Rehash(unsigned int);

//----------------------------------------------------------------------------
//  0x6F74B250 / 0x6F75BEF0 / 0x6F7606A0 - record 1's.
//----------------------------------------------------------------------------
template void TSExplicitList<STerrainHashRecord1>::UnlinkAll();
template void TSGrowableArray<TSExplicitList<STerrainHashRecord1> >::SetCount(unsigned int);
template void TSHashTable<STerrainHashRecord1, SPlaceholderKey1>::Rehash(unsigned int);

//----------------------------------------------------------------------------
//  0x6F74B2C0 / 0x6F75BFE0 / 0x6F760870 - record 2's.
//----------------------------------------------------------------------------
template void TSExplicitList<STerrainHashRecord2>::UnlinkAll();
template void TSGrowableArray<TSExplicitList<STerrainHashRecord2> >::SetCount(unsigned int);
template void TSHashTable<STerrainHashRecord2, SPlaceholderKey2>::Rehash(unsigned int);

//----------------------------------------------------------------------------
//  A second BFS pass over this same trio's own bucket-array growth path:
//  each record's ComputeChunk (called from SetAlloc below, and from that
//  record's own SetCount whenever growth needs a fresh chunk size) and
//  SetAlloc (the realloc-or-move-elements body Containers/tsarray.inl
//  declares).  A `TSExplicitList<T>` element is 12 bytes regardless of T
//  (its own two fields, `m_linkoffset` and `m_terminator`, carry no T-sized
//  payload), so all three ComputeChunk instantiations fold to the same
//  `maxchunk = 256 / 12 = 21` this file's own header quotes as
//  Containers/tsarray.inl's worked example - visible in the dump as the
//  literal `15h` every one of the three shares. Each SetAlloc's own Storm
//  tag (`"AvTsexplicitli_60"`/`"_61"`/`"_62"`) is this build's own mangled
//  suffix for "the Nth `TSExplicitList<T>` instantiation Storm has seen",
//  which is what ties each SetAlloc to its own record: record 0's carries
//  `_60`, record 1's `_61`, record 2's `_62`.
//----------------------------------------------------------------------------

//  0x6F73E740 / 0x6F754770 - record 0's.
template unsigned int TSGrowableArray<TSExplicitList<STerrainHashRecord0> >::ComputeChunk(unsigned int);
template void TSGrowableArray<TSExplicitList<STerrainHashRecord0> >::SetAlloc(unsigned int);

//  0x6F73E7A0 / 0x6F7548B0 - record 1's.
template unsigned int TSGrowableArray<TSExplicitList<STerrainHashRecord1> >::ComputeChunk(unsigned int);
template void TSGrowableArray<TSExplicitList<STerrainHashRecord1> >::SetAlloc(unsigned int);

//  0x6F73E800 / 0x6F7549F0 - record 2's.
template unsigned int TSGrowableArray<TSExplicitList<STerrainHashRecord2> >::ComputeChunk(unsigned int);
template void TSGrowableArray<TSExplicitList<STerrainHashRecord2> >::SetAlloc(unsigned int);
