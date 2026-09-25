//============================================================================
//  0x6F4036B0 - CDependencyAliasTableOwner::FindOrCreateRecord, split into
//  its own translation unit for the reason footprinttypename.cpp already
//  gives for the same shape: CountRequirement/GetThreshold
//  (ability_typedep.cpp) call this for real in the shipped code, and
//  MSVC inlines a same-TU four/five-line callee like this one whenever it
//  judges the call site profitable - which loses the real, out-of-line
//  `call` both of those callers have.
//============================================================================
#include "ability_typedep.h"
#include "footprinttype.h"
#include "unittypequeries.h"
#include "tshash.inl"

template CDependencyAliasRecord* TSHashTable<CDependencyAliasRecord, AGILE_TYPE_ID>::Ptr(
    unsigned int, const AGILE_TYPE_ID&);
template CDependencyAliasRecord* TSHashTable<CDependencyAliasRecord, AGILE_TYPE_ID>::NewNode(
    unsigned int, void*, void*);

//  Same reason ability_typefieldrecord.cpp needs its own copies: NewNode
//  reaches Initialize()/UnlinkAll() through tsarray.inl/tslist.inl,
//  neither of which tshash.inl itself defines.
#define TSARRAY_TYPENAME ".?AV?$TSExplicitList@VCDependencyAliasRecord@@$0?CCCCCCCD@@@"
#include "tsarray.inl"
#include "tslist.inl"

template unsigned int TSGrowableArray<TSExplicitList<CDependencyAliasRecord> >::ComputeChunk(unsigned int);
template void TSGrowableArray<TSExplicitList<CDependencyAliasRecord> >::SetAlloc(unsigned int);
template void TSGrowableArray<TSExplicitList<CDependencyAliasRecord> >::SetCount(unsigned int);
template void TSExplicitList<CDependencyAliasRecord, TSLIST_LINKOFFSET_POISON>::UnlinkAll();

//----------------------------------------------------------------------------
//  0x6F4036B0 - find or create typeId's dependency-alias record.  Same
//  shape as LookupAbilityFieldRecord (ability_typefieldrecord.cpp) with
//  one real difference: this record has no vftable of its own, so once
//  freshly made it is finished with three ordinary predicate calls
//  rather than one virtual dispatch, folding a category bit into +0x30.
//
//  The first predicate (0x6F2B0000, 71 instructions - its own /GS
//  stack-cookie frame plus a five-deep closure of still-unnamed
//  CObjectDataTable helpers, none of them dumped - see
//  cunit_vtable_closure_worklist.json's own note for this address) stays a
//  declared-only thunk; it is also thunked separately as `IsKnownDataKey`
//  in Game/objectdatatable_thunks.cpp and there is nothing here to
//  reconstruct it *from* without those helpers' own bodies.  The second is
//  not a new predicate at all: 0x6F32CC80 is GetFootprintIsStructureFlag,
//  already reconstructed (footprintisstructure.cpp) - "IsStructure" on the
//  *footprint* type registry, called here with the same typeId.  The third
//  (0x6F29D2F0) is now reconstructed for real -
//  UnitTypeIsRegistered_6F29D2F0 (unittypequeries.cpp,
//  cunit_vtable_closure_worklist.json slot 107 closure batch 24) - and
//  called through directly rather than re-thunked here.
//----------------------------------------------------------------------------
extern int __fastcall DependencyCategoryPredicateA(unsigned int typeId);  // 0x6F2B0000
__declspec(naked) int __fastcall DependencyCategoryPredicateA(unsigned int)
{
    __asm { mov eax, 0x6F2B0000 }
    __asm { jmp eax }
}

CDependencyAliasRecord* CDependencyAliasTableOwner::FindOrCreateRecord(
    unsigned int typeId, int allowCreate)
{
    AGILE_TYPE_ID key(typeId);
    unsigned int hashval = key.Hash();

    CDependencyAliasRecord* record = m_table.Ptr(hashval, key);
    if (record != 0 || !allowCreate)
        return record;

    record = m_table.NewNode(hashval, 0, 0);
    record->m_hashval = hashval;
    record->m_key = key;

    if (DependencyCategoryPredicateA(typeId))
        record->m_flags |= 2;
    else if (GetFootprintIsStructureFlag(typeId))
        record->m_flags |= 4;
    else if (UnitTypeIsRegistered_6F29D2F0(typeId))
        record->m_flags |= 8;
    return record;
}
