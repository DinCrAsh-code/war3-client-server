//============================================================================
//  0x6F02E540 / 0x6F02E5A0 - a matched pair of private helpers (neither is
//  a vtable slot on its own): refresh CAbility's type-record cache
//  (m_field54, via LookupAbilityTypeRecord - abilitylevelfield.cpp's own
//  thunk to 0x6F265ED0) the same way SetTypeId's own tail does, then hand
//  back one word of the record - or the sentinel 1 when there is no ready
//  record (null, or +0x2C zero).  0x6F02E540 reads the record's own +0x38;
//  0x6F02E5A0 reads +0x40.  Both `this` only, `retn 0`.
//
//  Own translation unit: 0x6F02Exxx is a different shipped module from
//  0x6F052xxx (ability_slots.cpp, where both callers - Method_6F052B10 and
//  Method_6F0529F0, worklist slots 32/131 - live), and each of these two
//  is small enough that /Ob2 folds it straight into its caller when they
//  share a TU, which is not what the shipped code's own real `call` does.
//============================================================================
#include "ability.h"

//  0x6F265ED0 - abilitylevelfield.cpp's own thunk; declared locally here
//  the same way ability_slots.cpp already does, since that file does not
//  expose it through a shared header.
extern void* __fastcall LookupAbilityTypeRecord(unsigned int typeId, int selector);

int CAbility::Method_6F02E540()
{
    if (m_field54 == 0)
        m_field54 = (unsigned int)LookupAbilityTypeRecord(m_typeId, 0);

    if (m_field54 != 0 && *(int*)(m_field54 + 0x2C) != 0)
        return *(int*)(m_field54 + 0x38);

    return 1;
}

int CAbility::Method_6F02E5A0()
{
    if (m_field54 == 0)
        m_field54 = (unsigned int)LookupAbilityTypeRecord(m_typeId, 0);

    if (m_field54 != 0 && *(int*)(m_field54 + 0x2C) != 0)
        return *(int*)(m_field54 + 0x40);

    return 1;
}
