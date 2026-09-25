//============================================================================
//  0x6F32C8E0/0x6F32B650/0x6F32A8B0 - CAbility's depth<=5 closure batch 3.
//  See abilityfieldrecordtable.h for the shared table/node shape.
//
//  0x6F32B650 and 0x6F32A8B0 are not new code at all: they are two more
//  members of the same TSHashTable<T, K> template every other
//  instantiation in this binary already shares (Containers/tshash.inl) -
//  NewNode() and CheckGrow() respectively, byte-for-byte, once T is
//  SAbilityFieldRecordNode and K is AGILE_TYPE_ID.  Explicit
//  instantiation, the same way tshashtable.cpp does it for
//  AGILE_TYPE_RELATIONSHIP.  Ptr() (0x6F320A40), Initialize()
//  (0x6F327DF0), Rehash() (0x6F328200) and the bucket array's own
//  SetCount()/UnlinkAll() are the same template's other members, reached
//  from this same closure but instantiated in abilityfieldrecordtable.cpp
//  by a sibling batch - not repeated here, an explicit instantiation in
//  two translation units is a duplicate-symbol link error.
//============================================================================
#include "ability_typefieldrecord.h"
#include "tshash.inl"

typedef SAbilityFieldRecordNode RECORD;

template int  TSHashTable<RECORD, AGILE_TYPE_ID>::CheckGrow(unsigned int);
template RECORD* TSHashTable<RECORD, AGILE_TYPE_ID>::NewNode(unsigned int, void*, void*);

//  NewNode's own body reaches Initialize() (mask == -1, already
//  instantiated in abilityfieldrecordtable.cpp) and CheckGrow's own growth
//  path reaches SetAlloc()/ComputeChunk(), which are not instantiated
//  anywhere else - this TU needs its own copies of just those two, or the
//  .mix does not link.
#include "tsarray.inl"

template unsigned int TSGrowableArray<TSExplicitList<RECORD> >::ComputeChunk(unsigned int);
template void TSGrowableArray<TSExplicitList<RECORD> >::SetAlloc(unsigned int);

//----------------------------------------------------------------------------
//  0x6F32C8E0 - LookupAbilityFieldRecord.  Hash typeId the same way
//  AGILE_TYPE_ID::Hash always does (agileid.cpp), look it up in the
//  table, and on a miss allocate a fresh node, key it, and let its own
//  vtable slot 0 initialise it with the raw typeId.
//----------------------------------------------------------------------------
void* __fastcall LookupAbilityFieldRecord(unsigned int typeId, const char*)
{
    AGILE_TYPE_ID key(typeId);
    unsigned int hashval = key.Hash();

    RECORD* record = g_abilityFieldRecordTable.Ptr(hashval, key);
    if (record != 0)
        return record;

    record = g_abilityFieldRecordTable.NewNode(hashval, 0, 0);
    record->m_hashval = hashval;
    record->m_key = key;
    record->FinishConstruct(typeId);
    return record;
}
