//============================================================================
//  0x6F4975A0 - NIpse::CRlProperty::Apply, vtable slot 3.
//
//  Tells this object's own embedded CAgentRelation about `arg`
//  (ResetEventBinding, agentrelation.h), then tail-calls the exact same
//  leaf address Pathfinding/CPathTrace2.cpp's own
//  `CPathTrace::RefreshSyncSample` already owns (0x6F497440) - an
//  already-EXACT, cross-dump-reused body (tools/funcmap.py's own
//  BEHAVIOUR note on that address), called here against `this`
//  reinterpreted as the layout-compatible receiver the shipped code
//  actually hands it.  Simpler than Pathfinding/crlagent.h's own
//  `CRlAgent::Apply` one level up this chain - no lookup/fixup step.
//============================================================================
#include "rlproperty.h"
#include "agentrelation.h"
#include "game.h"

namespace NIpse {

void CRlProperty::Apply(SHandleRegistrationRequest* arg)
{
    ((CAgentRelation*)this)->ResetEventBinding(arg);
    ((CPathTrace*)this)->RefreshSyncSample();
}

}  // namespace NIpse
