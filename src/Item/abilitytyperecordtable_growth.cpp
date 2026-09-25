//============================================================================
//  abilitytyperecordtable.cpp's own missing link: its Initialize()/
//  CheckGrow() bodies call into the array/list growth chain
//  (tsarray.inl/tslist.inl), which that file never included, so nothing
//  ever explicitly instantiated these members for T=SAbilityTypeRecordNode.
//  Addresses read directly off this table's own dump
//  (asm/sub_6F052990_0x6F052990_calltree_asm.md):
//
//  - Initialize() (0x6F265270) calls `m_buckets.SetCount(4)` (0x6F264DE0)
//    then loops `m_buckets[i].SetLinkOffset(HashLinkOffset())` - compiled
//    to a bare `call sub_6F25EF60` with no argument, because
//    HashLinkOffset() folds to a compile-time constant here too (the same
//    fold tshash.inl's own Rehash() comment documents), leaving only the
//    UnlinkAll() SetLinkOffset() calls internally.
//  - CheckGrow() (0x6F265630) calls `Rehash(2*m_mask+2)` (0x6F2652D0) on
//    the grow path, which itself parks nodes on a local list (UnlinkAll,
//    0x6F25EF60 again), calls SetCount() (0x6F264DE0 again) to resize the
//    bucket array, and splices every node back in.
//  - SetCount()'s own grow path (0x6F264DE0) calls ComputeChunk()
//    (0x6F255F40) and SetAlloc() (0x6F2632F0).
//
//  Own translation unit for the same reason abilitytyperecordtable.cpp
//  gets one: a second, real out-of-line copy of the generic templates for
//  this specific T.
//============================================================================
#include "agiletype.h"
#include "tshash.inl"

struct SAbilityTypeRecordNode : public TSHashObject<SAbilityTypeRecordNode, AGILE_TYPE_ID>
{
    char m_reserved18[0x1F8 - 0x18];
};

//  0x6F2652D0 - Rehash().
template void TSHashTable<SAbilityTypeRecordNode, AGILE_TYPE_ID>::Rehash(unsigned int);

#include "tsarray.inl"
#include "tslist.inl"

//  0x6F255F40 - the bucket array's own ComputeChunk, reached out of
//  SetCount()'s grow path.
template unsigned int TSGrowableArray<TSExplicitList<SAbilityTypeRecordNode> >::ComputeChunk(unsigned int);
//  0x6F2632F0 - SetAlloc(), same path.
template void TSGrowableArray<TSExplicitList<SAbilityTypeRecordNode> >::SetAlloc(unsigned int);
//  0x6F264DE0 - SetCount(), reached from both Initialize() and Rehash().
template void TSGrowableArray<TSExplicitList<SAbilityTypeRecordNode> >::SetCount(unsigned int);
//  0x6F25EF60 - UnlinkAll(), reached from Initialize() (via the folded
//  SetLinkOffset()) and from Rehash()'s own node-parking step.
template void TSExplicitList<SAbilityTypeRecordNode>::UnlinkAll();
