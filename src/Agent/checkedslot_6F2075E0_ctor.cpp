//============================================================================
//  0x6F0D4BE0 - SCheckedSlot_6F2075E0's converting constructor, from the
//  CUnit vtable closure walk (cunit_agent2_worklist.md).  The same
//  three-statement shape every scoped member of the type-checked-slot
//  family has (agenttypedslots.h) - zero m_value, Assign inside a `__try`,
//  Release in a `__finally` guarded by AbnormalTermination() - gated on the
//  raw rawcode 0x6F2075E0 answers ('AHer' MSB-first, `mov eax,41486572h`).
//
//  0x6F2075E0's own Assign body (0x6F0D2DD0) is out of this agent's slice
//  of the parallel CUnit-closure worklist (cunit_agent1_worklist.md has
//  it), so it is only declared here, not defined - named
//  `SCheckedSlot_6F2075E0` by the same "class named after its own
//  fixed-type-id getter's address" convention checkedslot_batch2526.h
//  already established, specifically so an independent reconstruction of
//  0x6F0D2DD0 there links against this constructor without a merge-time
//  rename.
//============================================================================
#include "agent.h"
#include <excpt.h>   // AbnormalTermination()

struct SCheckedSlot_6F2075E0
{
    //  0x6F0D4BE0 - this file.
    SCheckedSlot_6F2075E0(CAgent* candidate);

    ~SCheckedSlot_6F2075E0() { Release(); }

    //  0x6F0D2DD0 - cunit_agent1_worklist.md's, not this file's.
    SCheckedSlot_6F2075E0* Assign(CAgent* candidate);

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            held->m_refcount += -1;
            if (held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};

SCheckedSlot_6F2075E0::SCheckedSlot_6F2075E0(CAgent* candidate)
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

//----------------------------------------------------------------------------
//  Link-completion thunk - 0x6F0D2DD0 is a real, separately-scored
//  reconstruction elsewhere (cunit_agent1's own CAgentPtr_AHer::AssignChecked,
//  tools/funcmap.py), but under a different class name, so the declared
//  SCheckedSlot_6F2075E0::Assign above never resolves against it. Same
//  redirect-thunk pattern as this repo's other out-of-scope callees.
//----------------------------------------------------------------------------
__declspec(naked) SCheckedSlot_6F2075E0* SCheckedSlot_6F2075E0::Assign(CAgent*)
{
    __asm { mov eax, 06F0D2DD0h }
    __asm { jmp eax }
}
