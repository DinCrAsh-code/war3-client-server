//============================================================================
//  0x6F4A4D70 - NIpse::CRlAgent::Apply.
//
//  Reached through CAgentBaseAbs's own vtable slot 3 (+0x0C), but that is
//  not who owns it: walking the full CPresence -> ... -> CAgentBaseAbs
//  slot-3 column (agent_worktrees/classes/<ADDR>.json for each level),
//  CPresenceTagged overrides slot 3 first (0x6F4AEFF0), CPrRelation
//  overrides it again (0x6F4A6070), and CRlAgent overrides it a *third*
//  time with this address - CAgentBaseAbs never touches the slot again.
//  So this is CRlAgent's own method (CLAUDE.md's "lowest class in the chain
//  whose own address for that slot differs from its own base's"), not
//  CAgentBaseAbs's.
//
//  Tells some other object (`arg`) about this agent (0x6F4A6070, out of
//  this session's batch), looks up whatever `arg` itself resolves to
//  (0x6F4A45E0) and stamps the result into this agent's own `this+0x50` -
//  the same field `NIpse::CRlAgent::Save` (0x6F4A4EE0,
//  agentbaseabs_relatedaccessors.cpp/agentbaseabssave.cpp) later writes out
//  first, ahead of the related-agent count - then, if that lookup found
//  something, fixes it up (0x6F4A45B0) with this agent as its own argument.
//
//  0x6F4A6070 is now real - `CAgentRelation::ResetEventBinding`
//  (agentbaseabseventbinding.cpp, a parallel session's own batch of this
//  dump): called here with the outer `this` unchanged, the same "this class
//  *is* a CAgentRelation at offset 0" confirmation agentbaseabs_related.h's
//  own header comment already draws from 0x6F4A6570/`Save`.
//  0x6F4A45E0/0x6F4A45B0 are still out of this session's batch, owned by a
//  parallel session working the same dump - forward-declared in
//  agentbaseabs_related.h purely so this translation unit compiles under
//  `verify.py`, which never links.
//============================================================================
#include "crlagent.h"
#include "agentbaseabs_related.h"

namespace NIpse {

void CRlAgent::Apply(SHandleRegistrationRequest* arg)
{
    ((CAgentRelation*)this)->ResetEventBinding(arg);

    void* result = ((CAgentBaseAbsSlot3Arg*)arg)->LookUp_6F4A45E0();
    *(void**)((char*)this + 0x50) = result;

    if (result != 0)
        ((CAgentBaseAbsSlot3Result*)((char*)result + 0x50))->FixUp_6F4A45B0(this);
}

}  // namespace NIpse
