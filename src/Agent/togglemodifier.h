//============================================================================
//  ToggleModifier - `??_7ToggleModifier@@6B@`, RTTI-verified
//  (agent_worktrees/classes/0x6F951B9C.json, Class Informer ground truth:
//  "ToggleModifier: IntegerModifier, CModifier, CRelation, CAgentField,
//  CPresenceRef, TRefCnt;"). `ToggleModifier : public IntegerModifier`
//  (integermodifier.h).
//
//  Adds no new vtable slots and no new overrides beyond its own DeleteSelf -
//  same "inherits CModifier's real slot 6/7 bodies unmodified" note as
//  IntegerModifier (this class's own committed vtable,
//  agent_worktrees/classes/0x6F951B9C.json, still reads 0x6F4797F0/
//  0x6F479760 at those two slots).
//============================================================================
#ifndef TOGGLEMODIFIER_H
#define TOGGLEMODIFIER_H

#include "integermodifier.h"

class ToggleModifier : public IntegerModifier
{
public:
    //  slot 1 / +0x04 (0x6F473110) - real. See togglemodifier_deleteself.cpp.
    virtual ToggleModifier* DeleteSelf(int flags);
};

#endif
