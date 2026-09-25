//============================================================================
//  0x6F0755F0 - SCheckedAAbtSlot::Assign.  cunit_agent7 batch (CUnit closure
//  BFS).  See agenttypedslots.h for the shape and itemtypedassign.cpp for
//  why it is spelled the way it is.
//
//  The required type id comes from a private leaf at 0x6F0241E0
//  (`mov eax, 41416274h / retn`) rather than an immediate - same call
//  shape SCheckedWidgetSlot's own GetWidgetAgileTypeId() has.  0x41416274
//  reads "AAbt" most-significant byte first; nothing in this batch's own
//  closure names what ability or class that tags, so it stays literal
//  (misc_rawcode_getters.cpp's own rule for an unresolved FourCC).
//  GetAAbtTypeTag lives in checkedslot_agent7_leaves.cpp, not `static`
//  here: a `static` one-instruction leaf inlines away entirely at this
//  build's /Ob2, losing the dump's own out-of-line `call` (see that file's
//  own header for the score this cost before the fix).
//============================================================================
#include "agenttypedslots.h"
#include "agiletype.h"

extern unsigned int GetAAbtTypeTag();

SCheckedAAbtSlot* SCheckedAAbtSlot::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetAAbtTypeTag()))
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
