//============================================================================
//  CAgentPtr_<tag> - one instantiation per fixed agile type id of the exact
//  same family CAgentPtr itself is (agent.h): AssignChecked runs the
//  candidate's own GetAgileTypeId() through AgileTypeIsDerivedFrom against a
//  DIFFERENT fixed base per instantiation instead of GetAgileRootTypeId, and
//  the converting constructor is CAgentPtr::CAgentPtr(CAgent*)'s own
//  `m_ptr = 0; __try { AssignChecked(obj); } __finally { if
//  (AbnormalTermination()) Release(); }` verbatim.  The image was linked
//  without /OPT:ICF (agentdefaults.cpp), so identical bodies at distinct
//  addresses stay distinct functions here too - see agenttypedslots.h for
//  the same story told about SCheckedXxxSlot.
//
//  Reached off CUnit's own vtable BFS (cunit_agent1_worklist.md, batch
//  cunit_agent1): each fixed type is a bare FourCC
//  (cunit_agent1_fourcc_getters.cpp) with no recoverable business name in
//  this call tree, so the struct keeps the literal tag in its own name -
//  the same honesty GetFourCC_<addr> already uses for the leaf itself.
//============================================================================
#ifndef CUNIT_AGENT1_AGENTPTR_FAMILY_H
#define CUNIT_AGENT1_AGENTPTR_FAMILY_H

#include "agent.h"

//  0x6F0922E0 / 0x6F084570 - 'Aaha'.
struct CAgentPtr_Aaha
{
    CAgentPtr_Aaha(CAgent* obj);
    ~CAgentPtr_Aaha() { Release(); }

    CAgentPtr_Aaha* AssignChecked(CAgent* obj);

    void Release()
    {
        if (m_ptr)
        {
            if (--m_ptr->m_refcount == 0)
                m_ptr->ReleaseSelf();
        }
    }

    CAgent* m_ptr;    // +0x00
};

//  0x6F220D60 / 0x6F211E80 - 'Aarm'.
struct CAgentPtr_Aarm
{
    CAgentPtr_Aarm(CAgent* obj);
    ~CAgentPtr_Aarm() { Release(); }

    CAgentPtr_Aarm* AssignChecked(CAgent* obj);

    void Release()
    {
        if (m_ptr)
        {
            if (--m_ptr->m_refcount == 0)
                m_ptr->ReleaseSelf();
        }
    }

    CAgent* m_ptr;    // +0x00
};

//  0x6F2210C0 / 0x6F212300 - 'Abrf'.
struct CAgentPtr_Abrf
{
    CAgentPtr_Abrf(CAgent* obj);
    ~CAgentPtr_Abrf() { Release(); }

    CAgentPtr_Abrf* AssignChecked(CAgent* obj);

    void Release()
    {
        if (m_ptr)
        {
            if (--m_ptr->m_refcount == 0)
                m_ptr->ReleaseSelf();
        }
    }

    CAgent* m_ptr;    // +0x00
};

//  0x6F2213C0 / 0x6F212700 - 'Abur'.
struct CAgentPtr_Abur
{
    CAgentPtr_Abur(CAgent* obj);
    ~CAgentPtr_Abur() { Release(); }

    CAgentPtr_Abur* AssignChecked(CAgent* obj);

    void Release()
    {
        if (m_ptr)
        {
            if (--m_ptr->m_refcount == 0)
                m_ptr->ReleaseSelf();
        }
    }

    CAgent* m_ptr;    // +0x00
};

//  0x6F221600 / 0x6F212A00 - 'Achl'.
struct CAgentPtr_Achl
{
    CAgentPtr_Achl(CAgent* obj);
    ~CAgentPtr_Achl() { Release(); }

    CAgentPtr_Achl* AssignChecked(CAgent* obj);

    void Release()
    {
        if (m_ptr)
        {
            if (--m_ptr->m_refcount == 0)
                m_ptr->ReleaseSelf();
        }
    }

    CAgent* m_ptr;    // +0x00
};

//  0x6F2217E0 / 0x6F212C80 - 'ACad'.
struct CAgentPtr_ACad
{
    CAgentPtr_ACad(CAgent* obj);
    ~CAgentPtr_ACad() { Release(); }

    CAgentPtr_ACad* AssignChecked(CAgent* obj);

    void Release()
    {
        if (m_ptr)
        {
            if (--m_ptr->m_refcount == 0)
                m_ptr->ReleaseSelf();
        }
    }

    CAgent* m_ptr;    // +0x00
};

//  0x6F221AE0 / 0x6F213080 - 'Adda'.
struct CAgentPtr_Adda
{
    CAgentPtr_Adda(CAgent* obj);
    ~CAgentPtr_Adda() { Release(); }

    CAgentPtr_Adda* AssignChecked(CAgent* obj);

    void Release()
    {
        if (m_ptr)
        {
            if (--m_ptr->m_refcount == 0)
                m_ptr->ReleaseSelf();
        }
    }

    CAgent* m_ptr;    // +0x00
};

//  0x6F221E40 / 0x6F213500 - 'Advc'.
struct CAgentPtr_Advc
{
    CAgentPtr_Advc(CAgent* obj);
    ~CAgentPtr_Advc() { Release(); }

    CAgentPtr_Advc* AssignChecked(CAgent* obj);

    void Release()
    {
        if (m_ptr)
        {
            if (--m_ptr->m_refcount == 0)
                m_ptr->ReleaseSelf();
        }
    }

    CAgent* m_ptr;    // +0x00
};

//  0x6F222140 / 0x6F213900 - 'Aetl'.
struct CAgentPtr_Aetl
{
    CAgentPtr_Aetl(CAgent* obj);
    ~CAgentPtr_Aetl() { Release(); }

    CAgentPtr_Aetl* AssignChecked(CAgent* obj);

    void Release()
    {
        if (m_ptr)
        {
            if (--m_ptr->m_refcount == 0)
                m_ptr->ReleaseSelf();
        }
    }

    CAgent* m_ptr;    // +0x00
};

//  0x6F222DA0 / 0x6F214980 - 'Adts'.
struct CAgentPtr_Adts
{
    CAgentPtr_Adts(CAgent* obj);
    ~CAgentPtr_Adts() { Release(); }

    CAgentPtr_Adts* AssignChecked(CAgent* obj);

    void Release()
    {
        if (m_ptr)
        {
            if (--m_ptr->m_refcount == 0)
                m_ptr->ReleaseSelf();
        }
    }

    CAgent* m_ptr;    // +0x00
};

//  0x6F2234C0 / 0x6F215300 - 'ANpi'.
struct CAgentPtr_ANpi
{
    CAgentPtr_ANpi(CAgent* obj);
    ~CAgentPtr_ANpi() { Release(); }

    CAgentPtr_ANpi* AssignChecked(CAgent* obj);

    void Release()
    {
        if (m_ptr)
        {
            if (--m_ptr->m_refcount == 0)
                m_ptr->ReleaseSelf();
        }
    }

    CAgent* m_ptr;    // +0x00
};

//  0x6F224AE0 / 0x6F217080 - 'ANth'.
struct CAgentPtr_ANth
{
    CAgentPtr_ANth(CAgent* obj);
    ~CAgentPtr_ANth() { Release(); }

    CAgentPtr_ANth* AssignChecked(CAgent* obj);

    void Release()
    {
        if (m_ptr)
        {
            if (--m_ptr->m_refcount == 0)
                m_ptr->ReleaseSelf();
        }
    }

    CAgent* m_ptr;    // +0x00
};

//  0x6F225140 / 0x6F217900 - 'AEar'.
struct CAgentPtr_AEar
{
    CAgentPtr_AEar(CAgent* obj);
    ~CAgentPtr_AEar() { Release(); }

    CAgentPtr_AEar* AssignChecked(CAgent* obj);

    void Release()
    {
        if (m_ptr)
        {
            if (--m_ptr->m_refcount == 0)
                m_ptr->ReleaseSelf();
        }
    }

    CAgent* m_ptr;    // +0x00
};

//  0x6F225440 / 0x6F217D00 - 'ANbs'.
struct CAgentPtr_ANbs
{
    CAgentPtr_ANbs(CAgent* obj);
    ~CAgentPtr_ANbs() { Release(); }

    CAgentPtr_ANbs* AssignChecked(CAgent* obj);

    void Release()
    {
        if (m_ptr)
        {
            if (--m_ptr->m_refcount == 0)
                m_ptr->ReleaseSelf();
        }
    }

    CAgent* m_ptr;    // +0x00
};

//  0x6F2256E0 / 0x6F218080 - 'ANca'.
struct CAgentPtr_ANca
{
    CAgentPtr_ANca(CAgent* obj);
    ~CAgentPtr_ANca() { Release(); }

    CAgentPtr_ANca* AssignChecked(CAgent* obj);

    void Release()
    {
        if (m_ptr)
        {
            if (--m_ptr->m_refcount == 0)
                m_ptr->ReleaseSelf();
        }
    }

    CAgent* m_ptr;    // +0x00
};

//  0x6F2259E0 / 0x6F218480 - 'AUdd'.
struct CAgentPtr_AUdd
{
    CAgentPtr_AUdd(CAgent* obj);
    ~CAgentPtr_AUdd() { Release(); }

    CAgentPtr_AUdd* AssignChecked(CAgent* obj);

    void Release()
    {
        if (m_ptr)
        {
            if (--m_ptr->m_refcount == 0)
                m_ptr->ReleaseSelf();
        }
    }

    CAgent* m_ptr;    // +0x00
};

//  0x6F225CE0 / 0x6F218880 - 'AOeq'.
struct CAgentPtr_AOeq
{
    CAgentPtr_AOeq(CAgent* obj);
    ~CAgentPtr_AOeq() { Release(); }

    CAgentPtr_AOeq* AssignChecked(CAgent* obj);

    void Release()
    {
        if (m_ptr)
        {
            if (--m_ptr->m_refcount == 0)
                m_ptr->ReleaseSelf();
        }
    }

    CAgent* m_ptr;    // +0x00
};

//  0x6F225FE0 / 0x6F218C80 - 'AHfs'.
struct CAgentPtr_AHfs
{
    CAgentPtr_AHfs(CAgent* obj);
    ~CAgentPtr_AHfs() { Release(); }

    CAgentPtr_AHfs* AssignChecked(CAgent* obj);

    void Release()
    {
        if (m_ptr)
        {
            if (--m_ptr->m_refcount == 0)
                m_ptr->ReleaseSelf();
        }
    }

    CAgent* m_ptr;    // +0x00
};

//  0x6F2262E0 / 0x6F219080 - 'AOhx'.
struct CAgentPtr_AOhx
{
    CAgentPtr_AOhx(CAgent* obj);
    ~CAgentPtr_AOhx() { Release(); }

    CAgentPtr_AOhx* AssignChecked(CAgent* obj);

    void Release()
    {
        if (m_ptr)
        {
            if (--m_ptr->m_refcount == 0)
                m_ptr->ReleaseSelf();
        }
    }

    CAgent* m_ptr;    // +0x00
};

//  0x6F226640 / 0x6F219500 - 'AEmb'.
struct CAgentPtr_AEmb
{
    CAgentPtr_AEmb(CAgent* obj);
    ~CAgentPtr_AEmb() { Release(); }

    CAgentPtr_AEmb* AssignChecked(CAgent* obj);

    void Release()
    {
        if (m_ptr)
        {
            if (--m_ptr->m_refcount == 0)
                m_ptr->ReleaseSelf();
        }
    }

    CAgent* m_ptr;    // +0x00
};

//  0x6F226940 / 0x6F219900 - 'ANrf'.
struct CAgentPtr_ANrf
{
    CAgentPtr_ANrf(CAgent* obj);
    ~CAgentPtr_ANrf() { Release(); }

    CAgentPtr_ANrf* AssignChecked(CAgent* obj);

    void Release()
    {
        if (m_ptr)
        {
            if (--m_ptr->m_refcount == 0)
                m_ptr->ReleaseSelf();
        }
    }

    CAgent* m_ptr;    // +0x00
};

//  0x6F226C40 / 0x6F219D00 - 'ANsl'.
struct CAgentPtr_ANsl
{
    CAgentPtr_ANsl(CAgent* obj);
    ~CAgentPtr_ANsl() { Release(); }

    CAgentPtr_ANsl* AssignChecked(CAgent* obj);

    void Release()
    {
        if (m_ptr)
        {
            if (--m_ptr->m_refcount == 0)
                m_ptr->ReleaseSelf();
        }
    }

    CAgent* m_ptr;    // +0x00
};

//  0x6F226F40 / 0x6F21A100 - 'ANsq'.
struct CAgentPtr_ANsq
{
    CAgentPtr_ANsq(CAgent* obj);
    ~CAgentPtr_ANsq() { Release(); }

    CAgentPtr_ANsq* AssignChecked(CAgent* obj);

    void Release()
    {
        if (m_ptr)
        {
            if (--m_ptr->m_refcount == 0)
                m_ptr->ReleaseSelf();
        }
    }

    CAgent* m_ptr;    // +0x00
};

//  0x6F227360 / 0x6F21A680 - 'AOwd'.
struct CAgentPtr_AOwd
{
    CAgentPtr_AOwd(CAgent* obj);
    ~CAgentPtr_AOwd() { Release(); }

    CAgentPtr_AOwd* AssignChecked(CAgent* obj);

    void Release()
    {
        if (m_ptr)
        {
            if (--m_ptr->m_refcount == 0)
                m_ptr->ReleaseSelf();
        }
    }

    CAgent* m_ptr;    // +0x00
};

//  0x6F227660 / 0x6F21AA80 - 'ANtm'.
struct CAgentPtr_ANtm
{
    CAgentPtr_ANtm(CAgent* obj);
    ~CAgentPtr_ANtm() { Release(); }

    CAgentPtr_ANtm* AssignChecked(CAgent* obj);

    void Release()
    {
        if (m_ptr)
        {
            if (--m_ptr->m_refcount == 0)
                m_ptr->ReleaseSelf();
        }
    }

    CAgent* m_ptr;    // +0x00
};

//  0x6F227900 / 0x6F21AE00 - 'ANia'.
struct CAgentPtr_ANia
{
    CAgentPtr_ANia(CAgent* obj);
    ~CAgentPtr_ANia() { Release(); }

    CAgentPtr_ANia* AssignChecked(CAgent* obj);

    void Release()
    {
        if (m_ptr)
        {
            if (--m_ptr->m_refcount == 0)
                m_ptr->ReleaseSelf();
        }
    }

    CAgent* m_ptr;    // +0x00
};

//  0x6F227C00 / 0x6F21B200 - 'AIim'.
struct CAgentPtr_AIim
{
    CAgentPtr_AIim(CAgent* obj);
    ~CAgentPtr_AIim() { Release(); }

    CAgentPtr_AIim* AssignChecked(CAgent* obj);

    void Release()
    {
        if (m_ptr)
        {
            if (--m_ptr->m_refcount == 0)
                m_ptr->ReleaseSelf();
        }
    }

    CAgent* m_ptr;    // +0x00
};

//  0x6F227F00 / 0x6F21B600 - 'AIfm'.
struct CAgentPtr_AIfm
{
    CAgentPtr_AIfm(CAgent* obj);
    ~CAgentPtr_AIfm() { Release(); }

    CAgentPtr_AIfm* AssignChecked(CAgent* obj);

    void Release()
    {
        if (m_ptr)
        {
            if (--m_ptr->m_refcount == 0)
                m_ptr->ReleaseSelf();
        }
    }

    CAgent* m_ptr;    // +0x00
};

//  0x6F228200 / 0x6F21BA00 - 'AIfu'.
struct CAgentPtr_AIfu
{
    CAgentPtr_AIfu(CAgent* obj);
    ~CAgentPtr_AIfu() { Release(); }

    CAgentPtr_AIfu* AssignChecked(CAgent* obj);

    void Release()
    {
        if (m_ptr)
        {
            if (--m_ptr->m_refcount == 0)
                m_ptr->ReleaseSelf();
        }
    }

    CAgent* m_ptr;    // +0x00
};

//  0x6F228500 / 0x6F21BE00 - 'AIlb'.
struct CAgentPtr_AIlb
{
    CAgentPtr_AIlb(CAgent* obj);
    ~CAgentPtr_AIlb() { Release(); }

    CAgentPtr_AIlb* AssignChecked(CAgent* obj);

    void Release()
    {
        if (m_ptr)
        {
            if (--m_ptr->m_refcount == 0)
                m_ptr->ReleaseSelf();
        }
    }

    CAgent* m_ptr;    // +0x00
};

//  0x6F228800 / 0x6F21C200 - 'AIrb'.
struct CAgentPtr_AIrb
{
    CAgentPtr_AIrb(CAgent* obj);
    ~CAgentPtr_AIrb() { Release(); }

    CAgentPtr_AIrb* AssignChecked(CAgent* obj);

    void Release()
    {
        if (m_ptr)
        {
            if (--m_ptr->m_refcount == 0)
                m_ptr->ReleaseSelf();
        }
    }

    CAgent* m_ptr;    // +0x00
};

//  0x6F228B00 / 0x6F21C600 - 'AIda'.
struct CAgentPtr_AIda
{
    CAgentPtr_AIda(CAgent* obj);
    ~CAgentPtr_AIda() { Release(); }

    CAgentPtr_AIda* AssignChecked(CAgent* obj);

    void Release()
    {
        if (m_ptr)
        {
            if (--m_ptr->m_refcount == 0)
                m_ptr->ReleaseSelf();
        }
    }

    CAgent* m_ptr;    // +0x00
};

//  0x6F228E00 / 0x6F21CA00 - 'AIpm'.
struct CAgentPtr_AIpm
{
    CAgentPtr_AIpm(CAgent* obj);
    ~CAgentPtr_AIpm() { Release(); }

    CAgentPtr_AIpm* AssignChecked(CAgent* obj);

    void Release()
    {
        if (m_ptr)
        {
            if (--m_ptr->m_refcount == 0)
                m_ptr->ReleaseSelf();
        }
    }

    CAgent* m_ptr;    // +0x00
};

//  0x6F229100 / 0x6F21CE00 - 'AIsp'.
struct CAgentPtr_AIsp
{
    CAgentPtr_AIsp(CAgent* obj);
    ~CAgentPtr_AIsp() { Release(); }

    CAgentPtr_AIsp* AssignChecked(CAgent* obj);

    void Release()
    {
        if (m_ptr)
        {
            if (--m_ptr->m_refcount == 0)
                m_ptr->ReleaseSelf();
        }
    }

    CAgent* m_ptr;    // +0x00
};

//  0x6F0EAD40 / 0x6F0E00C0 - 'AIfc'.
struct CAgentPtr_AIfc
{
    CAgentPtr_AIfc(CAgent* obj);
    ~CAgentPtr_AIfc() { Release(); }

    CAgentPtr_AIfc* AssignChecked(CAgent* obj);

    void Release()
    {
        if (m_ptr)
        {
            if (--m_ptr->m_refcount == 0)
                m_ptr->ReleaseSelf();
        }
    }

    CAgent* m_ptr;    // +0x00
};

//  0x6F2296A0 / 0x6F21D580 - 'AIca'.
struct CAgentPtr_AIca
{
    CAgentPtr_AIca(CAgent* obj);
    ~CAgentPtr_AIca() { Release(); }

    CAgentPtr_AIca* AssignChecked(CAgent* obj);

    void Release()
    {
        if (m_ptr)
        {
            if (--m_ptr->m_refcount == 0)
                m_ptr->ReleaseSelf();
        }
    }

    CAgent* m_ptr;    // +0x00
};

//  0x6F280F00 / 0x6F275730 - 'ANbu'.
struct CAgentPtr_ANbu
{
    CAgentPtr_ANbu(CAgent* obj);
    ~CAgentPtr_ANbu() { Release(); }

    CAgentPtr_ANbu* AssignChecked(CAgent* obj);

    void Release()
    {
        if (m_ptr)
        {
            if (--m_ptr->m_refcount == 0)
                m_ptr->ReleaseSelf();
        }
    }

    CAgent* m_ptr;    // +0x00
};

//  0x6F1E5600 / 0x6F1DA890 - 'ACsp'.  A 38th instance, added from a CUnit
//  vtable-closure sweep reached through a different caller
//  (Unit/cunit_agent1_applyabilityeffect.cpp's own SApplyAbilityEffectLeaf)
//  than the other 37's own worklist - same family, same shape.  Its
//  ctor/AssignChecked are two hops out of that sweep's own slice (same
//  "somebody else's reconstruction" reasoning the other 37 already carry
//  for whichever of them isn't yet real), so they stay self-redirect
//  thunks rather than real bodies - see
//  Agent/cunit_agent1_agentptr_acsp_thunks.cpp.
struct CAgentPtr_ACsp
{
    CAgentPtr_ACsp(CAgent* obj);
    ~CAgentPtr_ACsp() { Release(); }

    CAgentPtr_ACsp* AssignChecked(CAgent* obj);

    void Release()
    {
        if (m_ptr)
        {
            if (--m_ptr->m_refcount == 0)
                m_ptr->ReleaseSelf();
        }
    }

    CAgent* m_ptr;    // +0x00
};

#endif