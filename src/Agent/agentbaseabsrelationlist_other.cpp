//============================================================================
//  0x6F4A4B80 / 0x6F4A4BB0 - the other two of this agent's four relation
//  lists' own detach-all functions (`this+0x7C`/`this+0x8C`), completing
//  the set `agentbaseabsrelationlist9c.cpp`/`agentbaseabsrelationlistac.cpp`
//  (`this+0x9C`/`this+0xAC`) already cover - see
//  `agentbaseabsrelationlists.h`'s own header comment for the shared
//  reasoning (list shape not independently confirmed, own TU per function
//  since /O2 inlines these into whichever caller shares a file with them).
//============================================================================
#include "agentbaseabsrelationlists.h"
#include "agentrelation.h"

void CAgentBaseAbsRelationListsExternal::DetachOther2_6F4A4B80(int alsoEndpointB)
{
    char* self = (char*)this;
    void* node = *(void**)(self + 0x7C);
    while (node != 0)
    {
        CAgentRelation* relation = *(CAgentRelation**)((char*)node + 8);
        DetachRelation(relation, alsoEndpointB);
        node = *(void**)(self + 0x7C);
    }
}

void CAgentBaseAbsRelationListsExternal::DetachOther1_6F4A4BB0(int alsoEndpointB)
{
    char* self = (char*)this;
    void* node = *(void**)(self + 0x8C);
    while (node != 0)
    {
        CAgentRelation* relation = *(CAgentRelation**)((char*)node + 8);
        DetachRelation(relation, alsoEndpointB);
        node = *(void**)(self + 0x8C);
    }
}
