//============================================================================
//  0x6F16DFC0 - SCheckedSlot_6F027BD0's converting constructor, from the
//  CUnit vtable closure walk (cunit_agent2_worklist.md).  Same shape as
//  checkedslot_6F2075E0_ctor.cpp, gated on 0x6F027BD0's own rawcode ('ARal'
//  MSB-first, `mov eax,4152616Ch`).
//
//  0x6F027BD0's own Assign body (0x6F15FF80) is out of this agent's slice
//  (cunit_agent1_worklist.md has it) - declared, not defined, here, named
//  by the same address-of-its-getter convention for the same reason.
//============================================================================
#include "agent.h"
#include <excpt.h>   // AbnormalTermination()

struct SCheckedSlot_6F027BD0
{
    //  0x6F16DFC0 - this file.
    SCheckedSlot_6F027BD0(CAgent* candidate);

    ~SCheckedSlot_6F027BD0() { Release(); }

    //  0x6F15FF80 - cunit_agent1_worklist.md's, not this file's.
    SCheckedSlot_6F027BD0* Assign(CAgent* candidate);

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

SCheckedSlot_6F027BD0::SCheckedSlot_6F027BD0(CAgent* candidate)
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
//  Link-completion thunk - 0x6F15FF80 is a real, separately-scored
//  reconstruction elsewhere (cunit_agent1's own CAgentPtr_ARal::AssignChecked,
//  tools/funcmap.py), but under a different class name, so the declared
//  SCheckedSlot_6F027BD0::Assign above never resolves against it. A plain
//  redirect thunk - the same `mov eax,<addr>/jmp eax` idiom this repo uses
//  for any out-of-scope callee - makes this TU link without re-claiming
//  the address.
//----------------------------------------------------------------------------
__declspec(naked) SCheckedSlot_6F027BD0* SCheckedSlot_6F027BD0::Assign(CAgent*)
{
    __asm { mov eax, 06F15FF80h }
    __asm { jmp eax }
}
