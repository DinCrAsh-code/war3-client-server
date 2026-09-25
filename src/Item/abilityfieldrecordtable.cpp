//============================================================================
//  g_abilityFieldRecordTable's own template instantiations - Ptr(),
//  Initialize(), Rehash() and the bucket array's SetCount()/UnlinkAll().
//  See abilityfieldrecordtable.h for the table/node shape.  Every function
//  here is tshash.inl's/tsarray.inl's/tslist.inl's own generic body,
//  reused verbatim; only the node type and the table singleton are new.
//
//  ability_typefieldrecord.cpp instantiates this same table's NewNode()/
//  CheckGrow()/SetAlloc()/ComputeChunk() (0x6F32B650/0x6F32A8B0 and their
//  own array-growth needs) and the real LookupAbilityFieldRecord() caller
//  - split the same way tsarray.cpp/tslist.cpp split
//  AGILE_TYPE_RELATIONSHIP's own instantiations across TUs, one per
//  address-owning session, not duplicated.
//
//  Own translation unit, the same reason every other TSHashTable<T,K>
//  instantiation gets one.
//============================================================================
#include "abilityfieldrecordtable.h"
#include "tshash.inl"
#include "tsarray.inl"
#include "tslist.inl"

//  0x6F320A40 - Ptr().
template SAbilityFieldRecordNode*
TSHashTable<SAbilityFieldRecordNode, AGILE_TYPE_ID>::Ptr(unsigned int,
                                                         const AGILE_TYPE_ID&);
//  0x6F327DF0 - Initialize(): first use of the table, four buckets,
//  m_mask=3.
template void TSHashTable<SAbilityFieldRecordNode, AGILE_TYPE_ID>::Initialize();
//  0x6F328200 - Rehash().
template void TSHashTable<SAbilityFieldRecordNode, AGILE_TYPE_ID>::Rehash(unsigned int);
//  0x6F3235E0 - the bucket array's own SetCount, reached out of Initialize()
//  and Rehash() alike.
template void TSGrowableArray<SAbilityFieldRecordBucket>::SetCount(unsigned int);
//  0x6F3136E0 - one bucket's own UnlinkAll (SetLinkOffset's, and Rehash's
//  parked list's).
template void TSExplicitList<SAbilityFieldRecordNode>::UnlinkAll();
