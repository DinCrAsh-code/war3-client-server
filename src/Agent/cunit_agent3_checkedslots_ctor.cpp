//============================================================================
//  cunit_agent3 batch - the thirty-eight constructors for
//  cunit_agent3_checkedslots.h's own ability-slot family.
//
//  Own translation unit, separate from cunit_agent3_checkedslots_assign.cpp:
//  each constructor issues a genuine `call` to its own type's Assign, so the
//  two must not be in a position to be inlined into each other - the same
//  split checkedlocationslot.cpp/locationtypedassign.cpp already keep for
//  their own single pair, done here for all thirty-eight at once.
//
//  Same __try/__finally shape as every other converting constructor in this
//  family (SCheckedLocationSlot::SCheckedLocationSlot, checkedlocationslot
//  .cpp): Assign's virtual GetAgileTypeId() call through `candidate` is the
//  one thing here that can raise, and the frame releases whatever
//  half-assigned value is left if it does.
//============================================================================
#include "cunit_agent3_checkedslots.h"
#include <excpt.h>   // AbnormalTermination()

//  Two constructors (SCheckedAbilitySlot_ARal 0x6F16DFC0, SCheckedAbilitySlot_
//  Amai 0x6F280FC0) are NOT defined here: both addresses are agent2's own
//  worklist targets in this parallel run (cross-checked against
//  asm/cunit_agent2_worklist.md), so cunit_agent3_checkedslots.h only
//  declares them and this file leaves their bodies for that branch.

//  0x6F16DFC0 - agent2's own target, declared not defined here.

//  0x6F220AC0
SCheckedAbilitySlot_Aast::SCheckedAbilitySlot_Aast(CAgent* candidate)
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

//  0x6F220E20
SCheckedAbilitySlot_Aadm::SCheckedAbilitySlot_Aadm(CAgent* candidate)
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

//  0x6F221180
SCheckedAbilitySlot_Abli::SCheckedAbilitySlot_Abli(CAgent* candidate)
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

//  0x6F092640
SCheckedAbilitySlot_Adro::SCheckedAbilitySlot_Adro(CAgent* candidate)
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

//  0x6F221660
SCheckedAbilitySlot_Acmg::SCheckedAbilitySlot_Acmg(CAgent* candidate)
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

//  0x6F2218A0
SCheckedAbilitySlot_ACrn::SCheckedAbilitySlot_ACrn(CAgent* candidate)
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

//  0x6F221C60
SCheckedAbilitySlot_Adef::SCheckedAbilitySlot_Adef(CAgent* candidate)
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

//  0x6F221F00
SCheckedAbilitySlot_Adis::SCheckedAbilitySlot_Adis(CAgent* candidate)
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

//  0x6F222200
SCheckedAbilitySlot_Aeye::SCheckedAbilitySlot_Aeye(CAgent* candidate)
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

//  0x6F223220
SCheckedAbilitySlot_ANgl::SCheckedAbilitySlot_ANgl(CAgent* candidate)
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

//  0x6F224060
SCheckedAbilitySlot_ANsa::SCheckedAbilitySlot_ANsa(CAgent* candidate)
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

//  0x6F224F00
SCheckedAbilitySlot_Aamk::SCheckedAbilitySlot_Aamk(CAgent* candidate)
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

//  0x6F225200
SCheckedAbilitySlot_AUav::SCheckedAbilitySlot_AUav(CAgent* candidate)
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

//  0x6F225500
SCheckedAbilitySlot_AHbz::SCheckedAbilitySlot_AHbz(CAgent* candidate)
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

//  0x6F2257A0
SCheckedAbilitySlot_AOcr::SCheckedAbilitySlot_AOcr(CAgent* candidate)
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

//  0x6F225AA0
SCheckedAbilitySlot_AUin::SCheckedAbilitySlot_AUin(CAgent* candidate)
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

//  0x6F225DA0
SCheckedAbilitySlot_AEer::SCheckedAbilitySlot_AEer(CAgent* candidate)
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

//  0x6F2260A0
SCheckedAbilitySlot_AEfn::SCheckedAbilitySlot_AEfn(CAgent* candidate)
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

//  0x6F2263A0
SCheckedAbilitySlot_ANht::SCheckedAbilitySlot_ANht(CAgent* candidate)
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

//  0x6F226700
SCheckedAbilitySlot_AHmt::SCheckedAbilitySlot_AHmt(CAgent* candidate)
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

//  0x6F226A00
SCheckedAbilitySlot_AHre::SCheckedAbilitySlot_AHre(CAgent* candidate)
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

//  0x6F226D00
SCheckedAbilitySlot_AOsf::SCheckedAbilitySlot_AOsf(CAgent* candidate)
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

//  0x6F227000
SCheckedAbilitySlot_AUts::SCheckedAbilitySlot_AUts(CAgent* candidate)
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

//  0x6F227420
SCheckedAbilitySlot_ANwm::SCheckedAbilitySlot_ANwm(CAgent* candidate)
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

//  0x6F2276C0
SCheckedAbilitySlot_ANde::SCheckedAbilitySlot_ANde(CAgent* candidate)
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

//  0x6F2279C0
SCheckedAbilitySlot_ANso::SCheckedAbilitySlot_ANso(CAgent* candidate)
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

//  0x6F227CC0
SCheckedAbilitySlot_AIat::SCheckedAbilitySlot_AIat(CAgent* candidate)
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

//  0x6F227FC0
SCheckedAbilitySlot_AIfe::SCheckedAbilitySlot_AIfe(CAgent* candidate)
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

//  0x6F2282C0
SCheckedAbilitySlot_AIfs::SCheckedAbilitySlot_AIfs(CAgent* candidate)
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

//  0x6F2285C0
SCheckedAbilitySlot_AIml::SCheckedAbilitySlot_AIml(CAgent* candidate)
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

//  0x6F2288C0
SCheckedAbilitySlot_AIsb::SCheckedAbilitySlot_AIsb(CAgent* candidate)
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

//  0x6F228BC0
SCheckedAbilitySlot_AIdi::SCheckedAbilitySlot_AIdi(CAgent* candidate)
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

//  0x6F228EC0
SCheckedAbilitySlot_AIrt::SCheckedAbilitySlot_AIrt(CAgent* candidate)
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

//  0x6F2291C0
SCheckedAbilitySlot_AIte::SCheckedAbilitySlot_AIte(CAgent* candidate)
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

//  0x6F229460
SCheckedAbilitySlot_AIgo::SCheckedAbilitySlot_AIgo(CAgent* candidate)
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

//  0x6F229760
SCheckedAbilitySlot_AIbl::SCheckedAbilitySlot_AIbl(CAgent* candidate)
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

//  0x6F280FC0 - agent2's own target, declared not defined here.


//----------------------------------------------------------------------------
//  Link-completion thunks - 0x6F16DFC0 / 0x6F280FC0 are real, separately-
//  scored reconstructions elsewhere (cunit_agent2's own
//  SCheckedSlot_6F027BD0::SCheckedSlot_6F027BD0 /
//  SCheckedSlot_6F028940::SCheckedSlot_6F028940, tools/funcmap.py), but
//  under different class names, so the declarations above never resolve
//  against them. Same redirect-thunk pattern as this repo's other
//  out-of-scope callees.
//----------------------------------------------------------------------------
__declspec(naked) SCheckedAbilitySlot_ARal::SCheckedAbilitySlot_ARal(CAgent*)
{
    __asm { mov eax, 06F16DFC0h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Amai::SCheckedAbilitySlot_Amai(CAgent*)
{
    __asm { mov eax, 06F280FC0h }
    __asm { jmp eax }
}
