//============================================================================
//  0x6F02E9E0 - CAbilityInventory::GetLevelField: read one per-level field
//  out of the ability's own type record, or hand back zero.
//
//  Own translation unit: it is a real out-of-line call from
//  CAbilityInventory::GetSlotCount (0x6F0E4050, abilityinventoryslot.cpp)
//  in the 0x6F0Exxx module, while this is 0x6F02Exxx - two modules, and
//  the shipped code keeps them apart.
//
//  The type record is resolved lazily from the ability's own rawcode and
//  cached at +0x54.  Three things can send the read to the zero answer,
//  and the shipped code tests them in this order with a shared tail:
//  no record at all, a record whose +0x2C is zero, and a level at or past
//  the record's row count.  A *negative* level is rejected before any of
//  that, by a signed `jge` at the top, and returns zero without even
//  touching the cache - so it is a separate branch and not folded into
//  the unsigned bounds test further down.
//
//  Row stride is 0x68 bytes and the field is at +0x20 within a row; the
//  rows themselves hang off the record at +0x54.
//
//  The out buffer is returned, not the value: the shipped code's `mov
//  eax,[esp+arg_0]` before the store is what puts the *pointer* in eax,
//  which is what the caller then hands to CFloatToInt.
//============================================================================
#include "abilityinventoryslot.h"
#include "CFloat.h"

//----------------------------------------------------------------------------
//  0x6F265ED0 - resolve an ability rawcode to its type record.
//  Reconstructed for real in Item/abilitytyperecordtable.cpp (the same
//  module as g_abilityTypeRecordTable's own TSHashTable<T,K> members it
//  calls into); declared here the same way that file's own callers
//  redeclare a shared free function - a second declaration with the
//  identical signature is the same symbol as far as the linker is
//  concerned.  __fastcall (rawcode in ecx, a zero selector in edx) and
//  `retn 0`, so nothing is pushed for it.
//----------------------------------------------------------------------------
void* __fastcall LookupAbilityTypeRecord(unsigned int typeId, int selector);

//----------------------------------------------------------------------------
//  The record's own three touched fields.  A local type rather than a
//  named class: this call tree reads nothing else of it, and CLAUDE.md's
//  rule is that the offsets are recoverable and the layout around them is
//  not.
//----------------------------------------------------------------------------
struct SAbilityTypeRecord
{
    char            m_reserved00[0x2C];
    //  0x2C - zero means the record is present but carries no rows.
    int             m_ready;        // 0x2C
    char            m_reserved30[0x20];
    //  0x50 - how many level rows the record has.
    unsigned int    m_rowCount;     // 0x50
    //  0x54 - the rows, 0x68 bytes each.
    char*           m_rows;         // 0x54
};

const int kAbilityLevelRowStride = 0x68;
const int kAbilityLevelRowValue  = 0x20;

CFloat* CAbilityInventory::GetLevelField(CFloat* out, int level)
{
    if (level < 0)
    {
        *out = g_CFloatZero;
        return out;
    }

    if (m_pTypeRecord == 0)
        m_pTypeRecord = LookupAbilityTypeRecord(m_typeId, 0);

    SAbilityTypeRecord* record = (SAbilityTypeRecord*)m_pTypeRecord;
    if (record && record->m_ready != 0 && record->m_rowCount > (unsigned int)level)
    {
        *out = *(const CFloat*)(record->m_rows +
                                level * kAbilityLevelRowStride +
                                kAbilityLevelRowValue);
        return out;
    }

    *out = g_CFloatZero;
    return out;
}
