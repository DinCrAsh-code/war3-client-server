//============================================================================
//  0x6F4AA050 / 0x6F4AA060 - NIpse::CPoLkLis::Save/Load, vtable slots 9/10.
//  Both shipped bodies are a single `jmp` straight into
//  CAgentRelation::Save/::Load (Agent/agentrelation.h, 0x6F4A6570/
//  0x6F4A6C10) - same argument, same `retn 4` - the same tail-jmp shape
//  Slot4 above documents.
//============================================================================
#include "cpolklis.h"
#include "agentrelation.h"

namespace NIpse {

void CPoLkLis::Save(void* writer)
{
    ((CAgentRelation*)this)->Save(writer);
}

void CPoLkLis::Load(void* reader)
{
    ((CAgentRelation*)this)->Load(reader);
}

}  // namespace NIpse
