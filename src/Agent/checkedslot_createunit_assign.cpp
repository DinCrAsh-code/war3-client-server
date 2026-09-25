//============================================================================
//  0x6F085E40 / 0x6F0C58F0 - SCheckedSlot_6F26C3E0::Assign and
//  SCheckedSlot_6F2E59B0::Assign (checkedslot_createunit.h).  Byte-for-byte
//  the same body as every other member of the family, but with the getter
//  called *inline inside* the `candidate &&` short-circuit rather than
//  hoisted into its own local ahead of the null test - the dump's own
//  `call sub_6F26C3E0` sits after `test edi,edi` / `jz`, matching
//  checkedslot_batch25.cpp's CHECKED_SLOT macro shape rather than
//  checkedaqueslot.cpp's hoisted-`required`-local one.
//============================================================================
#include "checkedslot_createunit.h"
#include "agiletype.h"

SCheckedSlot_6F26C3E0* SCheckedSlot_6F26C3E0::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(),
                                             GetFixedRawcode_6F26C3E0()))
        checked = candidate;
    else
        checked = 0;

    CAgent* old = m_value;
    if (old != checked)
    {
        if (old)
        {
            if (--old->m_refcount == 0)
                old->ReleaseSelf();
        }
        if (checked)
        {
            if (candidate)
                candidate->m_refcount++;
        }
        m_value = checked;
    }
    return this;
}

SCheckedSlot_6F2E59B0* SCheckedSlot_6F2E59B0::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(),
                                             GetFixedRawcode_6F2E59B0()))
        checked = candidate;
    else
        checked = 0;

    CAgent* old = m_value;
    if (old != checked)
    {
        if (old)
        {
            if (--old->m_refcount == 0)
                old->ReleaseSelf();
        }
        if (checked)
        {
            if (candidate)
                candidate->m_refcount++;
        }
        m_value = checked;
    }
    return this;
}
