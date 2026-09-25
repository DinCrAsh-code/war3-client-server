//============================================================================
//  Two more CAgentPtr_<tag>::AssignChecked instantiations (agent.h,
//  CAgentPtr::AssignChecked), directly in cunit_agent1_worklist.md:
//  0x6F2755B0 ('AObu', 0x6F0D90A0) and 0x6F2759B0 ('Aupg', 0x6F15C300).
//  Both are the identical instruction stream as CAgentPtr::AssignChecked
//  itself with only the fixed-type leaf differing, the same shape
//  0x6F0D2DD0/0x6F15FF80 (Agent/cunit_agent1_agentptr_hero_ral*.cpp)
//  already are.
//
//  0x6F280DE0 is the matching converting constructor for the 'AObu' slot
//  (CUnit vtable slot 91's own closure - Misc/cunit_agent6_queryfields.cpp's
//  QUERY_FIELD_SLOT(6F29ABA0, ...) still thunks this address under its own
//  private Slot_6F29ABA0 wrapper name, exactly the way it already coexists
//  with this file's own real AssignChecked definition for 0x6F2755B0 - two
//  distinct mangled symbols aimed at the same shipped address, not a
//  conflict). Byte-for-byte CAgentPtr::CAgentPtr (agent.cpp) with the fixed
//  type baked into AssignChecked instead of read from an argument: zero the
//  pointer, then wrap the one virtual call AssignChecked makes (an unknown
//  vtable slot, reached before /GS-/EHs-c- have any say) in a __try/
//  __finally that releases whatever half-assigned pointer is left behind if
//  that call raises a real structured exception. Same unreproducible
//  __except_handler4-shaped SEH frame this repo's fixed /GS- /EHs-c- build
//  cannot emit (docs/msvc-vc8-idioms.md, "An __except_handler4-shaped frame
//  this toolchain cannot reproduce") - past the frame, the body is the
//  identical two-call shape (`m_ptr = 0` then `AssignChecked(obj)`) with
//  the same `mov eax, esi` / `retn 4` tail.
//============================================================================
#include "agent.h"
#include "agiletype.h"
#include <excpt.h>   // AbnormalTermination(), for the converting constructor's __finally

//  0x6F0D90A0 ('AObu') / 0x6F15C300 ('Aupg') - defined in
//  Misc/cunit_agent1_leaf_getters.cpp, a separate TU so they stay real
//  out-of-line calls here rather than being inlined into their one caller
//  each (measured - see that file's own note on this).
extern unsigned int GetFourCC_6F0D90A0();
extern unsigned int GetFourCC_6F15C300();

//  0x6F471910 - agiletype.h.
int __fastcall AgileTypeIsDerivedFrom(unsigned int type, unsigned int base);

struct CAgentPtr_AObu
{
    CAgentPtr_AObu(CAgent* obj);
    CAgentPtr_AObu* AssignChecked(CAgent* obj);
    void Release();
    CAgent* m_ptr;
};
struct CAgentPtr_AUpg
{
    CAgentPtr_AUpg(CAgent* candidate);   //  0x6F2810E0
    CAgentPtr_AUpg* AssignChecked(CAgent* obj);
    CAgent* m_ptr;
};

//  0x6F280DE0 - see this file's own header comment.
CAgentPtr_AObu::CAgentPtr_AObu(CAgent* obj)
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

//  Not its own address - inlined into the __finally above the same way
//  agent.h's own CAgentPtr::Release is, releasing whatever m_ptr already
//  holds if it is non-null.
void CAgentPtr_AObu::Release()
{
    CAgent* held = m_ptr;
    if (held)
    {
        if (--held->m_refcount == 0)
            held->ReleaseSelf();
    }
}

//  0x6F2755B0
CAgentPtr_AObu* CAgentPtr_AObu::AssignChecked(CAgent* obj)
{
    CAgent* checked;
    if (obj && AgileTypeIsDerivedFrom(obj->GetAgileTypeId(), GetFourCC_6F0D90A0()))
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

//  0x6F2810E0 - the converting constructor `CAgentPtr_AUpg(CAgent*)` for
//  this same slot: the ctor called from ResolveAgentQueryField_6F29B220's
//  own QUERY_FIELD_SLOT(6F29B220, 0x6F2810E0, ..., 0x6F2759B0) thunk pair
//  (Misc/cunit_agent6_queryfields.cpp) - that macro still thunks this
//  address (it is below agent-6's own BFS cut) but a real definition now
//  exists here for whichever session next wires the two together.  Same
//  __except_handler4-shaped SEH frame every destructible-local converting
//  constructor in this family carries (docs/msvc-vc8-idioms.md's own
//  section on it, and the CAgentPtr(CAgent*) example there specifically) -
//  unreproducible under this build's fixed /GS- /EHs-c-, so the frame
//  furniture alone caps this well short of EXACT.  Unlike that doc's own
//  sub_6F02F780 example, this instance's __finally is *empty* (its scope
//  table tail-calls nullsub_45, the same no-op shape
//  TSGrowableArray<AGENT_SLOT>::Append's own __finally uses) rather than a
//  conditional Release() - dump-confirmed, not a guess: m_ptr is still 0
//  when AssignChecked can throw, so a conditional Release() would be a
//  no-op here regardless, and the compiler evidently proved that too.
CAgentPtr_AUpg::CAgentPtr_AUpg(CAgent* candidate)
{
    m_ptr = 0;
    __try { AssignChecked(candidate); }
    __finally { }
}

//  0x6F2759B0
CAgentPtr_AUpg* CAgentPtr_AUpg::AssignChecked(CAgent* obj)
{
    CAgent* checked;
    if (obj && AgileTypeIsDerivedFrom(obj->GetAgileTypeId(), GetFourCC_6F15C300()))
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
