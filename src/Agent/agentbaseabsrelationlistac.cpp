//============================================================================
//  0x6F4A4C10 - CAgentBaseAbsRelationLists::DetachRelationListAC. See
//  agentbaseabsrelationlists.h for the class/field/TU-split reasoning.
//============================================================================
#include "agentbaseabsrelationlists.h"
#include "agentrelation.h"

void CAgentBaseAbsRelationLists::DetachRelationListAC(int alsoEndpointB)
{
    char* self = (char*)this;
    void* node = *(void**)(self + 0xAC);
    while (node != 0)
    {
        CAgentRelation* relation = *(CAgentRelation**)((char*)node + 8);
        DetachRelation(relation, alsoEndpointB);
        node = *(void**)(self + 0xAC);
    }
}
