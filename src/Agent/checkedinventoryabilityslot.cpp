//============================================================================
//  0x6F226520 - SCheckedInventoryAbilitySlot::SCheckedInventoryAbilitySlot
//  (CAgent*).  cunit_agent7 batch (CUnit closure BFS).
//
//  Same shape and same unreproducible frame as checkedattackabilityslot.cpp
//  - see that file's comment.  Called from 0x6F2458F0, one of this batch's
//  own "GetXxxAbility" accessors.
//
//  Assign (0x6F219380, 'AInv' - Inventory) is cunit_agent6's own worklist
//  address: declared in agenttypedslots.h, not defined here.
//============================================================================
#include "agenttypedslots.h"
#include <excpt.h>

SCheckedInventoryAbilitySlot::SCheckedInventoryAbilitySlot(CAgent* candidate)
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
//  Link-completion thunk - 0x6F219380 is a real, separately-scored
//  reconstruction elsewhere (cunit_agent6's own
//  SCheckedInventoryAbilitySlot_6F219380::Assign, tools/funcmap.py), but
//  under a different class name, so the declaration in agenttypedslots.h
//  never resolves against it. Same redirect-thunk pattern as this repo's
//  other out-of-scope callees.
//----------------------------------------------------------------------------
__declspec(naked) SCheckedInventoryAbilitySlot* SCheckedInventoryAbilitySlot::Assign(CAgent*)
{
    __asm { mov eax, 06F219380h }
    __asm { jmp eax }
}
