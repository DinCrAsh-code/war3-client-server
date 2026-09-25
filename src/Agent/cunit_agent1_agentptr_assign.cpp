//============================================================================
//  CAgentPtr_<tag>::AssignChecked - CAgentPtr::AssignChecked's own body
//  (agent.cpp), once per fixed type: the candidate's own GetAgileTypeId()
//  checked against this instantiation's own fixed FourCC leaf instead of
//  GetAgileRootTypeId.  None of these addresses is separately dumped in
//  cunit_agent1_worklist.md (they are depth-3 callees of the depth-1/2 make-
//  agent functions in cunit_agent1_makeagent.cpp), so none is scored on its
//  own; each still carries its real address and is transcribed to the same
//  instruction shape AssignChecked already matches exactly, tag swapped.
//============================================================================
#include "cunit_agent1_agentptr_family.h"
#include "agiletype.h"

extern unsigned int GetFourCC_6F021B40();
extern unsigned int GetFourCC_6F022F10();
extern unsigned int GetFourCC_6F196E60();
extern unsigned int GetFourCC_6F1988C0();
extern unsigned int GetFourCC_6F199040();
extern unsigned int GetFourCC_6F194F90();
extern unsigned int GetFourCC_6F1D4640();
extern unsigned int GetFourCC_6F07CCF0();
extern unsigned int GetFourCC_6F07F3B0();
extern unsigned int GetFourCC_6F024A90();
extern unsigned int GetFourCC_6F10E7A0();
extern unsigned int GetFourCC_6F1569A0();
extern unsigned int GetFourCC_6F195E20();
extern unsigned int GetFourCC_6F197260();
extern unsigned int GetFourCC_6F1995C0();
extern unsigned int GetFourCC_6F07BE00();
extern unsigned int GetFourCC_6F07E0F0();
extern unsigned int GetFourCC_6F081130();
extern unsigned int GetFourCC_6F0829F0();
extern unsigned int GetFourCC_6F110CE0();
extern unsigned int GetFourCC_6F197D80();
extern unsigned int GetFourCC_6F195360();
extern unsigned int GetFourCC_6F159CA0();
extern unsigned int GetFourCC_6F159520();
extern unsigned int GetFourCC_6F194980();
extern unsigned int GetFourCC_6F080500();
extern unsigned int GetFourCC_6F0DCBA0();
extern unsigned int GetFourCC_6F0DD6A0();
extern unsigned int GetFourCC_6F0800D0();
extern unsigned int GetFourCC_6F0DBC80();
extern unsigned int GetFourCC_6F0DD4A0();
extern unsigned int GetFourCC_6F07CB10();
extern unsigned int GetFourCC_6F10FB70();
extern unsigned int GetFourCC_6F1D6740();
extern unsigned int GetFourCC_6F0DB5A0();
extern unsigned int GetFourCC_6F0D9E60();
extern unsigned int GetFourCC_6F0D8FA0();

//  0x6F471910 - agiletype.h.
int __fastcall AgileTypeIsDerivedFrom(unsigned int type, unsigned int base);

//  0x6F084570
CAgentPtr_Aaha* CAgentPtr_Aaha::AssignChecked(CAgent* obj)
{
    CAgent* checked;
    if (obj && AgileTypeIsDerivedFrom(obj->GetAgileTypeId(), GetFourCC_6F021B40()))
        checked = obj;
    else
        checked = 0;

    CAgent* old = m_ptr;
    if (old != checked)
    {
        if (old)
        {
            if (--old->m_refcount == 0)
                old->ReleaseSelf();
        }
        if (checked)
        {
            if (obj)
                obj->m_refcount++;
        }
        m_ptr = checked;
    }

    return this;
}

//  0x6F211E80
CAgentPtr_Aarm* CAgentPtr_Aarm::AssignChecked(CAgent* obj)
{
    CAgent* checked;
    if (obj && AgileTypeIsDerivedFrom(obj->GetAgileTypeId(), GetFourCC_6F022F10()))
        checked = obj;
    else
        checked = 0;

    CAgent* old = m_ptr;
    if (old != checked)
    {
        if (old)
        {
            if (--old->m_refcount == 0)
                old->ReleaseSelf();
        }
        if (checked)
        {
            if (obj)
                obj->m_refcount++;
        }
        m_ptr = checked;
    }

    return this;
}

//  0x6F212300
CAgentPtr_Abrf* CAgentPtr_Abrf::AssignChecked(CAgent* obj)
{
    CAgent* checked;
    if (obj && AgileTypeIsDerivedFrom(obj->GetAgileTypeId(), GetFourCC_6F196E60()))
        checked = obj;
    else
        checked = 0;

    CAgent* old = m_ptr;
    if (old != checked)
    {
        if (old)
        {
            if (--old->m_refcount == 0)
                old->ReleaseSelf();
        }
        if (checked)
        {
            if (obj)
                obj->m_refcount++;
        }
        m_ptr = checked;
    }

    return this;
}

//  0x6F212700
CAgentPtr_Abur* CAgentPtr_Abur::AssignChecked(CAgent* obj)
{
    CAgent* checked;
    if (obj && AgileTypeIsDerivedFrom(obj->GetAgileTypeId(), GetFourCC_6F1988C0()))
        checked = obj;
    else
        checked = 0;

    CAgent* old = m_ptr;
    if (old != checked)
    {
        if (old)
        {
            if (--old->m_refcount == 0)
                old->ReleaseSelf();
        }
        if (checked)
        {
            if (obj)
                obj->m_refcount++;
        }
        m_ptr = checked;
    }

    return this;
}

//  0x6F212A00
CAgentPtr_Achl* CAgentPtr_Achl::AssignChecked(CAgent* obj)
{
    CAgent* checked;
    if (obj && AgileTypeIsDerivedFrom(obj->GetAgileTypeId(), GetFourCC_6F199040()))
        checked = obj;
    else
        checked = 0;

    CAgent* old = m_ptr;
    if (old != checked)
    {
        if (old)
        {
            if (--old->m_refcount == 0)
                old->ReleaseSelf();
        }
        if (checked)
        {
            if (obj)
                obj->m_refcount++;
        }
        m_ptr = checked;
    }

    return this;
}

//  0x6F212C80
CAgentPtr_ACad* CAgentPtr_ACad::AssignChecked(CAgent* obj)
{
    CAgent* checked;
    if (obj && AgileTypeIsDerivedFrom(obj->GetAgileTypeId(), GetFourCC_6F194F90()))
        checked = obj;
    else
        checked = 0;

    CAgent* old = m_ptr;
    if (old != checked)
    {
        if (old)
        {
            if (--old->m_refcount == 0)
                old->ReleaseSelf();
        }
        if (checked)
        {
            if (obj)
                obj->m_refcount++;
        }
        m_ptr = checked;
    }

    return this;
}

//  0x6F213080
CAgentPtr_Adda* CAgentPtr_Adda::AssignChecked(CAgent* obj)
{
    CAgent* checked;
    if (obj && AgileTypeIsDerivedFrom(obj->GetAgileTypeId(), GetFourCC_6F1D4640()))
        checked = obj;
    else
        checked = 0;

    CAgent* old = m_ptr;
    if (old != checked)
    {
        if (old)
        {
            if (--old->m_refcount == 0)
                old->ReleaseSelf();
        }
        if (checked)
        {
            if (obj)
                obj->m_refcount++;
        }
        m_ptr = checked;
    }

    return this;
}

//  0x6F213500
CAgentPtr_Advc* CAgentPtr_Advc::AssignChecked(CAgent* obj)
{
    CAgent* checked;
    if (obj && AgileTypeIsDerivedFrom(obj->GetAgileTypeId(), GetFourCC_6F07CCF0()))
        checked = obj;
    else
        checked = 0;

    CAgent* old = m_ptr;
    if (old != checked)
    {
        if (old)
        {
            if (--old->m_refcount == 0)
                old->ReleaseSelf();
        }
        if (checked)
        {
            if (obj)
                obj->m_refcount++;
        }
        m_ptr = checked;
    }

    return this;
}

//  0x6F213900
CAgentPtr_Aetl* CAgentPtr_Aetl::AssignChecked(CAgent* obj)
{
    CAgent* checked;
    if (obj && AgileTypeIsDerivedFrom(obj->GetAgileTypeId(), GetFourCC_6F07F3B0()))
        checked = obj;
    else
        checked = 0;

    CAgent* old = m_ptr;
    if (old != checked)
    {
        if (old)
        {
            if (--old->m_refcount == 0)
                old->ReleaseSelf();
        }
        if (checked)
        {
            if (obj)
                obj->m_refcount++;
        }
        m_ptr = checked;
    }

    return this;
}

//  0x6F214980
CAgentPtr_Adts* CAgentPtr_Adts::AssignChecked(CAgent* obj)
{
    CAgent* checked;
    if (obj && AgileTypeIsDerivedFrom(obj->GetAgileTypeId(), GetFourCC_6F024A90()))
        checked = obj;
    else
        checked = 0;

    CAgent* old = m_ptr;
    if (old != checked)
    {
        if (old)
        {
            if (--old->m_refcount == 0)
                old->ReleaseSelf();
        }
        if (checked)
        {
            if (obj)
                obj->m_refcount++;
        }
        m_ptr = checked;
    }

    return this;
}

//  0x6F215300
CAgentPtr_ANpi* CAgentPtr_ANpi::AssignChecked(CAgent* obj)
{
    CAgent* checked;
    if (obj && AgileTypeIsDerivedFrom(obj->GetAgileTypeId(), GetFourCC_6F10E7A0()))
        checked = obj;
    else
        checked = 0;

    CAgent* old = m_ptr;
    if (old != checked)
    {
        if (old)
        {
            if (--old->m_refcount == 0)
                old->ReleaseSelf();
        }
        if (checked)
        {
            if (obj)
                obj->m_refcount++;
        }
        m_ptr = checked;
    }

    return this;
}

//  0x6F217080
CAgentPtr_ANth* CAgentPtr_ANth::AssignChecked(CAgent* obj)
{
    CAgent* checked;
    if (obj && AgileTypeIsDerivedFrom(obj->GetAgileTypeId(), GetFourCC_6F1569A0()))
        checked = obj;
    else
        checked = 0;

    CAgent* old = m_ptr;
    if (old != checked)
    {
        if (old)
        {
            if (--old->m_refcount == 0)
                old->ReleaseSelf();
        }
        if (checked)
        {
            if (obj)
                obj->m_refcount++;
        }
        m_ptr = checked;
    }

    return this;
}

//  0x6F217900
CAgentPtr_AEar* CAgentPtr_AEar::AssignChecked(CAgent* obj)
{
    CAgent* checked;
    if (obj && AgileTypeIsDerivedFrom(obj->GetAgileTypeId(), GetFourCC_6F195E20()))
        checked = obj;
    else
        checked = 0;

    CAgent* old = m_ptr;
    if (old != checked)
    {
        if (old)
        {
            if (--old->m_refcount == 0)
                old->ReleaseSelf();
        }
        if (checked)
        {
            if (obj)
                obj->m_refcount++;
        }
        m_ptr = checked;
    }

    return this;
}

//  0x6F217D00
CAgentPtr_ANbs* CAgentPtr_ANbs::AssignChecked(CAgent* obj)
{
    CAgent* checked;
    if (obj && AgileTypeIsDerivedFrom(obj->GetAgileTypeId(), GetFourCC_6F197260()))
        checked = obj;
    else
        checked = 0;

    CAgent* old = m_ptr;
    if (old != checked)
    {
        if (old)
        {
            if (--old->m_refcount == 0)
                old->ReleaseSelf();
        }
        if (checked)
        {
            if (obj)
                obj->m_refcount++;
        }
        m_ptr = checked;
    }

    return this;
}

//  0x6F218080
CAgentPtr_ANca* CAgentPtr_ANca::AssignChecked(CAgent* obj)
{
    CAgent* checked;
    if (obj && AgileTypeIsDerivedFrom(obj->GetAgileTypeId(), GetFourCC_6F1995C0()))
        checked = obj;
    else
        checked = 0;

    CAgent* old = m_ptr;
    if (old != checked)
    {
        if (old)
        {
            if (--old->m_refcount == 0)
                old->ReleaseSelf();
        }
        if (checked)
        {
            if (obj)
                obj->m_refcount++;
        }
        m_ptr = checked;
    }

    return this;
}

//  0x6F218480
CAgentPtr_AUdd* CAgentPtr_AUdd::AssignChecked(CAgent* obj)
{
    CAgent* checked;
    if (obj && AgileTypeIsDerivedFrom(obj->GetAgileTypeId(), GetFourCC_6F07BE00()))
        checked = obj;
    else
        checked = 0;

    CAgent* old = m_ptr;
    if (old != checked)
    {
        if (old)
        {
            if (--old->m_refcount == 0)
                old->ReleaseSelf();
        }
        if (checked)
        {
            if (obj)
                obj->m_refcount++;
        }
        m_ptr = checked;
    }

    return this;
}

//  0x6F218880
CAgentPtr_AOeq* CAgentPtr_AOeq::AssignChecked(CAgent* obj)
{
    CAgent* checked;
    if (obj && AgileTypeIsDerivedFrom(obj->GetAgileTypeId(), GetFourCC_6F07E0F0()))
        checked = obj;
    else
        checked = 0;

    CAgent* old = m_ptr;
    if (old != checked)
    {
        if (old)
        {
            if (--old->m_refcount == 0)
                old->ReleaseSelf();
        }
        if (checked)
        {
            if (obj)
                obj->m_refcount++;
        }
        m_ptr = checked;
    }

    return this;
}

//  0x6F218C80
CAgentPtr_AHfs* CAgentPtr_AHfs::AssignChecked(CAgent* obj)
{
    CAgent* checked;
    if (obj && AgileTypeIsDerivedFrom(obj->GetAgileTypeId(), GetFourCC_6F081130()))
        checked = obj;
    else
        checked = 0;

    CAgent* old = m_ptr;
    if (old != checked)
    {
        if (old)
        {
            if (--old->m_refcount == 0)
                old->ReleaseSelf();
        }
        if (checked)
        {
            if (obj)
                obj->m_refcount++;
        }
        m_ptr = checked;
    }

    return this;
}

//  0x6F219080
CAgentPtr_AOhx* CAgentPtr_AOhx::AssignChecked(CAgent* obj)
{
    CAgent* checked;
    if (obj && AgileTypeIsDerivedFrom(obj->GetAgileTypeId(), GetFourCC_6F0829F0()))
        checked = obj;
    else
        checked = 0;

    CAgent* old = m_ptr;
    if (old != checked)
    {
        if (old)
        {
            if (--old->m_refcount == 0)
                old->ReleaseSelf();
        }
        if (checked)
        {
            if (obj)
                obj->m_refcount++;
        }
        m_ptr = checked;
    }

    return this;
}

//  0x6F219500
CAgentPtr_AEmb* CAgentPtr_AEmb::AssignChecked(CAgent* obj)
{
    CAgent* checked;
    if (obj && AgileTypeIsDerivedFrom(obj->GetAgileTypeId(), GetFourCC_6F110CE0()))
        checked = obj;
    else
        checked = 0;

    CAgent* old = m_ptr;
    if (old != checked)
    {
        if (old)
        {
            if (--old->m_refcount == 0)
                old->ReleaseSelf();
        }
        if (checked)
        {
            if (obj)
                obj->m_refcount++;
        }
        m_ptr = checked;
    }

    return this;
}

//  0x6F219900
CAgentPtr_ANrf* CAgentPtr_ANrf::AssignChecked(CAgent* obj)
{
    CAgent* checked;
    if (obj && AgileTypeIsDerivedFrom(obj->GetAgileTypeId(), GetFourCC_6F197D80()))
        checked = obj;
    else
        checked = 0;

    CAgent* old = m_ptr;
    if (old != checked)
    {
        if (old)
        {
            if (--old->m_refcount == 0)
                old->ReleaseSelf();
        }
        if (checked)
        {
            if (obj)
                obj->m_refcount++;
        }
        m_ptr = checked;
    }

    return this;
}

//  0x6F219D00
CAgentPtr_ANsl* CAgentPtr_ANsl::AssignChecked(CAgent* obj)
{
    CAgent* checked;
    if (obj && AgileTypeIsDerivedFrom(obj->GetAgileTypeId(), GetFourCC_6F195360()))
        checked = obj;
    else
        checked = 0;

    CAgent* old = m_ptr;
    if (old != checked)
    {
        if (old)
        {
            if (--old->m_refcount == 0)
                old->ReleaseSelf();
        }
        if (checked)
        {
            if (obj)
                obj->m_refcount++;
        }
        m_ptr = checked;
    }

    return this;
}

//  0x6F21A100
CAgentPtr_ANsq* CAgentPtr_ANsq::AssignChecked(CAgent* obj)
{
    CAgent* checked;
    if (obj && AgileTypeIsDerivedFrom(obj->GetAgileTypeId(), GetFourCC_6F159CA0()))
        checked = obj;
    else
        checked = 0;

    CAgent* old = m_ptr;
    if (old != checked)
    {
        if (old)
        {
            if (--old->m_refcount == 0)
                old->ReleaseSelf();
        }
        if (checked)
        {
            if (obj)
                obj->m_refcount++;
        }
        m_ptr = checked;
    }

    return this;
}

//  0x6F21A680
CAgentPtr_AOwd* CAgentPtr_AOwd::AssignChecked(CAgent* obj)
{
    CAgent* checked;
    if (obj && AgileTypeIsDerivedFrom(obj->GetAgileTypeId(), GetFourCC_6F159520()))
        checked = obj;
    else
        checked = 0;

    CAgent* old = m_ptr;
    if (old != checked)
    {
        if (old)
        {
            if (--old->m_refcount == 0)
                old->ReleaseSelf();
        }
        if (checked)
        {
            if (obj)
                obj->m_refcount++;
        }
        m_ptr = checked;
    }

    return this;
}

//  0x6F21AA80
CAgentPtr_ANtm* CAgentPtr_ANtm::AssignChecked(CAgent* obj)
{
    CAgent* checked;
    if (obj && AgileTypeIsDerivedFrom(obj->GetAgileTypeId(), GetFourCC_6F194980()))
        checked = obj;
    else
        checked = 0;

    CAgent* old = m_ptr;
    if (old != checked)
    {
        if (old)
        {
            if (--old->m_refcount == 0)
                old->ReleaseSelf();
        }
        if (checked)
        {
            if (obj)
                obj->m_refcount++;
        }
        m_ptr = checked;
    }

    return this;
}

//  0x6F21AE00
CAgentPtr_ANia* CAgentPtr_ANia::AssignChecked(CAgent* obj)
{
    CAgent* checked;
    if (obj && AgileTypeIsDerivedFrom(obj->GetAgileTypeId(), GetFourCC_6F080500()))
        checked = obj;
    else
        checked = 0;

    CAgent* old = m_ptr;
    if (old != checked)
    {
        if (old)
        {
            if (--old->m_refcount == 0)
                old->ReleaseSelf();
        }
        if (checked)
        {
            if (obj)
                obj->m_refcount++;
        }
        m_ptr = checked;
    }

    return this;
}

//  0x6F21B200
CAgentPtr_AIim* CAgentPtr_AIim::AssignChecked(CAgent* obj)
{
    CAgent* checked;
    if (obj && AgileTypeIsDerivedFrom(obj->GetAgileTypeId(), GetFourCC_6F0DCBA0()))
        checked = obj;
    else
        checked = 0;

    CAgent* old = m_ptr;
    if (old != checked)
    {
        if (old)
        {
            if (--old->m_refcount == 0)
                old->ReleaseSelf();
        }
        if (checked)
        {
            if (obj)
                obj->m_refcount++;
        }
        m_ptr = checked;
    }

    return this;
}

//  0x6F21B600
CAgentPtr_AIfm* CAgentPtr_AIfm::AssignChecked(CAgent* obj)
{
    CAgent* checked;
    if (obj && AgileTypeIsDerivedFrom(obj->GetAgileTypeId(), GetFourCC_6F0DD6A0()))
        checked = obj;
    else
        checked = 0;

    CAgent* old = m_ptr;
    if (old != checked)
    {
        if (old)
        {
            if (--old->m_refcount == 0)
                old->ReleaseSelf();
        }
        if (checked)
        {
            if (obj)
                obj->m_refcount++;
        }
        m_ptr = checked;
    }

    return this;
}

//  0x6F21BA00
CAgentPtr_AIfu* CAgentPtr_AIfu::AssignChecked(CAgent* obj)
{
    CAgent* checked;
    if (obj && AgileTypeIsDerivedFrom(obj->GetAgileTypeId(), GetFourCC_6F0800D0()))
        checked = obj;
    else
        checked = 0;

    CAgent* old = m_ptr;
    if (old != checked)
    {
        if (old)
        {
            if (--old->m_refcount == 0)
                old->ReleaseSelf();
        }
        if (checked)
        {
            if (obj)
                obj->m_refcount++;
        }
        m_ptr = checked;
    }

    return this;
}

//  0x6F21BE00
CAgentPtr_AIlb* CAgentPtr_AIlb::AssignChecked(CAgent* obj)
{
    CAgent* checked;
    if (obj && AgileTypeIsDerivedFrom(obj->GetAgileTypeId(), GetFourCC_6F0DBC80()))
        checked = obj;
    else
        checked = 0;

    CAgent* old = m_ptr;
    if (old != checked)
    {
        if (old)
        {
            if (--old->m_refcount == 0)
                old->ReleaseSelf();
        }
        if (checked)
        {
            if (obj)
                obj->m_refcount++;
        }
        m_ptr = checked;
    }

    return this;
}

//  0x6F21C200
CAgentPtr_AIrb* CAgentPtr_AIrb::AssignChecked(CAgent* obj)
{
    CAgent* checked;
    if (obj && AgileTypeIsDerivedFrom(obj->GetAgileTypeId(), GetFourCC_6F0DD4A0()))
        checked = obj;
    else
        checked = 0;

    CAgent* old = m_ptr;
    if (old != checked)
    {
        if (old)
        {
            if (--old->m_refcount == 0)
                old->ReleaseSelf();
        }
        if (checked)
        {
            if (obj)
                obj->m_refcount++;
        }
        m_ptr = checked;
    }

    return this;
}

//  0x6F21C600
CAgentPtr_AIda* CAgentPtr_AIda::AssignChecked(CAgent* obj)
{
    CAgent* checked;
    if (obj && AgileTypeIsDerivedFrom(obj->GetAgileTypeId(), GetFourCC_6F07CB10()))
        checked = obj;
    else
        checked = 0;

    CAgent* old = m_ptr;
    if (old != checked)
    {
        if (old)
        {
            if (--old->m_refcount == 0)
                old->ReleaseSelf();
        }
        if (checked)
        {
            if (obj)
                obj->m_refcount++;
        }
        m_ptr = checked;
    }

    return this;
}

//  0x6F21CA00
CAgentPtr_AIpm* CAgentPtr_AIpm::AssignChecked(CAgent* obj)
{
    CAgent* checked;
    if (obj && AgileTypeIsDerivedFrom(obj->GetAgileTypeId(), GetFourCC_6F10FB70()))
        checked = obj;
    else
        checked = 0;

    CAgent* old = m_ptr;
    if (old != checked)
    {
        if (old)
        {
            if (--old->m_refcount == 0)
                old->ReleaseSelf();
        }
        if (checked)
        {
            if (obj)
                obj->m_refcount++;
        }
        m_ptr = checked;
    }

    return this;
}

//  0x6F21CE00
CAgentPtr_AIsp* CAgentPtr_AIsp::AssignChecked(CAgent* obj)
{
    CAgent* checked;
    if (obj && AgileTypeIsDerivedFrom(obj->GetAgileTypeId(), GetFourCC_6F1D6740()))
        checked = obj;
    else
        checked = 0;

    CAgent* old = m_ptr;
    if (old != checked)
    {
        if (old)
        {
            if (--old->m_refcount == 0)
                old->ReleaseSelf();
        }
        if (checked)
        {
            if (obj)
                obj->m_refcount++;
        }
        m_ptr = checked;
    }

    return this;
}

//  0x6F0E00C0
CAgentPtr_AIfc* CAgentPtr_AIfc::AssignChecked(CAgent* obj)
{
    CAgent* checked;
    if (obj && AgileTypeIsDerivedFrom(obj->GetAgileTypeId(), GetFourCC_6F0DB5A0()))
        checked = obj;
    else
        checked = 0;

    CAgent* old = m_ptr;
    if (old != checked)
    {
        if (old)
        {
            if (--old->m_refcount == 0)
                old->ReleaseSelf();
        }
        if (checked)
        {
            if (obj)
                obj->m_refcount++;
        }
        m_ptr = checked;
    }

    return this;
}

//  0x6F21D580
CAgentPtr_AIca* CAgentPtr_AIca::AssignChecked(CAgent* obj)
{
    CAgent* checked;
    if (obj && AgileTypeIsDerivedFrom(obj->GetAgileTypeId(), GetFourCC_6F0D9E60()))
        checked = obj;
    else
        checked = 0;

    CAgent* old = m_ptr;
    if (old != checked)
    {
        if (old)
        {
            if (--old->m_refcount == 0)
                old->ReleaseSelf();
        }
        if (checked)
        {
            if (obj)
                obj->m_refcount++;
        }
        m_ptr = checked;
    }

    return this;
}

//  0x6F275730 turned out to already be cunit_agent4's own worklist item,
//  properly claimed and scored there as SAgent4TypeCheckedRefHolder_6F275730
//  ::Set (tools/funcmap.py). This definition carries no funcmap row of its
//  own and exists purely so MakeCheckedAgent_ANbu (cunit_agent1_makeagent.cpp)
//  compiles - same reasoning as cunit_agent1_agentptr_hero_ctor.cpp's own
//  note about 0x6F0D4BE0/cunit_agent2.
CAgentPtr_ANbu* CAgentPtr_ANbu::AssignChecked(CAgent* obj)
{
    CAgent* checked;
    if (obj && AgileTypeIsDerivedFrom(obj->GetAgileTypeId(), GetFourCC_6F0D8FA0()))
        checked = obj;
    else
        checked = 0;

    CAgent* old = m_ptr;
    if (old != checked)
    {
        if (old)
        {
            if (--old->m_refcount == 0)
                old->ReleaseSelf();
        }
        if (checked)
        {
            if (obj)
                obj->m_refcount++;
        }
        m_ptr = checked;
    }

    return this;
}
