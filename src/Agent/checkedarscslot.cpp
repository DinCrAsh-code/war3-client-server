//============================================================================
//  Bodies for checkedarscslot.h - see that header for the family shape.
//============================================================================
#include "checkedarscslot.h"
#include "agiletype.h"
#include <excpt.h>

unsigned int GetFourCC_6F1D3160()
{
    return 0x41727363u;   // 'Arsc'
}

SLocalAbilitySlot_Arsc::SLocalAbilitySlot_Arsc(CAgent* candidate)
{
    m_value = 0;
    __try
    {
        Assign(candidate);
    }
    __finally
    {
        if (AbnormalTermination())
            Release();
    }
}

SLocalAbilitySlot_Arsc* SLocalAbilitySlot_Arsc::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F1D3160()))
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

void SLocalAbilitySlot_Arsc::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        held->m_refcount += -1;
        if (held->m_refcount == 0)
            held->ReleaseSelf();
    }
}
