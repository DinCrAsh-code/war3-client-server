//============================================================================
//  sub_6F430C80: register a handle's object as the agent for a given type,
//  reusing a free slot in the flat agent-slot array when one is available.
//
//  Everything below +0x184 of CAgentRegistry is untouched by this call tree
//  (the class almost certainly starts well before that - CAgentRegistry
//  itself begins at the first field this call tree reads, not at the real
//  object's actual base) and is kept as one reserved block rather than
//  guessed at.
//============================================================================
#ifndef AGENTREGISTRY_H
#define AGENTREGISTRY_H

#include "agent.h"
#include "handle2agent.h"

//: The fixed offset folded into every agent-slot token (see
//: CAgentRegistry::Register).  Not otherwise explained by this call tree -
//: kept as a named constant rather than a bare 0x100000 because it is
//: compared, stored and reconstructed from in three different places.
const unsigned int kAgentSlotTokenBias = 0x100000;

//----------------------------------------------------------------------------
//  The two fields sub_6F430C80 reads off whatever LookupHandle() finds,
//  reinterpreted from its CHandleObject* result.  Deliberately *not* added
//  to CHandleObject itself (game.h): the field at +0x20 falls outside the
//  0x1C bytes SmartPtrToWorldVec3's target already fixed as CHandleObject's
//  size (CPathTrace derives from it at exactly that offset), so it belongs
//  to whatever concrete handle type sub_6F430C80 actually receives, not the
//  shared base every handle type derives from - and this call tree never
//  identifies which concrete type that is.
//----------------------------------------------------------------------------
struct CAgileHandleCheck
{
    char            m_reserved00[0x0C];
    //  0x0C - rejects any handle whose object does not carry this exact
    //  tag before registering it as an agent.
    //
    //  It *is* readable, and the one byte it differs from
    //  kAgileRootTypeId by is the point: MSB first it is '+agl' against
    //  CAgent's own '+aga' (0x2B61676C vs 0x2B616761).  Every class in
    //  the CAgent chain carries a tag of this shape - '+w3a', '+w3i',
    //  '+w3w', '+w3s', '+w3d', '+w3u' - so this is an agile type id in
    //  the same family, not an opaque cookie: what it rejects is an
    //  object of the wrong agile type, and '+agl' is the type every
    //  handle-registered object shares.  See misc_rawcode_getters.cpp
    //  for the whole family in one table.
    //
    //  The same constant appears under three other names in this repo -
    //  kAgentTagPathable (widgetpathref.cpp), kAgileLTag
    //  (agent_slot18.cpp) and a bare literal in widgetagentquery.cpp,
    //  itemhandlemain.cpp and jasssetitemposition_native.cpp.  They are
    //  one tag; the names differ because each site was reconstructed
    //  before the family was mapped.
    unsigned int    m_agileHandleTag;
    char            m_reserved10[0x14];
    //  0x20 - refuses to register a handle whose object already has a
    //  non-zero value here; nothing in this call tree ever writes it, so
    //  whatever marks a handle "already bound to an agent" happens
    //  elsewhere.
    unsigned int    m_agentBinding;
};

//: See CAgileHandleCheck::m_agileHandleTag.
const unsigned int kAgileHandleTag = 0x2B61676C;

//----------------------------------------------------------------------------
//  The object sub_6F430C80's second argument points at.  It is passed to
//  CAgentPtr::AssignChecked() and CAgent::NotifyRegistered() as a bare
//  CAgent* with no adjustment, so it derives from CAgent at offset 0.  The
//  handle and type tag this dump reads at 0x0C/0x10 turned out (see the
//  CWidget__Constructor target, agent.h) to be CAgent's own m_handle/
//  m_typeTag fields, not CUnitAgent's - CUnitAgent adds nothing of its own
//  beyond what CAgent already has.  Kept as its own class regardless: it
//  is the type this call tree's own evidence names, even with an empty
//  body, so a reader sees the same distinction the shipped source did.
//  Nothing else about it is observable from this call tree, including its
//  real name - "unit" only because the one traced caller in this dump's
//  ## User notes is JASS_CreateUnit.
//----------------------------------------------------------------------------
class CUnitAgent : public CAgent
{
};

class CAgentRegistry
{
public:
    //  0x6F430C80.  Returns the new (or reused, or already-registered)
    //  slot token, or 0 if `unit`'s handle does not resolve to a fresh,
    //  correctly-tagged object.  `extra` is stored verbatim into the slot
    //  and never otherwise examined here.
    unsigned int Register(CUnitAgent* unit, unsigned int extra);

    //  0x6F42DD40 - CUnit batch-26.  Drop one reference on the slot named
    //  by `token`, freeing it back through sub_6F42D3F0 (not reconstructed
    //  - called by raw address, the same "real, unhooked shipped code"
    //  idiom unitsubgrouprefresh.cpp's own AppendCandidateSlot uses) once
    //  the count reaches zero.  `retn 4`.
    void ReleaseSlotRef(unsigned int token);

    char                            m_reserved000[0x184];
    TSGrowableArray<void*>          m_freeSlots;    // 0x184 - freed slot tokens
    TSGrowableArray<AGENT_SLOT>     m_slots;        // 0x194
    HANDLE2AGENT_TABLE              m_handleTable;  // 0x1A4 - type tag -> slot
};

#endif
