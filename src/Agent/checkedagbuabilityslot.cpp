//============================================================================
//  0x6F280EA0 - SCheckedAGbuAbilitySlot::SCheckedAGbuAbilitySlot(CAgent*).
//  cunit_agent7 batch (CUnit closure BFS).
//
//  Same shape and same unreproducible frame as checkedattackabilityslot.cpp
//  - see that file's comment.  Called from 0x6F29AD40 (outside this batch).
//
//  Assign (0x6F2756B0, 'AGbu' - no readable meaning, kept literal) is
//  cunit_agent3's own worklist address: declared in agenttypedslots.h, not
//  defined here.
//============================================================================
#include "agenttypedslots.h"
#include <excpt.h>

SCheckedAGbuAbilitySlot::SCheckedAGbuAbilitySlot(CAgent* candidate)
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
//  Link-completion thunk - 0x6F2756B0 is a real, separately-scored
//  reconstruction elsewhere (cunit_agent3's own SCheckedAbilitySlot_AGbu::
//  Assign, tools/funcmap.py), but under a different class name, so the
//  declaration in agenttypedslots.h never resolves against it. Same
//  redirect-thunk pattern as this repo's other out-of-scope callees.
//----------------------------------------------------------------------------
__declspec(naked) SCheckedAGbuAbilitySlot* SCheckedAGbuAbilitySlot::Assign(CAgent*)
{
    __asm { mov eax, 06F2756B0h }
    __asm { jmp eax }
}
