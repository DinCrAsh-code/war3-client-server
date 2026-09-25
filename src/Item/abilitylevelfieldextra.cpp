//============================================================================
//  0x6F02EB30 / 0x6F02EAC0 - two more per-level type-record field reads,
//  the same shape CAbilityInventory::GetLevelField (0x6F02E9E0,
//  abilitylevelfield.cpp) already has and adjacent to it in the same
//  0x6F02Exxx module: negative index rejected first and unconditionally
//  (matching `level < 0` there), then the lazily-cached type record
//  (m_pTypeRecord/m_typeId), its own "ready" flag, and the row bound -
//  and a different row field apiece, 0x2C and 0x28 rather than
//  GetLevelField's 0x20.
//
//  Own translation unit, next to GetLevelField's rather than inside it:
//  neither calls the other and neither is called from here, so nothing
//  forces them together, but 0x6F02EB30/0x6F02EAC0 are a matched pair (each
//  one instruction apart in the dump) with two independent callers -
//  CUnit::IsOwnedByAPlayer (0x6F277A80, unitisownedbyaplayer.cpp) and
//  CAbilityInventory::AcceptsPowerup (0x6F0E3C50, abilityacceptspowerup.cpp)
//  - so sharing a TU with either caller risks folding it into just the one
//  that happens to sit beside it.
//
//  Names: neither caller's own name is any more informative about what the
//  field itself means, so these are named for the field they read rather
//  than invented ability semantics - GetLevelField's own row field (+0x20)
//  is not named beyond "the level field" either.
//============================================================================
#include "unitinventory.h"
#include "CFloat.h"

//  0x6F265ED0 - resolve an ability rawcode to its type record.  Same
//  declaration as abilitylevelfield.cpp's; a second declaration of the same
//  extern symbol is nothing the linker minds.
void* __fastcall LookupAbilityTypeRecord(unsigned int typeId, int selector);

//  The record's own three touched fields - same shape as
//  abilitylevelfield.cpp's SAbilityTypeRecord, a private local type in both
//  places because CLAUDE.md's rule is that the offsets are recoverable and
//  the surrounding layout is not.
struct SAbilityTypeRecord
{
    char            m_reserved00[0x2C];
    int             m_ready;        // 0x2C
    char            m_reserved30[0x20];
    unsigned int    m_rowCount;     // 0x50
    char*           m_rows;         // 0x54
};

const int kAbilityLevelRowStride = 0x68;

//----------------------------------------------------------------------------
//  0x6F02EB30 - row field +0x2C.  CUnit::IsOwnedByAPlayer's own callee,
//  through the unit's inventory ability.
//----------------------------------------------------------------------------
CFloat* CAbilityInventory::GetOwnershipLevelField(CFloat* out, int level)
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
                                level * kAbilityLevelRowStride + 0x2C);
        return out;
    }

    *out = g_CFloatZero;
    return out;
}

//----------------------------------------------------------------------------
//  0x6F02EAC0 - row field +0x28.  CAbilityInventory::AcceptsPowerup's own
//  callee, on `this` directly rather than through a unit.
//----------------------------------------------------------------------------
CFloat* CAbilityInventory::GetPowerupLevelField(CFloat* out, int level)
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
                                level * kAbilityLevelRowStride + 0x28);
        return out;
    }

    *out = g_CFloatZero;
    return out;
}
