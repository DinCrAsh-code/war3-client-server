//============================================================================
//  0x6F4A47A0 - NIpse::CRlAgentDef::Apply, vtable slot 3.  Resets this
//  class's own embedded CAgentRelation's event binding, then registers
//  `this` at the front of the global registry rooted at
//  `g_pTimeSync + 0x25C` (LinkNodeIntoRegistry, rlagentdef_linknoderegistry.cpp
//  - its own translation unit, per CLAUDE.md's "put a callee in its own TU").
//============================================================================
#include "rlagentdef.h"
#include "agentrelation.h"
#include "game.h"

namespace NIpse {

void CRlAgentDef::Apply(SHandleRegistrationRequest* arg)
{
    ((CAgentRelation*)this)->ResetEventBinding(arg);

    ((SRegistryRoot*)((char*)g_pTimeSync + 0x25C))->LinkNodeIntoRegistry(this);
}

}  // namespace NIpse
