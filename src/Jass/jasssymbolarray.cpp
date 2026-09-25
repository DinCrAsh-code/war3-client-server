//============================================================================
//  JassScopeTable's own bucket array - TSGrowableArray<TSExplicitList<
//  JassSymbolNode> >'s three out-of-line members (Containers/tsarray.inl),
//  reached from Rehash's own m_buckets.SetCount() (jasssymbolhash.cpp):
//
//      0x6F456340  ComputeChunk(unsigned int) - 256/sizeof(TSExplicitList<T>)
//                  is 21 (0x15) regardless of T, the same as every other
//                  TSGrowableArray<TSExplicitList<*> > instantiation in
//                  this repo (e.g. Misc/citemclosure_dbfieldhash.cpp).
//      0x6F457930  SetAlloc(unsigned int) - the Storm_405/401/403
//                  realloc-or-alloc-and-move.
//      0x6F457B60  SetCount(unsigned int)
//============================================================================
//  The dump's own label for the Storm allocator's __FILE__ slot at this
//  instantiation's SetAlloc (0x6F457930) is `aAvTsexplicitli_108`,
//  consistent with the other TSGrowableArray<TSExplicitList<T> >
//  instantiations in this repo but for a *global*, unqualified `struct T`
//  rather than a nested `AGILE_TYPE_DATA::AGILE_TYPE_RELATIONSHIP` one, so
//  the tag differs from tsarray.inl's own default only in the type name.
#define TSARRAY_TYPENAME ".?AV?$TSExplicitList@UJassSymbolNode@@$0?CCCCCCCD@@@"

#include "jassscopetable.h"
#include "tslist.inl"
#include "tsarray.inl"

template unsigned int
TSGrowableArray<TSExplicitList<JassSymbolNode> >::ComputeChunk(unsigned int);

template void
TSGrowableArray<TSExplicitList<JassSymbolNode> >::SetAlloc(unsigned int);

template void
TSGrowableArray<TSExplicitList<JassSymbolNode> >::SetCount(unsigned int);
