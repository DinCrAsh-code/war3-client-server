//============================================================================
//  CRelation - `??_7CRelation@@6B@`, RTTI-verified
//  (agent_worktrees/classes/0x6F877800.json, Class Informer ground truth:
//  "CRelation: CAgentField, CPresenceRef, TRefCnt;"). `CRelation : public
//  CAgentField` (agentfield.h) - the next link past the
//  TRefCnt -> CPresenceRef -> CAgentField chain those files already cover.
//
//  Slots 0/2/3/4/5 (ReleaseSelf/Detach/AttachFromTable/GetTrackedAgent/
//  GetAgent) are CAgentField's own, inherited here unmodified - see
//  agentfield.h, not repeated in this file.
//
//  Slot 1 (this class's own DeleteSelf override, 0x6F02F4F0) is real - see
//  relation_deleteself.cpp. Byte-identical in shape to every other slot-1
//  override in this chain (Agent/refcnt.h's own header comment explains
//  why: no class in this whole TRefCnt lineage has members with their own
//  destructors, so every level's deleting destructor collapses straight to
//  resetting the vtable pointer to TRefCnt's own, not to its immediate
//  base's).
//
//  Slots 6/7 (+0x18/+0x1C) are new here - CAgentField's own vtable
//  (agent_worktrees/classes/0x6F876D48.json) has only 6 slots, so these are
//  genuinely CRelation's own additions, not overrides of anything CAgentField
//  declared. On CRelation itself both are bare, argument-consuming no-ops
//  (0x6F02F4C0 `retn 4`, 0x6F02F4D0 `retn 8` - real one-instruction bodies,
//  not placeholders): a plain CRelation is never attached to an actual
//  CAgentRelation, so there is nothing for either slot to do at this level.
//  `CModifier` (modifier.h) gives both real bodies - see that file's own
//  header comment for why a CModifier composes with a CAgentRelation rather
//  than deriving from it. The two slots must keep the same name+signature at
//  every level of this chain (same load-bearing reasoning
//  Agent/presenceref.h's own header comment gives for `Detach`) or the
//  vtable's slot count would come out wrong at CModifier and every class
//  under it.
//
//  No constructor for this class was found anywhere in the current bulk
//  agent_worktrees/funcs/ dump (grepped for this vtable's own address and
//  for every callee address this file's own functions reach - zero hits) -
//  nothing here invents one.
//============================================================================
#ifndef RELATION_H
#define RELATION_H

#include "agentfield.h"

class CAgentRelation;   // Agent/agentrelation.h - only ever seen by pointer here

class CRelation : public CAgentField
{
public:
    //  slot 1 / +0x04 (0x6F02F4F0) - real. See relation_deleteself.cpp.
    virtual CRelation* DeleteSelf(int flags);

    //  slot 6 / +0x18 (0x6F02F4C0) - new slot this class introduces. Base
    //  version is a genuine bare no-op (`retn 4`) - real, if empty, body:
    //  a plain CRelation has no CAgentRelation to attach anything to.
    //  Overridden with real logic by CModifier::AttachRelationEndpointB
    //  (modifier.h, 0x6F4797F0). Defined out-of-line in
    //  relation_deleteself.cpp, not inline here - nothing else forces
    //  ??_7CRelation@@6B@ (no constructor for this class exists to stamp
    //  it), so an inline `{}` body is never emitted into any TU at all and
    //  has nothing for verify.py to score against the dump.
    virtual void AttachRelationEndpointB(void* context);

    //  slot 7 / +0x1C (0x6F02F4D0) - new slot this class introduces. Base
    //  version is a genuine bare no-op (`retn 8`). Overridden with real
    //  logic by CModifier::AttachRelationEndpointA (modifier.h, 0x6F479760).
    //  Same out-of-line reasoning as AttachRelationEndpointB above.
    virtual void AttachRelationEndpointA(void* context, CAgentRelation* relation);
};

#endif
