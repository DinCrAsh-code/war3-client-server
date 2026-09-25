//============================================================================
//  A fourth terrain hash table from the same BFS frontier Misc/
//  citemclosure_terrain_gates.cpp and citemclosure_terrain_hashtrio.cpp
//  already document (CItem vtable slots 32/104, depth 11) - the same
//  generic Containers/tslist.inl, tsarray.inl and tshash.inl machinery
//  those two files already instantiate: a TSExplicitList<T>::UnlinkAll, the
//  TSGrowableArray<TSExplicitList<T>> bucket array's SetCount, and the
//  owning TSHashTable<T,K>::Rehash that calls both.  Address neighbourhood
//  0x6F72Cxxx-0x6F733xxx, distinct from the hashtrio's own
//  0x6F74Bxxx/0x6F75Bxxx/0x6F760xxx and from citemclosure_terrain_hashtable2
//  .cpp's 0x6F7B1xxx-0x6F7B2xxx, so a fourth, unrelated table rather than a
//  fourth name for one already named.
//
//  Nothing in this dump names the owning class or the element type - no
//  RTTI descriptor, no vtable stamp - so the node stays an incomplete
//  placeholder, same convention as the hashtrio's own
//  STerrainHashRecord0/1/2.  Rehash is the one member here that
//  dereferences a node (`ptr->m_hashval`, offset 0), and the constant `4`
//  it writes into the parked list's link offset is HashLinkOffset()
//  folding to `&((T*)0)->m_hashlink` - a plain node, no vtable.
//============================================================================
#include "storm.h"
#include "tslist.inl"
#include "tsarray.inl"
#include "tshash.inl"

struct STerrainHashRecord4
{
    unsigned int                 m_hashval;    // 0x00
    TSLink<STerrainHashRecord4>  m_hashlink;   // 0x04
};

//  K is read only through the template identity here - Rehash never calls
//  Compare() - so any policy the table could plausibly use works; left
//  forward-declared and unnamed, same convention as the hashtrio's own
//  SPlaceholderKey0/1/2.
struct SPlaceholderKey4;

//----------------------------------------------------------------------------
//  0x6F72C740 - the all-nodes list's UnlinkAll.
//----------------------------------------------------------------------------
template void TSExplicitList<STerrainHashRecord4>::UnlinkAll();

//----------------------------------------------------------------------------
//  0x6F732270 - the bucket array's SetCount, reached from Rehash below.
//----------------------------------------------------------------------------
template void
TSGrowableArray<TSExplicitList<STerrainHashRecord4> >::SetCount(unsigned int);

//----------------------------------------------------------------------------
//  0x6F7336A0 - the owning table's Rehash: parks every bucket's nodes on a
//  local list (whose own SetLinkOffset() call unconditionally drains it via
//  the UnlinkAll above, per Containers/tshash.inl's own Rehash comment),
//  resizes the bucket array through SetCount above, then relinks every
//  parked node into its new bucket.
//----------------------------------------------------------------------------
template void
TSHashTable<STerrainHashRecord4, SPlaceholderKey4>::Rehash(unsigned int);
