//============================================================================
//  A fifth terrain hash table from the same BFS frontier Misc/
//  citemclosure_terrain_gates.cpp, citemclosure_terrain_hashtrio.cpp and
//  citemclosure_terrainhash4.cpp already document (CItem vtable slots
//  32/104, depth 11) - the same generic Containers/tslist.inl, tsarray.inl
//  and tshash.inl machinery those files already instantiate.
//
//  Address neighbourhood 0x6F7AFxxx-0x6F7B1xxx overlaps
//  citemclosure_terrain_hashtable2.cpp's own STerrainHashRecord3 table
//  (0x6F7B1xxx-0x6F7B2xxx), which first looked like the same table this
//  file's own UnlinkAll/ComputeChunk/SetCount/Rehash quartet belonged to -
//  same generic shape, same address block. The call graph says otherwise:
//  that file's own CheckGrow (0x6F7B2530) calls Rehash at 0x6F7B1F40, which
//  in turn calls SetCount at 0x6F7B1150 - a distinct pair from this file's
//  own Rehash (0x6F7B1D70, calling SetCount at 0x6F7B1060) and from this
//  file's own UnlinkAll's callees, so two genuinely separate tables sharing
//  a neighbourhood and a generic body, not one table reached twice. Kept a
//  distinct placeholder type (STerrainHashRecord5, the next free number
//  after citemclosure_terrainhash4.cpp's own STerrainHashRecord4) and a
//  distinct key policy (Rehash never calls Compare(), so nothing here
//  confirms HASHKEY_STRI the way citemclosure_terrain_hashtable2.cpp's own
//  Ptr() does for its table).
//============================================================================
#include "storm.h"
#include "tslist.inl"
#include "tsarray.inl"
#include "tshash.inl"

struct STerrainHashRecord5
{
    unsigned int                 m_hashval;    // 0x00
    TSLink<STerrainHashRecord5>  m_hashlink;   // 0x04
};

//  K is read only through the template identity here - Rehash never calls
//  Compare() - so any policy the table could plausibly use works; left
//  forward-declared and unnamed, same convention as
//  citemclosure_terrainhash4.cpp's own SPlaceholderKey4.
struct SPlaceholderKey5;

//----------------------------------------------------------------------------
//  0x6F7AFA40 - the all-nodes list's UnlinkAll.
//----------------------------------------------------------------------------
template void TSExplicitList<STerrainHashRecord5>::UnlinkAll();

//----------------------------------------------------------------------------
//  0x6F7AF020 - the bucket array's ComputeChunk.  Each
//  TSExplicitList<STerrainHashRecord5> element is 12 bytes: a 4-byte
//  m_linkoffset plus an 8-byte TSLink<T> terminator, which is what makes
//  ComputeChunk's own `256 / sizeof(T)` ceiling 0x15 here - the same
//  constant Containers/computechunk21.cpp's own 12-byte instantiation
//  reaches, independently, for a different table.
//----------------------------------------------------------------------------
template unsigned int
TSGrowableArray<TSExplicitList<STerrainHashRecord5> >::ComputeChunk(unsigned int);

//----------------------------------------------------------------------------
//  0x6F7B1060 - the bucket array's SetCount, reached from Rehash below.
//----------------------------------------------------------------------------
template void
TSGrowableArray<TSExplicitList<STerrainHashRecord5> >::SetCount(unsigned int);

//----------------------------------------------------------------------------
//  0x6F7B1D70 - the owning table's Rehash: parks every bucket's nodes on a
//  local list (whose own SetLinkOffset() call unconditionally drains it via
//  the UnlinkAll above, per Containers/tshash.inl's own Rehash comment),
//  resizes the bucket array through SetCount above, then relinks every
//  parked node into its new bucket.
//----------------------------------------------------------------------------
template void
TSHashTable<STerrainHashRecord5, SPlaceholderKey5>::Rehash(unsigned int);
