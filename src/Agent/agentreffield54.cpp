//============================================================================
//  0x6F4786A0 - AgentRef::QueryField0x54.  See agentref.h.
//
//  Six instructions: Resolve(), and the +0x54 word off whatever it hands
//  back.  The null case falls out of the resolve itself - eax is already
//  zero at the `retn`, so there is no second `xor`, which is why this is
//  written as an early return on the pointer rather than as a conditional
//  expression over the field read.
//
//  `this` is never touched here: the shipped body opens straight on the
//  `call`, letting ecx carry through to Resolve() unchanged.
//
//  Own translation unit: it calls Resolve() for real, 0x70 bytes back in the
//  same module, and its own caller is CAgent::Handler_lkl (0x6F472A00,
//  agent_bighandlers.cpp), which builds the AgentRef on its own frame and
//  passes its address in ecx.
//============================================================================
#include "agentref.h"

void* AgentRef::QueryField0x54()
{
    SItemHandleObject* object = Resolve();
    if (object == 0)
        return 0;

    return object->m_field54;
}
