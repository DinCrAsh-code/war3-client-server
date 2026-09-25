//============================================================================
//  Bodies for checkedslot_agent7_batch_slot107.h - see that header and
//  unit_getability_batch_slot107.cpp for the family shape (and
//  checkedslot_agent7_helpers.cpp for the sibling batch this one extends).
//============================================================================
#include "checkedslot_agent7_batch_slot107.h"
#include "agiletype.h"
#include <excpt.h>

unsigned int GetFourCC_6F026870()
{
    return 0x41616c6cu;   // 'Aall'
}

SLocalAbilitySlot_6F2380D0::SLocalAbilitySlot_6F2380D0(CAgent* candidate)
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

SLocalAbilitySlot_6F2380D0* SLocalAbilitySlot_6F2380D0::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F026870()))
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

void SLocalAbilitySlot_6F2380D0::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        held->m_refcount += -1;
        if (held->m_refcount == 0)
            held->ReleaseSelf();
    }
}

unsigned int GetFourCC_6F07CDF0()
{
    return 0x41436476u;   // 'ACdv'
}

SLocalAbilitySlot_6F23A3C0::SLocalAbilitySlot_6F23A3C0(CAgent* candidate)
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

SLocalAbilitySlot_6F23A3C0* SLocalAbilitySlot_6F23A3C0::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F07CDF0()))
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

void SLocalAbilitySlot_6F23A3C0::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        held->m_refcount += -1;
        if (held->m_refcount == 0)
            held->ReleaseSelf();
    }
}

unsigned int GetFourCC_6F081A60()
{
    return 0x41666c61u;   // 'Afla'
}

SLocalAbilitySlot_6F23BF60::SLocalAbilitySlot_6F23BF60(CAgent* candidate)
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

SLocalAbilitySlot_6F23BF60* SLocalAbilitySlot_6F23BF60::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F081A60()))
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

void SLocalAbilitySlot_6F23BF60::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        held->m_refcount += -1;
        if (held->m_refcount == 0)
            held->ReleaseSelf();
    }
}

unsigned int GetFourCC_6F07E3A0()
{
    return 0x41677261u;   // 'Agra'
}

SLocalAbilitySlot_6F23C6B0::SLocalAbilitySlot_6F23C6B0(CAgent* candidate)
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

SLocalAbilitySlot_6F23C6B0* SLocalAbilitySlot_6F23C6B0::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F07E3A0()))
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

void SLocalAbilitySlot_6F23C6B0::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        held->m_refcount += -1;
        if (held->m_refcount == 0)
            held->ReleaseSelf();
    }
}

unsigned int GetFourCC_6F082830()
{
    return 0x41687764u;   // 'Ahwd'
}

SLocalAbilitySlot_6F23CD30::SLocalAbilitySlot_6F23CD30(CAgent* candidate)
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

SLocalAbilitySlot_6F23CD30* SLocalAbilitySlot_6F23CD30::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F082830()))
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

void SLocalAbilitySlot_6F23CD30::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        held->m_refcount += -1;
        if (held->m_refcount == 0)
            held->ReleaseSelf();
    }
}

unsigned int GetFourCC_6F1100B0()
{
    return 0x416c7368u;   // 'Alsh'
}

SLocalAbilitySlot_6F23D3B0::SLocalAbilitySlot_6F23D3B0(CAgent* candidate)
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

SLocalAbilitySlot_6F23D3B0* SLocalAbilitySlot_6F23D3B0::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F1100B0()))
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

void SLocalAbilitySlot_6F23D3B0::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        held->m_refcount += -1;
        if (held->m_refcount == 0)
            held->ReleaseSelf();
    }
}

unsigned int GetFourCC_6F111830()
{
    return 0x416d6564u;   // 'Amed'
}

SLocalAbilitySlot_6F23DB00::SLocalAbilitySlot_6F23DB00(CAgent* candidate)
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

SLocalAbilitySlot_6F23DB00* SLocalAbilitySlot_6F23DB00::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F111830()))
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

void SLocalAbilitySlot_6F23DB00::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        held->m_refcount += -1;
        if (held->m_refcount == 0)
            held->ReleaseSelf();
    }
}

unsigned int GetFourCC_6F026790()
{
    return 0x416e6575u;   // 'Aneu'
}

SLocalAbilitySlot_6F23E180::SLocalAbilitySlot_6F23E180(CAgent* candidate)
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

SLocalAbilitySlot_6F23E180* SLocalAbilitySlot_6F23E180::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F026790()))
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

void SLocalAbilitySlot_6F23E180::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        held->m_refcount += -1;
        if (held->m_refcount == 0)
            held->ReleaseSelf();
    }
}

unsigned int GetFourCC_6F1139D0()
{
    return 0x41706f69u;   // 'Apoi'
}

SLocalAbilitySlot_6F23EDB0::SLocalAbilitySlot_6F23EDB0(CAgent* candidate)
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

SLocalAbilitySlot_6F23EDB0* SLocalAbilitySlot_6F23EDB0::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F1139D0()))
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

void SLocalAbilitySlot_6F23EDB0::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        held->m_refcount += -1;
        if (held->m_refcount == 0)
            held->ReleaseSelf();
    }
}

unsigned int GetFourCC_6F1D2D80()
{
    return 0x41726176u;   // 'Arav'
}

SLocalAbilitySlot_6F23F430::SLocalAbilitySlot_6F23F430(CAgent* candidate)
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

SLocalAbilitySlot_6F23F430* SLocalAbilitySlot_6F23F430::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F1D2D80()))
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

void SLocalAbilitySlot_6F23F430::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        held->m_refcount += -1;
        if (held->m_refcount == 0)
            held->ReleaseSelf();
    }
}

unsigned int GetFourCC_6F1D33E0()
{
    return 0x4172706cu;   // 'Arpl'
}

SLocalAbilitySlot_6F23FAB0::SLocalAbilitySlot_6F23FAB0(CAgent* candidate)
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

SLocalAbilitySlot_6F23FAB0* SLocalAbilitySlot_6F23FAB0::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F1D33E0()))
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

void SLocalAbilitySlot_6F23FAB0::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        held->m_refcount += -1;
        if (held->m_refcount == 0)
            held->ReleaseSelf();
    }
}

unsigned int GetFourCC_6F1D3AA0()
{
    return 0x41726f61u;   // 'Aroa'
}

SLocalAbilitySlot_6F240130::SLocalAbilitySlot_6F240130(CAgent* candidate)
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

SLocalAbilitySlot_6F240130* SLocalAbilitySlot_6F240130::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F1D3AA0()))
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

void SLocalAbilitySlot_6F240130::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        held->m_refcount += -1;
        if (held->m_refcount == 0)
            held->ReleaseSelf();
    }
}

unsigned int GetFourCC_6F1D4C20()
{
    return 0x4173686du;   // 'Ashm'
}

SLocalAbilitySlot_6F240950::SLocalAbilitySlot_6F240950(CAgent* candidate)
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

SLocalAbilitySlot_6F240950* SLocalAbilitySlot_6F240950::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F1D4C20()))
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

void SLocalAbilitySlot_6F240950::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        held->m_refcount += -1;
        if (held->m_refcount == 0)
            held->ReleaseSelf();
    }
}

unsigned int GetFourCC_6F1D73D0()
{
    return 0x4173706cu;   // 'Aspl'
}

SLocalAbilitySlot_6F241170::SLocalAbilitySlot_6F241170(CAgent* candidate)
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

SLocalAbilitySlot_6F241170* SLocalAbilitySlot_6F241170::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F1D73D0()))
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

void SLocalAbilitySlot_6F241170::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        held->m_refcount += -1;
        if (held->m_refcount == 0)
            held->ReleaseSelf();
    }
}

unsigned int GetFourCC_6F156560()
{
    return 0x41746c70u;   // 'Atlp'
}

SLocalAbilitySlot_6F241990::SLocalAbilitySlot_6F241990(CAgent* candidate)
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

SLocalAbilitySlot_6F241990* SLocalAbilitySlot_6F241990::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F156560()))
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

void SLocalAbilitySlot_6F241990::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        held->m_refcount += -1;
        if (held->m_refcount == 0)
            held->ReleaseSelf();
    }
}

unsigned int GetFourCC_6F158F50()
{
    return 0x41756e73u;   // 'Auns'
}

SLocalAbilitySlot_6F2420E0::SLocalAbilitySlot_6F2420E0(CAgent* candidate)
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

SLocalAbilitySlot_6F2420E0* SLocalAbilitySlot_6F2420E0::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F158F50()))
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

void SLocalAbilitySlot_6F2420E0::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        held->m_refcount += -1;
        if (held->m_refcount == 0)
            held->ReleaseSelf();
    }
}

unsigned int GetFourCC_6F194F10()
{
    return 0x4155616eu;   // 'AUan'
}

SLocalAbilitySlot_6F242760::SLocalAbilitySlot_6F242760(CAgent* candidate)
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

SLocalAbilitySlot_6F242760* SLocalAbilitySlot_6F242760::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F194F10()))
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

void SLocalAbilitySlot_6F242760::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        held->m_refcount += -1;
        if (held->m_refcount == 0)
            held->ReleaseSelf();
    }
}

unsigned int GetFourCC_6F1102C0()
{
    return 0x416c6f63u;   // 'Aloc'
}

SLocalAbilitySlot_6F245A90::SLocalAbilitySlot_6F245A90(CAgent* candidate)
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

SLocalAbilitySlot_6F245A90* SLocalAbilitySlot_6F245A90::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F1102C0()))
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

void SLocalAbilitySlot_6F245A90::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        held->m_refcount += -1;
        if (held->m_refcount == 0)
            held->ReleaseSelf();
    }
}

unsigned int GetFourCC_6F159920()
{
    return 0x41487765u;   // 'AHwe'
}

SLocalAbilitySlot_6F2478A0::SLocalAbilitySlot_6F2478A0(CAgent* candidate)
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

SLocalAbilitySlot_6F2478A0* SLocalAbilitySlot_6F2478A0::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F159920()))
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

void SLocalAbilitySlot_6F2478A0::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        held->m_refcount += -1;
        if (held->m_refcount == 0)
            held->ReleaseSelf();
    }
}

unsigned int GetFourCC_6F0DCCA0()
{
    return 0x41497369u;   // 'AIsi'
}

SLocalAbilitySlot_6F24A6F0::SLocalAbilitySlot_6F24A6F0(CAgent* candidate)
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

SLocalAbilitySlot_6F24A6F0* SLocalAbilitySlot_6F24A6F0::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetFourCC_6F0DCCA0()))
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

void SLocalAbilitySlot_6F24A6F0::Release()
{
    CAgent* held = m_value;
    if (held)
    {
        held->m_refcount += -1;
        if (held->m_refcount == 0)
            held->ReleaseSelf();
    }
}
