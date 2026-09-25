//============================================================================
//  IntegerModifier - `??_7IntegerModifier@@6B@`, RTTI-verified
//  (agent_worktrees/classes/0x6F951B78.json, Class Informer ground truth:
//  "IntegerModifier: CModifier, CRelation, CAgentField, CPresenceRef,
//  TRefCnt;"). `IntegerModifier : public CModifier` (modifier.h).
//
//  Adds no new vtable slots and no new overrides beyond its own DeleteSelf:
//  slots 6/7 (AttachRelationEndpointB/A) both still read 0x6F4797F0/
//  0x6F479760 in this class's own committed vtable
//  (agent_worktrees/classes/0x6F951B78.json) - CModifier's real bodies,
//  inherited unmodified, not re-overridden here.
//============================================================================
#ifndef INTEGERMODIFIER_H
#define INTEGERMODIFIER_H

#include "modifier.h"

class IntegerModifier : public CModifier
{
public:
    //  slot 1 / +0x04 (0x6F472FD0) - real. See integermodifier_deleteself.cpp.
    virtual IntegerModifier* DeleteSelf(int flags);
};

#endif
