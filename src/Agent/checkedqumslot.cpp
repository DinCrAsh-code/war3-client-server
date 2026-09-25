//============================================================================
//  0x6F4271E0 and 0x6F428520 - the '+qum' instantiation of the
//  type-checked agent slot (agenttypedslots.h), the one the agent
//  registry's own singleton finish step (0x6F42C210,
//  agentregistrysingleton.cpp) holds.
//
//  Own translation unit: 0x6F428520 issues a real `call sub_6F4271E0`,
//  and both sit in the 0x6F42xxxx registry module rather than with the
//  item and unit pairs at 0x6F02x/0x6F03x.
//============================================================================
#include "agenttypedslots.h"
#include "agiletype.h"
#include <excpt.h>

//  0x6F3DB6A0 - 0x2B71756D, '+qum' MSB first: the same '+' prefix the
//  rest of the agile type family carries.  misc_rawcode_getters.cpp's.
unsigned int GetConstant_6F3DB6A0();

//----------------------------------------------------------------------------
//  0x6F428520.
//----------------------------------------------------------------------------
SCheckedQumSlot::SCheckedQumSlot(CAgent* candidate)
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
//  0x6F4271E0.  Same body as SCheckedUnitSlot::Assign against a different
//  required type - see agenttypedslots.h for why these are six bodies
//  and not one template.
//----------------------------------------------------------------------------
SCheckedQumSlot* SCheckedQumSlot::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetConstant_6F3DB6A0()))
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
