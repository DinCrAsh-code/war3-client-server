//============================================================================
//  CModifier's own slots 6/7 - both real, both resolve this object's own
//  tracked CAgentRelation (m_handle/m_typeTag, inherited from CPresenceRef)
//  and forward onto one of its endpoint-attach members. Adjacent addresses
//  (0x6F4797F0/0x6F479760), one translation unit - same "address
//  neighbourhood = one module" rule as everywhere else in this repo.
//
//  0x6F4797F0 - AttachRelationEndpointB(void* context). One stack argument
//  (`retn 4`): `context` is a caller-owned struct with an SRelationLink
//  embedded at +0x50. Resolves this->m_handle/m_typeTag through
//  LookupHandle (game.h, already reconstructed, DIFFERS - register
//  allocation only), then calls the resolved CAgentRelation's own
//  AttachEndpointB (agentrelation.h, already EXACT) with `context` itself as
//  the endpoint object and the embedded link as the splice-onto list.
//
//  0x6F479760 - AttachRelationEndpointA(void* context, CAgentRelation*
//  relation). Two stack arguments (`retn 8`) - note the incoming `this`
//  (ecx) is never read: the CAgentRelation to operate on arrives as the
//  second explicit argument instead, and the call is made straight through
//  it rather than through any handle lookup on `this`. `context` here has
//  its OWN embedded SRelationLink, at a different offset (+0x98, not the
//  +0x50 the B-side uses) - the two endpoints are unrelated structs from the
//  caller's point of view, matched only by the shape LookupHandle/
//  AttachEndpointA and AttachEndpointB happen to share. After attaching,
//  ORs 0x60000000 into `relation->m_flags` - both endpoints' "the
//  registration was holding it" bits (kRelDropAOnRelease | kRelDropBOnRelease,
//  agentrelation.h) - set unconditionally, not read back or branched on
//  here.
//============================================================================
#include "modifier.h"

//  One expression, not a named local for the resolved relation: MSVC
//  evaluates a call's arguments before its object expression (see
//  docs/msvc-vc8-idioms.md, "Arguments pushed before a call whose receiver
//  is itself a call"), so this pushes AttachEndpointB's two arguments
//  before calling LookupHandle for its receiver - matching the shipped
//  order (push list, push endpoint, *then* resolve the handle). Binding
//  the LookupHandle result to a local first reverses that and moves every
//  push after the call instead.
void CModifier::AttachRelationEndpointB(void* context)
{
    ((CAgentRelation*)LookupHandle(m_handle, m_typeTag))
        ->AttachEndpointB(context, (SRelationLink*)((char*)context + 0x50));
}

void CModifier::AttachRelationEndpointA(void* context, CAgentRelation* relation)
{
    relation->AttachEndpointA(context, (SRelationLink*)((char*)context + 0x98));
    relation->m_flags |= 0x60000000;
}
