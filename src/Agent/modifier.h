//============================================================================
//  CModifier - `??_7CModifier@@6B@`, RTTI-verified
//  (agent_worktrees/classes/0x6F877824.json, Class Informer ground truth:
//  "CModifier: CRelation, CAgentField, CPresenceRef, TRefCnt;").
//  `CModifier : public CRelation` (relation.h).
//
//  Slots 0/2/3/4/5 are CAgentField's own, inherited unmodified through
//  CRelation - see agentfield.h. Slot 1 (this class's own DeleteSelf
//  override, 0x6F02F590) is the same trivial-destructor shape as every
//  other level - see modifier_deleteself.cpp.
//
//  **Slots 6/7 (AttachRelationEndpointB/A, +0x18/+0x1C) are where this class
//  actually does something, and it is composition, not inheritance: a
//  `CModifier` is NOT a `CAgentRelation`.** It only *owns a handle to* one,
//  through the `m_handle`/`m_typeTag` fields it inherits from CPresenceRef
//  (presenceref.h) - the same handle pair every class in this chain already
//  carries to look up whatever tracked object it refers to
//  (CAgentField::GetTrackedAgent/GetAgent do the equivalent resolve for a
//  tracked CAgent). Both new overrides resolve that handle via
//  `LookupHandle(m_handle, m_typeTag)` (game.h) to get the actual
//  `CAgentRelation*` (agentrelation.h), then forward onto its own
//  `AttachEndpointB`/`AttachEndpointA` - already reconstructed, EXACT,
//  do not touch. See modifier_relationattach.cpp for both bodies and the
//  full struct-layout reasoning (the embedded `SRelationLink`s at +0x50 and
//  +0x98 of the caller-owned `context` argument, and why
//  AttachRelationEndpointA's own incoming `this` is genuinely unread - the
//  resolved `CAgentRelation*` arrives as its second explicit argument
//  instead).
//============================================================================
#ifndef MODIFIER_H
#define MODIFIER_H

#include "relation.h"
#include "agentrelation.h"

class CModifier : public CRelation
{
public:
    //  slot 1 / +0x04 (0x6F02F590) - real. See modifier_deleteself.cpp.
    virtual CModifier* DeleteSelf(int flags);

    //  slot 6 / +0x18 (0x6F4797F0) - real override of CRelation's own
    //  no-op. See modifier_relationattach.cpp.
    virtual void AttachRelationEndpointB(void* context);

    //  slot 7 / +0x1C (0x6F479760) - real override of CRelation's own
    //  no-op. See modifier_relationattach.cpp.
    virtual void AttachRelationEndpointA(void* context, CAgentRelation* relation);
};

#endif
