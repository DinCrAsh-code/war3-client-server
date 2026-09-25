//============================================================================
//  0x6F4A4BE0 - CAgentBaseAbsRelationLists::DetachRelationList9C. See
//  agentbaseabsrelationlists.h for the class/field/TU-split reasoning.
//============================================================================
#include "agentbaseabsrelationlists.h"
#include "agentrelation.h"

void CAgentBaseAbsRelationLists::DetachRelationList9C(int alsoEndpointB)
{
    char* self = (char*)this;
    void* node = *(void**)(self + 0x9C);
    while (node != 0)
    {
        CAgentRelation* relation = *(CAgentRelation**)((char*)node + 8);
        DetachRelation(relation, alsoEndpointB);
        node = *(void**)(self + 0x9C);
    }
}
