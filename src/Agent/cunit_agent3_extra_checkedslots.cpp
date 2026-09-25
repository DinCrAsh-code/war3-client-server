//============================================================================
//  0x6F0DF6C0 / 0x6F2756B0 - two more Assign bodies from CUnit's own
//  closure, same shape as cunit_agent3_checkedslots_assign.cpp's own family
//  and SCheckedItemSlot::Assign (itemtypedassign.cpp) - "byte-for-byte the
//  same body ... with one operand changed" - but reached here directly
//  (a candidate handed in from outside, `arg_0`), with no matching ctor or
//  outer "make one" wrapper anywhere in this call tree.
//============================================================================
#include "cunit_agent3_extra_checkedslots.h"
#include "agiletype.h"

//  0x6F0DF6C0
SCheckedAbilitySlot_Amov* SCheckedAbilitySlot_Amov::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetAbilityAgileTypeId_Amov()))
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

//  0x6F2756B0
SCheckedAbilitySlot_AGbu* SCheckedAbilitySlot_AGbu::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetAbilityAgileTypeId_AGbu()))
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

//----------------------------------------------------------------------------
//  Link-completion thunks - 0x6F200960 / 0x6F0D9220 are real, separately-
//  scored reconstructions elsewhere (cunit_agent2's own
//  GetMoveAbilityRawcode / cunit_agent1's own GetFourCC_6F0D9220,
//  tools/funcmap.py), but under different names, so the declarations in
//  cunit_agent3_extra_checkedslots.h never resolve against them. Same
//  redirect-thunk pattern as this repo's other out-of-scope callees.
//----------------------------------------------------------------------------
__declspec(naked) unsigned int GetAbilityAgileTypeId_Amov()
{
    __asm { mov eax, 06F200960h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetAbilityAgileTypeId_AGbu()
{
    __asm { mov eax, 06F0D9220h }
    __asm { jmp eax }
}
