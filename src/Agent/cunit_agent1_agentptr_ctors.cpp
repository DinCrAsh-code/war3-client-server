//============================================================================
//  CAgentPtr_<tag> converting constructors - CAgentPtr::CAgentPtr(CAgent*)'s
//  own body (agent.cpp), once per fixed type.  Own translation unit, same
//  reasoning as agent.cpp: the __try/__finally wraps the one call
//  (AssignChecked) that can raise a real structured exception through the
//  candidate's vtable, matching the unreproducible __except_handler4-shaped
//  frame docs/msvc-vc8-idioms.md already tracks for this whole family - none
//  of these addresses is separately dumped, so none is scored on its own.
//============================================================================
#include "cunit_agent1_agentptr_family.h"
#include <excpt.h>   // AbnormalTermination()

//  0x6F0922E0
CAgentPtr_Aaha::CAgentPtr_Aaha(CAgent* obj)
{
    m_ptr = 0;
    __try
    {
        AssignChecked(obj);
    }
    __finally
    {
        if (AbnormalTermination())
            Release();
    }
}

//  0x6F220D60
CAgentPtr_Aarm::CAgentPtr_Aarm(CAgent* obj)
{
    m_ptr = 0;
    __try
    {
        AssignChecked(obj);
    }
    __finally
    {
        if (AbnormalTermination())
            Release();
    }
}

//  0x6F2210C0
CAgentPtr_Abrf::CAgentPtr_Abrf(CAgent* obj)
{
    m_ptr = 0;
    __try
    {
        AssignChecked(obj);
    }
    __finally
    {
        if (AbnormalTermination())
            Release();
    }
}

//  0x6F2213C0
CAgentPtr_Abur::CAgentPtr_Abur(CAgent* obj)
{
    m_ptr = 0;
    __try
    {
        AssignChecked(obj);
    }
    __finally
    {
        if (AbnormalTermination())
            Release();
    }
}

//  0x6F221600
CAgentPtr_Achl::CAgentPtr_Achl(CAgent* obj)
{
    m_ptr = 0;
    __try
    {
        AssignChecked(obj);
    }
    __finally
    {
        if (AbnormalTermination())
            Release();
    }
}

//  0x6F2217E0
CAgentPtr_ACad::CAgentPtr_ACad(CAgent* obj)
{
    m_ptr = 0;
    __try
    {
        AssignChecked(obj);
    }
    __finally
    {
        if (AbnormalTermination())
            Release();
    }
}

//  0x6F221AE0
CAgentPtr_Adda::CAgentPtr_Adda(CAgent* obj)
{
    m_ptr = 0;
    __try
    {
        AssignChecked(obj);
    }
    __finally
    {
        if (AbnormalTermination())
            Release();
    }
}

//  0x6F221E40
CAgentPtr_Advc::CAgentPtr_Advc(CAgent* obj)
{
    m_ptr = 0;
    __try
    {
        AssignChecked(obj);
    }
    __finally
    {
        if (AbnormalTermination())
            Release();
    }
}

//  0x6F222140
CAgentPtr_Aetl::CAgentPtr_Aetl(CAgent* obj)
{
    m_ptr = 0;
    __try
    {
        AssignChecked(obj);
    }
    __finally
    {
        if (AbnormalTermination())
            Release();
    }
}

//  0x6F222DA0
CAgentPtr_Adts::CAgentPtr_Adts(CAgent* obj)
{
    m_ptr = 0;
    __try
    {
        AssignChecked(obj);
    }
    __finally
    {
        if (AbnormalTermination())
            Release();
    }
}

//  0x6F2234C0
CAgentPtr_ANpi::CAgentPtr_ANpi(CAgent* obj)
{
    m_ptr = 0;
    __try
    {
        AssignChecked(obj);
    }
    __finally
    {
        if (AbnormalTermination())
            Release();
    }
}

//  0x6F224AE0
CAgentPtr_ANth::CAgentPtr_ANth(CAgent* obj)
{
    m_ptr = 0;
    __try
    {
        AssignChecked(obj);
    }
    __finally
    {
        if (AbnormalTermination())
            Release();
    }
}

//  0x6F225140
CAgentPtr_AEar::CAgentPtr_AEar(CAgent* obj)
{
    m_ptr = 0;
    __try
    {
        AssignChecked(obj);
    }
    __finally
    {
        if (AbnormalTermination())
            Release();
    }
}

//  0x6F225440
CAgentPtr_ANbs::CAgentPtr_ANbs(CAgent* obj)
{
    m_ptr = 0;
    __try
    {
        AssignChecked(obj);
    }
    __finally
    {
        if (AbnormalTermination())
            Release();
    }
}

//  0x6F2256E0
CAgentPtr_ANca::CAgentPtr_ANca(CAgent* obj)
{
    m_ptr = 0;
    __try
    {
        AssignChecked(obj);
    }
    __finally
    {
        if (AbnormalTermination())
            Release();
    }
}

//  0x6F2259E0
CAgentPtr_AUdd::CAgentPtr_AUdd(CAgent* obj)
{
    m_ptr = 0;
    __try
    {
        AssignChecked(obj);
    }
    __finally
    {
        if (AbnormalTermination())
            Release();
    }
}

//  0x6F225CE0
CAgentPtr_AOeq::CAgentPtr_AOeq(CAgent* obj)
{
    m_ptr = 0;
    __try
    {
        AssignChecked(obj);
    }
    __finally
    {
        if (AbnormalTermination())
            Release();
    }
}

//  0x6F225FE0
CAgentPtr_AHfs::CAgentPtr_AHfs(CAgent* obj)
{
    m_ptr = 0;
    __try
    {
        AssignChecked(obj);
    }
    __finally
    {
        if (AbnormalTermination())
            Release();
    }
}

//  0x6F2262E0
CAgentPtr_AOhx::CAgentPtr_AOhx(CAgent* obj)
{
    m_ptr = 0;
    __try
    {
        AssignChecked(obj);
    }
    __finally
    {
        if (AbnormalTermination())
            Release();
    }
}

//  0x6F226640
CAgentPtr_AEmb::CAgentPtr_AEmb(CAgent* obj)
{
    m_ptr = 0;
    __try
    {
        AssignChecked(obj);
    }
    __finally
    {
        if (AbnormalTermination())
            Release();
    }
}

//  0x6F226940
CAgentPtr_ANrf::CAgentPtr_ANrf(CAgent* obj)
{
    m_ptr = 0;
    __try
    {
        AssignChecked(obj);
    }
    __finally
    {
        if (AbnormalTermination())
            Release();
    }
}

//  0x6F226C40
CAgentPtr_ANsl::CAgentPtr_ANsl(CAgent* obj)
{
    m_ptr = 0;
    __try
    {
        AssignChecked(obj);
    }
    __finally
    {
        if (AbnormalTermination())
            Release();
    }
}

//  0x6F226F40
CAgentPtr_ANsq::CAgentPtr_ANsq(CAgent* obj)
{
    m_ptr = 0;
    __try
    {
        AssignChecked(obj);
    }
    __finally
    {
        if (AbnormalTermination())
            Release();
    }
}

//  0x6F227360
CAgentPtr_AOwd::CAgentPtr_AOwd(CAgent* obj)
{
    m_ptr = 0;
    __try
    {
        AssignChecked(obj);
    }
    __finally
    {
        if (AbnormalTermination())
            Release();
    }
}

//  0x6F227660
CAgentPtr_ANtm::CAgentPtr_ANtm(CAgent* obj)
{
    m_ptr = 0;
    __try
    {
        AssignChecked(obj);
    }
    __finally
    {
        if (AbnormalTermination())
            Release();
    }
}

//  0x6F227900
CAgentPtr_ANia::CAgentPtr_ANia(CAgent* obj)
{
    m_ptr = 0;
    __try
    {
        AssignChecked(obj);
    }
    __finally
    {
        if (AbnormalTermination())
            Release();
    }
}

//  0x6F227C00
CAgentPtr_AIim::CAgentPtr_AIim(CAgent* obj)
{
    m_ptr = 0;
    __try
    {
        AssignChecked(obj);
    }
    __finally
    {
        if (AbnormalTermination())
            Release();
    }
}

//  0x6F227F00
CAgentPtr_AIfm::CAgentPtr_AIfm(CAgent* obj)
{
    m_ptr = 0;
    __try
    {
        AssignChecked(obj);
    }
    __finally
    {
        if (AbnormalTermination())
            Release();
    }
}

//  0x6F228200
CAgentPtr_AIfu::CAgentPtr_AIfu(CAgent* obj)
{
    m_ptr = 0;
    __try
    {
        AssignChecked(obj);
    }
    __finally
    {
        if (AbnormalTermination())
            Release();
    }
}

//  0x6F228500
CAgentPtr_AIlb::CAgentPtr_AIlb(CAgent* obj)
{
    m_ptr = 0;
    __try
    {
        AssignChecked(obj);
    }
    __finally
    {
        if (AbnormalTermination())
            Release();
    }
}

//  0x6F228800
CAgentPtr_AIrb::CAgentPtr_AIrb(CAgent* obj)
{
    m_ptr = 0;
    __try
    {
        AssignChecked(obj);
    }
    __finally
    {
        if (AbnormalTermination())
            Release();
    }
}

//  0x6F228B00
CAgentPtr_AIda::CAgentPtr_AIda(CAgent* obj)
{
    m_ptr = 0;
    __try
    {
        AssignChecked(obj);
    }
    __finally
    {
        if (AbnormalTermination())
            Release();
    }
}

//  0x6F228E00
CAgentPtr_AIpm::CAgentPtr_AIpm(CAgent* obj)
{
    m_ptr = 0;
    __try
    {
        AssignChecked(obj);
    }
    __finally
    {
        if (AbnormalTermination())
            Release();
    }
}

//  0x6F229100
CAgentPtr_AIsp::CAgentPtr_AIsp(CAgent* obj)
{
    m_ptr = 0;
    __try
    {
        AssignChecked(obj);
    }
    __finally
    {
        if (AbnormalTermination())
            Release();
    }
}

//  0x6F0EAD40
CAgentPtr_AIfc::CAgentPtr_AIfc(CAgent* obj)
{
    m_ptr = 0;
    __try
    {
        AssignChecked(obj);
    }
    __finally
    {
        if (AbnormalTermination())
            Release();
    }
}

//  0x6F2296A0
CAgentPtr_AIca::CAgentPtr_AIca(CAgent* obj)
{
    m_ptr = 0;
    __try
    {
        AssignChecked(obj);
    }
    __finally
    {
        if (AbnormalTermination())
            Release();
    }
}

//  0x6F280F00
CAgentPtr_ANbu::CAgentPtr_ANbu(CAgent* obj)
{
    m_ptr = 0;
    __try
    {
        AssignChecked(obj);
    }
    __finally
    {
        if (AbnormalTermination())
            Release();
    }
}
