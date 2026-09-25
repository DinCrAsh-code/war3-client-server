//============================================================================
//  CAgentPtr<CGhostImage> - the element type of the world frame's ghost-image
//  reference array at +0x650.
//
//  The shipped name is the tag its SetAlloc (0x6F2ACCA0) hands Storm,
//  ".?AV?$CAgentPtr@VCGhostImage@@@@"; this repo spells that family
//  SCheckedXxxSlot (Agent/agenttypedslots.h says why, and that the two are
//  the same template at different arguments), so this is
//  SCheckedGhostImageSlot.
//
//  Only two operations are reached: the default construction the array's own
//  SetCount does (one store of zero) and the release its shrink arm does.
//  There is no Assign in this call tree, so none is declared.
//============================================================================
#ifndef GHOSTIMAGESLOT_H
#define GHOSTIMAGESLOT_H

#include "agent.h"

struct SCheckedGhostImageSlot
{
    //  Inlined into the array's own shrink loop; no address of its own.  The
    //  decrement and its test are one statement - the shipped stream is
    //  `add [ecx+4], -1` / `jnz`, with no reload of the field - which is the
    //  shape SCheckedUnitSlot::Release has and not the `+= -1` then separate
    //  `== 0` shape half the family has.
    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    CAgent* m_value;
};

#endif
