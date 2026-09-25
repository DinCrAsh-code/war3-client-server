//============================================================================
//  SCheckedSlot_6F2D04D0 - one more instantiation of the type-checked
//  CAgent-slot family (agenttypedslots.h), reached from createunit-G2's own
//  dump.  Its own Assign (0x6F2D04D0) is out of this agent's own worklist
//  slice - declared, not defined, the same way cunit_agent8_abilityfactories.cpp
//  handles a family member reached one hop past its own cut.
//============================================================================
#ifndef CHECKEDSLOT_6F2D04D0_H
#define CHECKEDSLOT_6F2D04D0_H

#include "agent.h"

struct SCheckedSlot_6F2D04D0
{
    //  0x6F2D3800 - unit_checkedslot6f2d04d0_ctor.cpp.
    SCheckedSlot_6F2D04D0(CAgent* candidate);

    ~SCheckedSlot_6F2D04D0() { Release(); }

    //  0x6F2D04D0 - out of scope, declared only.
    SCheckedSlot_6F2D04D0* Assign(CAgent* candidate);

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
