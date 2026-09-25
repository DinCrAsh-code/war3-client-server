//============================================================================
//  cunit_agent3 batch - the thirty-eight Assign bodies for
//  cunit_agent3_checkedslots.h's own ability-slot family.
//
//  Byte-for-byte the same body as SCheckedItemSlot::Assign (itemtypedassign
//  .cpp) with one operand changed per member - the required FourCC - which
//  is exactly the shape agenttypedslots.h's own JASS-handle family already
//  documents.  MSVC evaluates AgileTypeIsDerivedFrom's arguments right to
//  left, so the type-id getter runs before candidate's own virtual slot 7
//  (GetAgileTypeId) - matching every dump in this family.
//============================================================================
#include "cunit_agent3_checkedslots.h"
#include "cunit_agent3_abilityids.h"
#include "agiletype.h"

//  Two Assigns (SCheckedAbilitySlot_ARal::Assign 0x6F15FF80, SCheckedAbility
//  Slot_Amai::Assign 0x6F275830) are NOT defined here: both addresses
//  belong to a different agent's own worklist slice in this parallel run
//  (agent1's and agent6's respectively - cross-checked against
//  asm/cunit_agent1_worklist.md and asm/cunit_agent6_worklist.md), so
//  cunit_agent3_checkedslots.h only declares them and this file leaves
//  their bodies for those branches.

//  0x6F15FF80 - another agent's own target, declared not defined here.

//  0x6F211B00
SCheckedAbilitySlot_Aast* SCheckedAbilitySlot_Aast::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetAbilityAgileTypeId_Aast()))
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

//  0x6F211F80
SCheckedAbilitySlot_Aadm* SCheckedAbilitySlot_Aadm::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetAbilityAgileTypeId_Aadm()))
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

//  0x6F212400
SCheckedAbilitySlot_Abli* SCheckedAbilitySlot_Abli::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetAbilityAgileTypeId_Abli()))
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

//  0x6F0849F0
SCheckedAbilitySlot_Adro* SCheckedAbilitySlot_Adro::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetAbilityAgileTypeId_Adro()))
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

//  0x6F212A80
SCheckedAbilitySlot_Acmg* SCheckedAbilitySlot_Acmg::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetAbilityAgileTypeId_Acmg()))
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

//  0x6F212D80
SCheckedAbilitySlot_ACrn* SCheckedAbilitySlot_ACrn::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetAbilityAgileTypeId_ACrn()))
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

//  0x6F213280
SCheckedAbilitySlot_Adef* SCheckedAbilitySlot_Adef::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetAbilityAgileTypeId_Adef()))
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

//  0x6F213600
SCheckedAbilitySlot_Adis* SCheckedAbilitySlot_Adis::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetAbilityAgileTypeId_Adis()))
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

//  0x6F213A00
SCheckedAbilitySlot_Aeye* SCheckedAbilitySlot_Aeye::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetAbilityAgileTypeId_Aeye()))
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

//  0x6F214F80
SCheckedAbilitySlot_ANgl* SCheckedAbilitySlot_ANgl::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetAbilityAgileTypeId_ANgl()))
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

//  0x6F216280
SCheckedAbilitySlot_ANsa* SCheckedAbilitySlot_ANsa::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetAbilityAgileTypeId_ANsa()))
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

//  0x6F217600
SCheckedAbilitySlot_Aamk* SCheckedAbilitySlot_Aamk::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetAbilityAgileTypeId_Aamk()))
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

//  0x6F217A00
SCheckedAbilitySlot_AUav* SCheckedAbilitySlot_AUav::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetAbilityAgileTypeId_AUav()))
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

//  0x6F217E00
SCheckedAbilitySlot_AHbz* SCheckedAbilitySlot_AHbz::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetAbilityAgileTypeId_AHbz()))
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

//  0x6F218180
SCheckedAbilitySlot_AOcr* SCheckedAbilitySlot_AOcr::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetAbilityAgileTypeId_AOcr()))
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

//  0x6F218580
SCheckedAbilitySlot_AUin* SCheckedAbilitySlot_AUin::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetAbilityAgileTypeId_AUin()))
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

//  0x6F218980
SCheckedAbilitySlot_AEer* SCheckedAbilitySlot_AEer::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetAbilityAgileTypeId_AEer()))
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

//  0x6F218D80
SCheckedAbilitySlot_AEfn* SCheckedAbilitySlot_AEfn::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetAbilityAgileTypeId_AEfn()))
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

//  0x6F219180
SCheckedAbilitySlot_ANht* SCheckedAbilitySlot_ANht::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetAbilityAgileTypeId_ANht()))
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

//  0x6F219600
SCheckedAbilitySlot_AHmt* SCheckedAbilitySlot_AHmt::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetAbilityAgileTypeId_AHmt()))
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

//  0x6F219A00
SCheckedAbilitySlot_AHre* SCheckedAbilitySlot_AHre::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetAbilityAgileTypeId_AHre()))
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

//  0x6F219E00
SCheckedAbilitySlot_AOsf* SCheckedAbilitySlot_AOsf::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetAbilityAgileTypeId_AOsf()))
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

//  0x6F21A200
SCheckedAbilitySlot_AUts* SCheckedAbilitySlot_AUts::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetAbilityAgileTypeId_AUts()))
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

//  0x6F21A780
SCheckedAbilitySlot_ANwm* SCheckedAbilitySlot_ANwm::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetAbilityAgileTypeId_ANwm()))
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

//  0x6F21AB00
SCheckedAbilitySlot_ANde* SCheckedAbilitySlot_ANde::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetAbilityAgileTypeId_ANde()))
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

//  0x6F21AF00
SCheckedAbilitySlot_ANso* SCheckedAbilitySlot_ANso::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetAbilityAgileTypeId_ANso()))
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

//  0x6F21B300
SCheckedAbilitySlot_AIat* SCheckedAbilitySlot_AIat::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetAbilityAgileTypeId_AIat()))
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

//  0x6F21B700
SCheckedAbilitySlot_AIfe* SCheckedAbilitySlot_AIfe::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetAbilityAgileTypeId_AIfe()))
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

//  0x6F21BB00
SCheckedAbilitySlot_AIfs* SCheckedAbilitySlot_AIfs::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetAbilityAgileTypeId_AIfs()))
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

//  0x6F21BF00
SCheckedAbilitySlot_AIml* SCheckedAbilitySlot_AIml::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetAbilityAgileTypeId_AIml()))
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

//  0x6F21C300
SCheckedAbilitySlot_AIsb* SCheckedAbilitySlot_AIsb::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetAbilityAgileTypeId_AIsb()))
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

//  0x6F21C700
SCheckedAbilitySlot_AIdi* SCheckedAbilitySlot_AIdi::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetAbilityAgileTypeId_AIdi()))
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

//  0x6F21CB00
SCheckedAbilitySlot_AIrt* SCheckedAbilitySlot_AIrt::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetAbilityAgileTypeId_AIrt()))
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

//  0x6F21CF00
SCheckedAbilitySlot_AIte* SCheckedAbilitySlot_AIte::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetAbilityAgileTypeId_AIte()))
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

//  0x6F21D280
SCheckedAbilitySlot_AIgo* SCheckedAbilitySlot_AIgo::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetAbilityAgileTypeId_AIgo()))
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

//  0x6F21D680
SCheckedAbilitySlot_AIbl* SCheckedAbilitySlot_AIbl::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetAbilityAgileTypeId_AIbl()))
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

//  0x6F275830 - another agent's own target, declared not defined here.


//----------------------------------------------------------------------------
//  Link-completion thunks - 0x6F15FF80 / 0x6F275830 are real, separately-
//  scored reconstructions elsewhere (cunit_agent1's own
//  CAgentPtr_ARal::AssignChecked / cunit_agent6's own
//  SCheckedSlot_6F275830::Assign, tools/funcmap.py), but under different
//  class names, so the declarations above never resolve against them. Same
//  redirect-thunk pattern as this repo's other out-of-scope callees.
//----------------------------------------------------------------------------
__declspec(naked) SCheckedAbilitySlot_ARal* SCheckedAbilitySlot_ARal::Assign(CAgent*)
{
    __asm { mov eax, 06F15FF80h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Amai* SCheckedAbilitySlot_Amai::Assign(CAgent*)
{
    __asm { mov eax, 06F275830h }
    __asm { jmp eax }
}
