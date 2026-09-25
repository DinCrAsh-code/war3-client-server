//============================================================================
//  0x6F092A00 - SCheckedAttackAbilitySlot::SCheckedAttackAbilitySlot(CAgent*).
//  cunit_agent7 batch (CUnit closure BFS).
//
//  Same __try/__finally shape as CAgentPtr::CAgentPtr(CAgent*) (agent.cpp)
//  and SCheckedItemSlot::SCheckedItemSlot(CAgent*) (itemtypedassign.cpp),
//  and for the identical reason - see that file's own comment.  The frame
//  itself (push -1 / push offset SEH_6F092A00 / fs:0 chain / non-/GS
//  cookie / __CxxFrameHandler3 trampoline) is the unreproducible
//  __except_handler4-shaped frame docs/msvc-vc8-idioms.md documents; this
//  build's fixed /GS- /EHs-c- cannot emit it with the bundled cl.exe.
//
//  Assign (0x6F084EF0, 'Aatk' - basic Attack) is cunit_agent6's own
//  worklist address: declared in agenttypedslots.h, not defined here.
//============================================================================
#include "agenttypedslots.h"
#include <excpt.h>   // AbnormalTermination()

SCheckedAttackAbilitySlot::SCheckedAttackAbilitySlot(CAgent* candidate)
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
//  Link-completion thunk - 0x6F084EF0 is a real, separately-scored
//  reconstruction elsewhere (cunit_agent6's own
//  SCheckedAttackAbilitySlot_6F084EF0::Assign, tools/funcmap.py), but under
//  a different class name, so the declaration in agenttypedslots.h never
//  resolves against it. Same redirect-thunk pattern as this repo's other
//  out-of-scope callees.
//----------------------------------------------------------------------------
__declspec(naked) SCheckedAttackAbilitySlot* SCheckedAttackAbilitySlot::Assign(CAgent*)
{
    __asm { mov eax, 06F084EF0h }
    __asm { jmp eax }
}
