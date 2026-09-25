//============================================================================
//  0x6F4A4C40 - CAgentBaseAbsRelationLists::DetachAllRelationLists: all four
//  of this agent's own relation lists. See agentbaseabsrelationlists.h for
//  the class/field/TU-split reasoning (DetachRelationList9C/ListAC live in
//  their own translation units so this call stays a real `call` instead of
//  getting inlined here).
//============================================================================
#include "agentbaseabsrelationlists.h"

void CAgentBaseAbsRelationLists::DetachAllRelationLists(int alsoEndpointB)
{
    DetachRelationListAC(alsoEndpointB);
    DetachRelationList9C(alsoEndpointB);
    ((CAgentBaseAbsRelationListsExternal*)this)
        ->DetachOther1_6F4A4BB0(alsoEndpointB);
    ((CAgentBaseAbsRelationListsExternal*)this)
        ->DetachOther2_6F4A4B80(alsoEndpointB);
}
