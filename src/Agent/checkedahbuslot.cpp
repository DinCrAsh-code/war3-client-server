//============================================================================
//  0x6F2754B0 - SCheckedAHbuSlot::Assign.  cunit_agent7 batch (CUnit closure
//  BFS).  See agenttypedslots.h and itemtypedassign.cpp.
//
//  The required type id comes from a `call` to 0x6F0D9020
//  (`mov eax, 41486275h / retn`, "AHbu" MSB first) - cunit_agent5's own
//  worklist address, so it is NOT defined here.  This TU's own copy
//  (GetAHbuTypeTag, checkedslot_agent7_leaves.cpp) is a second,
//  internal-to-this-batch instance of the same constant - harmless, since
//  the leaf has no observable state and `canonlib` canonicalises any
//  resolved call target to `call SYM` either way - but it has to live in
//  its own TU, not `static` in this one, or this build's /Ob2 inlines the
//  one-instruction leaf away and loses the dump's own out-of-line `call`
//  (see that file's own header for the score this cost before the fix).
//============================================================================
#include "agenttypedslots.h"
#include "agiletype.h"

extern unsigned int GetAHbuTypeTag();

SCheckedAHbuSlot* SCheckedAHbuSlot::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetAHbuTypeTag()))
        checked = candidate;
    else
        checked = 0;

    CAgent* old = m_value;
    if (old != checked)
    {
        if (old)
        {
            if (--old->m_refcount == 0)
                old->ReleaseSelf();
        }
        if (checked)
        {
            if (candidate)
                candidate->m_refcount++;
        }
        m_value = checked;
    }

    return this;
}
