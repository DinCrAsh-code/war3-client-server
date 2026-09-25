//============================================================================
//  g_abilityTypeRecordTable (dword_6FAB3E48) - the global cache
//  LookupAbilityTypeRecord (abilitylevelfield.cpp, 0x6F265ED0) looks a
//  resolved ability type id up in before building a fresh record: a fourth
//  TSHashTable<T,K> instantiation, keyed by AGILE_TYPE_ID exactly like
//  agiletype.h's own tables (Containers/tshashtable.cpp,
//  Containers/tshashtype.cpp) and Unit/unittypetable.h's CUnitTypeTable -
//  same template, only the node shape differs.
//
//  Every function here is tshash.inl's own generic body, reused verbatim -
//  confirmed field for field against the shipped stream: `[ecx+24h]==-1`
//  (m_mask), `[ecx+1Ch]` (m_buckets.m_data), the bucket index
//  `hashval & m_mask`, the walk through `Tail()`/`Prev()`, and the match
//  test `ptr->m_hashval==hashval && ptr->m_key==key` with m_key at the
//  node's own +0x14 - TSHashObject<T,K>'s layout again.  0x6F2656D0's own
//  tail (`m_list.LinkToHead(ptr)`, retn 0Ch for the three
//  (hashval,arg1,arg2) stack args) is NewNode(), not New(): the record's
//  own m_hashval/m_key stores happen in its caller
//  (abilitytyperecordbuild.cpp's record-construction function) right after
//  the NewNode() call rather than through New() itself, which is why
//  NewNode and not New is the member reached here.
//
//  Own translation unit, the same reason every other TSHashTable<T,K>
//  instantiation gets one: a second, real out-of-line copy of tshash.inl's
//  templates for this specific T, with nothing else sharing the file.
//============================================================================
#include "agiletype.h"
#include "tshash.inl"

//----------------------------------------------------------------------------
//  The record node.  TSHashObject<T,K> supplies the first 0x18 bytes
//  (m_hashval, m_hashlink, the table-wide list link, m_key); the rest is
//  abilitytyperecordbuild.cpp's own layout - this file only needs the type
//  complete enough to instantiate the table's template members, none of
//  which touch a field past +0x18.
//----------------------------------------------------------------------------
struct SAbilityTypeRecordNode : public TSHashObject<SAbilityTypeRecordNode, AGILE_TYPE_ID>
{
    char m_reserved18[0x1F8 - 0x18];
};

//  0x6F264D50 - Ptr().
template SAbilityTypeRecordNode*
TSHashTable<SAbilityTypeRecordNode, AGILE_TYPE_ID>::Ptr(unsigned int,
                                                        const AGILE_TYPE_ID&);
//  0x6F265270 - Initialize(): first use of the table, four buckets,
//  m_mask=3.
template void TSHashTable<SAbilityTypeRecordNode, AGILE_TYPE_ID>::Initialize();
//  0x6F265630 - CheckGrow().
template int  TSHashTable<SAbilityTypeRecordNode, AGILE_TYPE_ID>::CheckGrow(unsigned int);
//  0x6F2656D0 - NewNode().
template SAbilityTypeRecordNode*
TSHashTable<SAbilityTypeRecordNode, AGILE_TYPE_ID>::NewNode(unsigned int, void*, void*);

//  The table itself.  A global *object*, not a pointer - the shipped code
//  loads its address with `mov ecx, offset dword_6FAB3E48` - never given a
//  defining declaration here (same as g_unitTypeTable in
//  Unit/unittypetable.h): it already exists, built by the game's own
//  untouched startup code.
extern TSHashTable<SAbilityTypeRecordNode, AGILE_TYPE_ID> g_abilityTypeRecordTable;

//  0x6F2657A0 - build and populate a fresh record for a type id the table
//  does not have yet.  Out of scope for this batch: 607 instructions, its
//  own real `__CxxFrameHandler3` unwind frame (see
//  Item/abilitygenericfieldrecord.cpp's own header note on why this
//  toolchain cannot reproduce that frame regardless of source shape), and
//  a dozen callees of its own.  Naked redirect, `retn` bare (0 stack
//  args) - the shipped code's own tail per its dump.
__declspec(naked) SAbilityTypeRecordNode* __fastcall
CreateAndPopulateAbilityTypeRecord(unsigned int typeId)
{
    __asm { mov eax, 06F2657A0h }
    __asm { jmp eax }
}

//----------------------------------------------------------------------------
//  0x6F265ED0 - LookupAbilityTypeRecord (abilitylevelfield.cpp's own
//  declaration).  Hash the type id and consult the table; build a fresh
//  record only on a miss.  The `selector` parameter is never read here -
//  every call site in this closure passes 0, and this function's own body
//  reads only the `ecx` half of it.
//----------------------------------------------------------------------------
void* __fastcall LookupAbilityTypeRecord(unsigned int typeId, int)
{
    AGILE_TYPE_ID id(typeId);
    SAbilityTypeRecordNode* record = g_abilityTypeRecordTable.Ptr(id.Hash(), id);
    if (record != 0)
        return record;
    return CreateAndPopulateAbilityTypeRecord(typeId);
}
