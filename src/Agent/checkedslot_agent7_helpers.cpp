//============================================================================
//  Bodies for checkedslot_agent7_family.h - see that header and
//  unit_getability_agent7_batch.cpp for the family shape.
//
//  None of the three addresses per instance (leaf, Assign, converting
//  ctor) is this batch's own worklist heading - all three sit at BFS
//  depth 3, past this batch's own depth-1..2 cut - so none carries a
//  funcmap entry or a claimed verdict here.  Defined in their own TU
//  purely so the outer accessor's own call to the constructor stays a
//  real out-of-line `call` instead of being inlined away, matching every
//  other member of this family (agenttypedslots.h) having its own TU.
//============================================================================
#include "checkedslot_agent7_family.h"
#include "agiletype.h"
#include <excpt.h>

unsigned int GetFourCC_6F195D20()
{
    return 0x41616B62u;
}

SLocalAbilitySlot_6F2384E0::SLocalAbilitySlot_6F2384E0(CAgent* candidate)
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

SLocalAbilitySlot_6F2384E0* SLocalAbilitySlot_6F2384E0::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F195D20()))
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

void SLocalAbilitySlot_6F2384E0::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        if (--held->m_refcount == 0)
            held->ReleaseSelf();
    }
}

unsigned int GetFourCC_6F196D50()
{
    return 0x41626172u;
}

SLocalAbilitySlot_6F238C30::SLocalAbilitySlot_6F238C30(CAgent* candidate)
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

SLocalAbilitySlot_6F238C30* SLocalAbilitySlot_6F238C30::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F196D50()))
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

void SLocalAbilitySlot_6F238C30::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        if (--held->m_refcount == 0)
            held->ReleaseSelf();
    }
}

unsigned int GetFourCC_6F1984A0()
{
    return 0x41436266u;
}

SLocalAbilitySlot_6F239380::SLocalAbilitySlot_6F239380(CAgent* candidate)
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

SLocalAbilitySlot_6F239380* SLocalAbilitySlot_6F239380::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F1984A0()))
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

void SLocalAbilitySlot_6F239380::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        if (--held->m_refcount == 0)
            held->ReleaseSelf();
    }
}

unsigned int GetFourCC_6F1955E0()
{
    return 0x414E636Cu;
}

SLocalAbilitySlot_6F239AD0::SLocalAbilitySlot_6F239AD0(CAgent* candidate)
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

SLocalAbilitySlot_6F239AD0* SLocalAbilitySlot_6F239AD0::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F1955E0()))
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

void SLocalAbilitySlot_6F239AD0::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        if (--held->m_refcount == 0)
            held->ReleaseSelf();
    }
}

unsigned int GetFourCC_6F19A230()
{
    return 0x41636F68u;
}

SLocalAbilitySlot_6F23A150::SLocalAbilitySlot_6F23A150(CAgent* candidate)
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

SLocalAbilitySlot_6F23A150* SLocalAbilitySlot_6F23A150::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F19A230()))
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

void SLocalAbilitySlot_6F23A150::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        if (--held->m_refcount == 0)
            held->ReleaseSelf();
    }
}

unsigned int GetFourCC_6F07F780()
{
    return 0x41637273u;
}

SLocalAbilitySlot_6F23A7D0::SLocalAbilitySlot_6F23A7D0(CAgent* candidate)
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

SLocalAbilitySlot_6F23A7D0* SLocalAbilitySlot_6F23A7D0::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F07F780()))
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

void SLocalAbilitySlot_6F23A7D0::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        if (--held->m_refcount == 0)
            held->ReleaseSelf();
    }
}

unsigned int GetFourCC_6F07CD70()
{
    return 0x41646576u;
}

SLocalAbilitySlot_6F23B0C0::SLocalAbilitySlot_6F23B0C0(CAgent* candidate)
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

SLocalAbilitySlot_6F23B0C0* SLocalAbilitySlot_6F23B0C0::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F07CD70()))
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

void SLocalAbilitySlot_6F23B0C0::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        if (--held->m_refcount == 0)
            held->ReleaseSelf();
    }
}

unsigned int GetFourCC_6F0DA190()
{
    return 0x41656E63u;
}

SLocalAbilitySlot_6F23B740::SLocalAbilitySlot_6F23B740(CAgent* candidate)
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

SLocalAbilitySlot_6F23B740* SLocalAbilitySlot_6F23B740::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F0DA190()))
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

void SLocalAbilitySlot_6F23B740::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        if (--held->m_refcount == 0)
            held->ReleaseSelf();
    }
}

unsigned int GetFourCC_6F0818C0()
{
    return 0x4166616Bu;
}

SLocalAbilitySlot_6F23BE90::SLocalAbilitySlot_6F23BE90(CAgent* candidate)
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

SLocalAbilitySlot_6F23BE90* SLocalAbilitySlot_6F23BE90::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F0818C0()))
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

void SLocalAbilitySlot_6F23BE90::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        if (--held->m_refcount == 0)
            held->ReleaseSelf();
    }
}

unsigned int GetFourCC_6F112BD0()
{
    return 0x414E7061u;
}

SLocalAbilitySlot_6F23E730::SLocalAbilitySlot_6F23E730(CAgent* candidate)
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

SLocalAbilitySlot_6F23E730* SLocalAbilitySlot_6F23E730::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F112BD0()))
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

void SLocalAbilitySlot_6F23E730::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        if (--held->m_refcount == 0)
            held->ReleaseSelf();
    }
}

unsigned int GetFourCC_6F1D7B10()
{
    return 0x41497368u;
}

SLocalAbilitySlot_6F241240::SLocalAbilitySlot_6F241240(CAgent* candidate)
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

SLocalAbilitySlot_6F241240* SLocalAbilitySlot_6F241240::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F1D7B10()))
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

void SLocalAbilitySlot_6F241240::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        if (--held->m_refcount == 0)
            held->ReleaseSelf();
    }
}

unsigned int GetFourCC_6F1960A0()
{
    return 0x414F6165u;
}

SLocalAbilitySlot_6F242B70::SLocalAbilitySlot_6F242B70(CAgent* candidate)
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

SLocalAbilitySlot_6F242B70* SLocalAbilitySlot_6F242B70::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F1960A0()))
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

void SLocalAbilitySlot_6F242B70::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        if (--held->m_refcount == 0)
            held->ReleaseSelf();
    }
}

unsigned int GetFourCC_6F1D3BA0()
{
    return 0x414E6272u;
}

SLocalAbilitySlot_6F2431F0::SLocalAbilitySlot_6F2431F0(CAgent* candidate)
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

SLocalAbilitySlot_6F2431F0* SLocalAbilitySlot_6F2431F0::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F1D3BA0()))
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

void SLocalAbilitySlot_6F2431F0::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        if (--held->m_refcount == 0)
            held->ReleaseSelf();
    }
}

unsigned int GetFourCC_6F198D10()
{
    return 0x414F636Cu;
}

SLocalAbilitySlot_6F243870::SLocalAbilitySlot_6F243870(CAgent* candidate)
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

SLocalAbilitySlot_6F243870* SLocalAbilitySlot_6F243870::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F198D10()))
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

void SLocalAbilitySlot_6F243870::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        if (--held->m_refcount == 0)
            held->ReleaseSelf();
    }
}

unsigned int GetFourCC_6F025EB0()
{
    return 0x41556473u;
}

SLocalAbilitySlot_6F243EF0::SLocalAbilitySlot_6F243EF0(CAgent* candidate)
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

SLocalAbilitySlot_6F243EF0* SLocalAbilitySlot_6F243EF0::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F025EB0()))
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

void SLocalAbilitySlot_6F243EF0::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        if (--held->m_refcount == 0)
            held->ReleaseSelf();
    }
}

unsigned int GetFourCC_6F19AA40()
{
    return 0x414E6462u;
}

SLocalAbilitySlot_6F244570::SLocalAbilitySlot_6F244570(CAgent* candidate)
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

SLocalAbilitySlot_6F244570* SLocalAbilitySlot_6F244570::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F19AA40()))
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

void SLocalAbilitySlot_6F244570::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        if (--held->m_refcount == 0)
            held->ReleaseSelf();
    }
}

unsigned int GetFourCC_6F195460()
{
    return 0x414E6664u;
}

SLocalAbilitySlot_6F244BF0::SLocalAbilitySlot_6F244BF0(CAgent* candidate)
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

SLocalAbilitySlot_6F244BF0* SLocalAbilitySlot_6F244BF0::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F195460()))
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

void SLocalAbilitySlot_6F244BF0::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        if (--held->m_refcount == 0)
            held->ReleaseSelf();
    }
}

unsigned int GetFourCC_6F156E20()
{
    return 0x4155666Eu;
}

SLocalAbilitySlot_6F245270::SLocalAbilitySlot_6F245270(CAgent* candidate)
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

SLocalAbilitySlot_6F245270* SLocalAbilitySlot_6F245270::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F156E20()))
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

void SLocalAbilitySlot_6F245270::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        if (--held->m_refcount == 0)
            held->ReleaseSelf();
    }
}

unsigned int GetFourCC_6F113B50()
{
    return 0x41457061u;
}

SLocalAbilitySlot_6F246040::SLocalAbilitySlot_6F246040(CAgent* candidate)
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

SLocalAbilitySlot_6F246040* SLocalAbilitySlot_6F246040::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F113B50()))
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

void SLocalAbilitySlot_6F246040::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        if (--held->m_refcount == 0)
            held->ReleaseSelf();
    }
}

unsigned int GetFourCC_6F1D5800()
{
    return 0x414F7368u;
}

SLocalAbilitySlot_6F2466C0::SLocalAbilitySlot_6F2466C0(CAgent* candidate)
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

SLocalAbilitySlot_6F2466C0* SLocalAbilitySlot_6F2466C0::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F1D5800()))
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

void SLocalAbilitySlot_6F2466C0::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        if (--held->m_refcount == 0)
            held->ReleaseSelf();
    }
}

unsigned int GetFourCC_6F159BA0()
{
    return 0x414E7367u;
}

SLocalAbilitySlot_6F246D40::SLocalAbilitySlot_6F246D40(CAgent* candidate)
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

SLocalAbilitySlot_6F246D40* SLocalAbilitySlot_6F246D40::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F159BA0()))
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

void SLocalAbilitySlot_6F246D40::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        if (--held->m_refcount == 0)
            held->ReleaseSelf();
    }
}

unsigned int GetFourCC_6F158660()
{
    return 0x41457471u;
}

SLocalAbilitySlot_6F247630::SLocalAbilitySlot_6F247630(CAgent* candidate)
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

SLocalAbilitySlot_6F247630* SLocalAbilitySlot_6F247630::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F158660()))
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

void SLocalAbilitySlot_6F247630::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        if (--held->m_refcount == 0)
            held->ReleaseSelf();
    }
}

unsigned int GetFourCC_6F194880()
{
    return 0x414E6372u;
}

SLocalAbilitySlot_6F247CB0::SLocalAbilitySlot_6F247CB0(CAgent* candidate)
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

SLocalAbilitySlot_6F247CB0* SLocalAbilitySlot_6F247CB0::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F194880()))
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

void SLocalAbilitySlot_6F247CB0::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        if (--held->m_refcount == 0)
            held->ReleaseSelf();
    }
}

unsigned int GetFourCC_6F157300()
{
    return 0x414E7379u;
}

SLocalAbilitySlot_6F248330::SLocalAbilitySlot_6F248330(CAgent* candidate)
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

SLocalAbilitySlot_6F248330* SLocalAbilitySlot_6F248330::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F157300()))
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

void SLocalAbilitySlot_6F248330::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        if (--held->m_refcount == 0)
            held->ReleaseSelf();
    }
}

unsigned int GetFourCC_6F0DCB20()
{
    return 0x4149616Du;
}

SLocalAbilitySlot_6F2489B0::SLocalAbilitySlot_6F2489B0(CAgent* candidate)
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

SLocalAbilitySlot_6F2489B0* SLocalAbilitySlot_6F2489B0::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F0DCB20()))
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

void SLocalAbilitySlot_6F2489B0::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        if (--held->m_refcount == 0)
            held->ReleaseSelf();
    }
}

unsigned int GetFourCC_6F0DD320()
{
    return 0x4149656Du;
}

SLocalAbilitySlot_6F249030::SLocalAbilitySlot_6F249030(CAgent* candidate)
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

SLocalAbilitySlot_6F249030* SLocalAbilitySlot_6F249030::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F0DD320()))
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

void SLocalAbilitySlot_6F249030::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        if (--held->m_refcount == 0)
            held->ReleaseSelf();
    }
}

unsigned int GetFourCC_6F07FFD0()
{
    return 0x41496664u;
}

SLocalAbilitySlot_6F2496B0::SLocalAbilitySlot_6F2496B0(CAgent* candidate)
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

SLocalAbilitySlot_6F2496B0* SLocalAbilitySlot_6F2496B0::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F07FFD0()))
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

void SLocalAbilitySlot_6F2496B0::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        if (--held->m_refcount == 0)
            held->ReleaseSelf();
    }
}

unsigned int GetFourCC_6F082220()
{
    return 0x4149676Cu;
}

SLocalAbilitySlot_6F249D30::SLocalAbilitySlot_6F249D30(CAgent* candidate)
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

SLocalAbilitySlot_6F249D30* SLocalAbilitySlot_6F249D30::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F082220()))
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

void SLocalAbilitySlot_6F249D30::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        if (--held->m_refcount == 0)
            held->ReleaseSelf();
    }
}

unsigned int GetFourCC_6F0DCE20()
{
    return 0x41496D73u;
}

SLocalAbilitySlot_6F24A3B0::SLocalAbilitySlot_6F24A3B0(CAgent* candidate)
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

SLocalAbilitySlot_6F24A3B0* SLocalAbilitySlot_6F24A3B0::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F0DCE20()))
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

void SLocalAbilitySlot_6F24A3B0::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        if (--held->m_refcount == 0)
            held->ReleaseSelf();
    }
}

unsigned int GetFourCC_6F1993C0()
{
    return 0x4149636Fu;
}

SLocalAbilitySlot_6F24AB00::SLocalAbilitySlot_6F24AB00(CAgent* candidate)
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

SLocalAbilitySlot_6F24AB00* SLocalAbilitySlot_6F24AB00::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F1993C0()))
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

void SLocalAbilitySlot_6F24AB00::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        if (--held->m_refcount == 0)
            held->ReleaseSelf();
    }
}

unsigned int GetFourCC_6F111510()
{
    return 0x41496D61u;
}

SLocalAbilitySlot_6F24B180::SLocalAbilitySlot_6F24B180(CAgent* candidate)
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

SLocalAbilitySlot_6F24B180* SLocalAbilitySlot_6F24B180::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F111510()))
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

void SLocalAbilitySlot_6F24B180::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        if (--held->m_refcount == 0)
            held->ReleaseSelf();
    }
}

unsigned int GetFourCC_6F1D32E0()
{
    return 0x41497261u;
}

SLocalAbilitySlot_6F24B800::SLocalAbilitySlot_6F24B800(CAgent* candidate)
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

SLocalAbilitySlot_6F24B800* SLocalAbilitySlot_6F24B800::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F1D32E0()))
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

void SLocalAbilitySlot_6F24B800::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        if (--held->m_refcount == 0)
            held->ReleaseSelf();
    }
}

unsigned int GetFourCC_6F10FA10()
{
    return 0x41497675u;
}

SLocalAbilitySlot_6F24BE80::SLocalAbilitySlot_6F24BE80(CAgent* candidate)
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

SLocalAbilitySlot_6F24BE80* SLocalAbilitySlot_6F24BE80::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F10FA10()))
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

void SLocalAbilitySlot_6F24BE80::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        if (--held->m_refcount == 0)
            held->ReleaseSelf();
    }
}

unsigned int GetFourCC_6F1596A0()
{
    return 0x41496D6Fu;
}

SLocalAbilitySlot_6F24C500::SLocalAbilitySlot_6F24C500(CAgent* candidate)
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

SLocalAbilitySlot_6F24C500* SLocalAbilitySlot_6F24C500::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F1596A0()))
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

void SLocalAbilitySlot_6F24C500::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        if (--held->m_refcount == 0)
            held->ReleaseSelf();
    }
}

unsigned int GetFourCC_6F0DDB20()
{
    return 0x41497372u;
}

SLocalAbilitySlot_6F24CC50::SLocalAbilitySlot_6F24CC50(CAgent* candidate)
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

SLocalAbilitySlot_6F24CC50* SLocalAbilitySlot_6F24CC50::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F0DDB20()))
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

void SLocalAbilitySlot_6F24CC50::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        if (--held->m_refcount == 0)
            held->ReleaseSelf();
    }
}

unsigned int GetFourCC_6F0D91A0()
{
    return 0x41556275u;
}

SLocalAbilitySlot_6F29AC70::SLocalAbilitySlot_6F29AC70(CAgent* candidate)
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

SLocalAbilitySlot_6F29AC70* SLocalAbilitySlot_6F29AC70::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F0D91A0()))
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

void SLocalAbilitySlot_6F29AC70::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        if (--held->m_refcount == 0)
            held->ReleaseSelf();
    }
}

unsigned int GetFourCC_6F200960()
{
    return 0x416D6F76u;
}

SLocalAbilitySlot_6F29B2F0::SLocalAbilitySlot_6F29B2F0(CAgent* candidate)
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

SLocalAbilitySlot_6F29B2F0* SLocalAbilitySlot_6F29B2F0::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F200960()))
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

void SLocalAbilitySlot_6F29B2F0::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        if (--held->m_refcount == 0)
            held->ReleaseSelf();
    }
}

unsigned int GetFourCC_6F022E90()
{
    return 0x416F6172u;
}

SLocalAbilitySlot_6F238680::SLocalAbilitySlot_6F238680(CAgent* candidate)
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

SLocalAbilitySlot_6F238680* SLocalAbilitySlot_6F238680::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F022E90()))
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

void SLocalAbilitySlot_6F238680::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        if (--held->m_refcount == 0)
            held->ReleaseSelf();
    }
}

unsigned int GetFourCC_6F080250()
{
    return 0x41736F64u;
}

SLocalAbilitySlot_6F23AA40::SLocalAbilitySlot_6F23AA40(CAgent* candidate)
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

SLocalAbilitySlot_6F23AA40* SLocalAbilitySlot_6F23AA40::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F080250()))
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

void SLocalAbilitySlot_6F23AA40::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        if (--held->m_refcount == 0)
            held->ReleaseSelf();
    }
}

unsigned int GetFourCC_6F15AA90()
{
    return 0x41667368u;
}

SLocalAbilitySlot_6F23C030::SLocalAbilitySlot_6F23C030(CAgent* candidate)
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

SLocalAbilitySlot_6F23C030* SLocalAbilitySlot_6F23C030::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F15AA90()))
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

void SLocalAbilitySlot_6F23C030::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        if (--held->m_refcount == 0)
            held->ReleaseSelf();
    }
}

unsigned int GetFourCC_6F082660()
{
    return 0x41677964u;
}

SLocalAbilitySlot_6F23C780::SLocalAbilitySlot_6F23C780(CAgent* candidate)
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

SLocalAbilitySlot_6F23C780* SLocalAbilitySlot_6F23C780::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F082660()))
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

void SLocalAbilitySlot_6F23C780::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        if (--held->m_refcount == 0)
            held->ReleaseSelf();
    }
}

unsigned int GetFourCC_6F1D4CA0()
{
    return 0x41686964u;
}

SLocalAbilitySlot_6F23CE00::SLocalAbilitySlot_6F23CE00(CAgent* candidate)
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

SLocalAbilitySlot_6F23CE00* SLocalAbilitySlot_6F23CE00::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F1D4CA0()))
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

void SLocalAbilitySlot_6F23CE00::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        if (--held->m_refcount == 0)
            held->ReleaseSelf();
    }
}

unsigned int GetFourCC_6F15B410()
{
    return 0x416C6971u;
}

SLocalAbilitySlot_6F23D480::SLocalAbilitySlot_6F23D480(CAgent* candidate)
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

SLocalAbilitySlot_6F23D480* SLocalAbilitySlot_6F23D480::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F15B410()))
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

void SLocalAbilitySlot_6F23D480::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        if (--held->m_refcount == 0)
            held->ReleaseSelf();
    }
}

unsigned int GetFourCC_6F1118B0()
{
    return 0x416D656Cu;
}

SLocalAbilitySlot_6F23DBD0::SLocalAbilitySlot_6F23DBD0(CAgent* candidate)
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

SLocalAbilitySlot_6F23DBD0* SLocalAbilitySlot_6F23DBD0::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F1118B0()))
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

void SLocalAbilitySlot_6F23DBD0::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        if (--held->m_refcount == 0)
            held->ReleaseSelf();
    }
}

unsigned int GetFourCC_6F024C50()
{
    return 0x416E6474u;
}

SLocalAbilitySlot_6F23E250::SLocalAbilitySlot_6F23E250(CAgent* candidate)
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

SLocalAbilitySlot_6F23E250* SLocalAbilitySlot_6F23E250::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F024C50()))
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

void SLocalAbilitySlot_6F23E250::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        if (--held->m_refcount == 0)
            held->ReleaseSelf();
    }
}

unsigned int GetFourCC_6F113C40()
{
    return 0x41706C79u;
}

SLocalAbilitySlot_6F23EE80::SLocalAbilitySlot_6F23EE80(CAgent* candidate)
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

SLocalAbilitySlot_6F23EE80* SLocalAbilitySlot_6F23EE80::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F113C40()))
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

void SLocalAbilitySlot_6F23EE80::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        if (--held->m_refcount == 0)
            held->ReleaseSelf();
    }
}

unsigned int GetFourCC_6F1D7290()
{
    return 0x41726272u;
}

SLocalAbilitySlot_6F23F500::SLocalAbilitySlot_6F23F500(CAgent* candidate)
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

SLocalAbilitySlot_6F23F500* SLocalAbilitySlot_6F23F500::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F1D7290()))
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

void SLocalAbilitySlot_6F23F500::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        if (--held->m_refcount == 0)
            held->ReleaseSelf();
    }
}

unsigned int GetFourCC_6F1D3460()
{
    return 0x4172706Du;
}

SLocalAbilitySlot_6F23FB80::SLocalAbilitySlot_6F23FB80(CAgent* candidate)
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

SLocalAbilitySlot_6F23FB80* SLocalAbilitySlot_6F23FB80::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F1D3460()))
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

void SLocalAbilitySlot_6F23FB80::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        if (--held->m_refcount == 0)
            held->ReleaseSelf();
    }
}

unsigned int GetFourCC_6F0283A0()
{
    return 0x41726F6Fu;
}

SLocalAbilitySlot_6F240200::SLocalAbilitySlot_6F240200(CAgent* candidate)
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

SLocalAbilitySlot_6F240200* SLocalAbilitySlot_6F240200::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F0283A0()))
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

void SLocalAbilitySlot_6F240200::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        if (--held->m_refcount == 0)
            held->ReleaseSelf();
    }
}

unsigned int GetFourCC_6F07FD10()
{
    return 0x41736873u;
}

SLocalAbilitySlot_6F240A20::SLocalAbilitySlot_6F240A20(CAgent* candidate)
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

SLocalAbilitySlot_6F240A20* SLocalAbilitySlot_6F240A20::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F07FD10()))
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

void SLocalAbilitySlot_6F240A20::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        if (--held->m_refcount == 0)
            held->ReleaseSelf();
    }
}

unsigned int GetFourCC_6F198720()
{
    return 0x41737464u;
}

SLocalAbilitySlot_6F241310::SLocalAbilitySlot_6F241310(CAgent* candidate)
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

SLocalAbilitySlot_6F241310* SLocalAbilitySlot_6F241310::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F198720()))
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

void SLocalAbilitySlot_6F241310::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        if (--held->m_refcount == 0)
            held->ReleaseSelf();
    }
}

unsigned int GetFourCC_6F1565E0()
{
    return 0x41747069u;
}

SLocalAbilitySlot_6F241A60::SLocalAbilitySlot_6F241A60(CAgent* candidate)
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

SLocalAbilitySlot_6F241A60* SLocalAbilitySlot_6F241A60::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F1565E0()))
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

void SLocalAbilitySlot_6F241A60::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        if (--held->m_refcount == 0)
            held->ReleaseSelf();
    }
}

unsigned int GetFourCC_6F1D7780()
{
    return 0x41766E67u;
}

SLocalAbilitySlot_6F2421B0::SLocalAbilitySlot_6F2421B0(CAgent* candidate)
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

SLocalAbilitySlot_6F2421B0* SLocalAbilitySlot_6F2421B0::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F1D7780()))
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

void SLocalAbilitySlot_6F2421B0::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        if (--held->m_refcount == 0)
            held->ReleaseSelf();
    }
}

unsigned int GetFourCC_6F196690()
{
    return 0x41556175u;
}

SLocalAbilitySlot_6F242DE0::SLocalAbilitySlot_6F242DE0(CAgent* candidate)
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

SLocalAbilitySlot_6F242DE0* SLocalAbilitySlot_6F242DE0::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F196690()))
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

void SLocalAbilitySlot_6F242DE0::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        if (--held->m_refcount == 0)
            held->ReleaseSelf();
    }
}

unsigned int GetFourCC_6F111610()
{
    return 0x414E6D73u;
}

SLocalAbilitySlot_6F245C30::SLocalAbilitySlot_6F245C30(CAgent* candidate)
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

SLocalAbilitySlot_6F245C30* SLocalAbilitySlot_6F245C30::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F111610()))
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

void SLocalAbilitySlot_6F245C30::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        if (--held->m_refcount == 0)
            held->ReleaseSelf();
    }
}

unsigned int GetFourCC_6F157580()
{
    return 0x414E6373u;
}

SLocalAbilitySlot_6F247F20::SLocalAbilitySlot_6F247F20(CAgent* candidate)
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

SLocalAbilitySlot_6F247F20* SLocalAbilitySlot_6F247F20::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F157580()))
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

void SLocalAbilitySlot_6F247F20::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        if (--held->m_refcount == 0)
            held->ReleaseSelf();
    }
}

unsigned int GetFourCC_6F024BF0()
{
    return 0x41497461u;
}

SLocalAbilitySlot_6F24AD70::SLocalAbilitySlot_6F24AD70(CAgent* candidate)
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

SLocalAbilitySlot_6F24AD70* SLocalAbilitySlot_6F24AD70::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F024BF0()))
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

void SLocalAbilitySlot_6F24AD70::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        if (--held->m_refcount == 0)
            held->ReleaseSelf();
    }
}

//  GetFourCC_6F0278B0 is already real - Misc/misc_trivial_getters.cpp - so
//  it is declared (checkedslot_agent7_family.h), not redefined, here.

SLocalAbilitySlot_6F29AEE0::SLocalAbilitySlot_6F29AEE0(CAgent* candidate)
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

SLocalAbilitySlot_6F29AEE0* SLocalAbilitySlot_6F29AEE0::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F0278B0()))
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

void SLocalAbilitySlot_6F29AEE0::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        if (--held->m_refcount == 0)
            held->ReleaseSelf();
    }
}
