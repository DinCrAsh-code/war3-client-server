//============================================================================
//  0x6F2C9670 - SCheckedSelectableSlot's constructor (agenttypedslots.h).
//
//  The '+w3s' member of the same family the twelve JASS slots belong to, and
//  the same three statements: zero the value, Assign inside a `__try`,
//  Release in a `__finally` guarded by AbnormalTermination().  See
//  Agent/checkedunitslot.cpp for the shape and docs/msvc-vc8-idioms.md,
//  "An __except_handler4-shaped frame this toolchain cannot reproduce", for
//  why the frame itself does not come back at this repo's fixed /GS- /EHs-c-.
//
//  Own translation unit, separate from Agent/selectabletypedassign.cpp: the
//  constructor issues a genuine `call sub_6F2C86C0`, so the two must not be
//  in a position to be inlined into each other.
//============================================================================
#include "agenttypedslots.h"
#include <excpt.h>

SCheckedSelectableSlot::SCheckedSelectableSlot(CAgent* candidate)
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
