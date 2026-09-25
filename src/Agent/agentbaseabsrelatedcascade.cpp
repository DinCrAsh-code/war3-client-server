//============================================================================
//  0x6F4A57F0 - notify every related agent that this one is going away
//  (cascade its own vtable slot 4 - the same slot this function's own
//  caller, 0x6F4A5A70 below, sits at - with argument 0, walking the table
//  backwards), then clear the whole related-agents table
//  (`CRelatedAgentsTable::RemoveRange`, 0x6F4A5060,
//  Agent/agentbaseabs_related.h - a *different* parallel session's own
//  batch of this same dump, reused here by declaration only).
//
//  The backwards walk re-reads the table's own live count on every
//  iteration (`[this+0x74]`) and re-clamps its own index against it rather
//  than trusting a value cached before the loop started - defensive
//  against a slot-4 callee that itself shrinks this same table reentrantly
//  (plausible: a related agent's own Teardown detaches it from whatever
//  lists still reference it, and this table may be one of them).  Kept
//  exactly as written; a version that simply cached the count once would
//  read past a table a reentrant call had already shrunk.
//
//  0x6F4A5A70 (this function's own caller, CAgentBaseAbs's own vtable slot
//  4) is this file's other function.
//
//  CORRECTION (2026-09-07, vtable-derived): 0x6F4A5A70 is reached through
//  CAgentBaseAbs's own vtable slot 4 (+0x10), but is not that class's own
//  override. Walking the slot-4 column across the chain, CPresenceTagged
//  and CPrRelation both leave it a no-op, CRlAgent is the first real
//  override, and CAgentBaseAbs never touches it again - so this is
//  `NIpse::CRlAgent::Slot4` (Pathfinding/crlagent.h), not
//  `CAgentBaseAbsSlot4Host::Slot4`.
//============================================================================
#include "crlagent.h"
#include "agentrelation.h"
#include "agentbaseabs_related.h"
#include "agentbaseabsrelationlists.h"

typedef void (__thiscall *CAgentBaseAbsSlot4Fn)(void*, int);

//  0x6F4A4DF0 is `SAgentBaseAbsNotifySubject::NotifyField1(unsigned int)`
//  (Agent/agentbaseabsnotify.cpp, no separate header - re-declared locally
//  below, same shape every other cross-TU call in this codebase uses),
//  called here on `self` with subjectId=0 (the call site leaves `this` in
//  ecx untouched from Slot4's own prologue rather than re-loading it,
//  matching a plain `__thiscall` member - not the 2-register `__fastcall`
//  free function an earlier draft of this file guessed at).
struct SAgentBaseAbsNotifySubject
{
    void NotifyField1(unsigned int subjectId);
};

//  0x6F4A4C40 is NOT a second notify at all - reading the real dump
//  (`mov ecx,esi; push 0; call sub_6F4A4C40`, an explicit `this` reload
//  immediately before the call, unlike the 0x6F4A4DF0 call just above it)
//  shows this is the *already-reconstructed*
//  `CAgentBaseAbsRelationLists::DetachAllRelationLists(int alsoEndpointB)`
//  (Agent/agentbaseabsrelationlists.cpp, confirmed EXACT), called with
//  `alsoEndpointB=0` - an earlier draft of this file mis-read the call
//  site as a second "notify" and invented a `NotifyField2Group` name for
//  it; corrected here per CLAUDE.md's "fix the semantics to the correct
//  naming ... in all places where it's called" rule.

//----------------------------------------------------------------------------
//  0x6F4A57F0 - see this file's own header comment.
//----------------------------------------------------------------------------
void __fastcall RelatedAgentsCascade(void* outer)
{
    unsigned int count = *(unsigned int*)((char*)outer + 0x74);
    if (count == 0)
        return;

    unsigned int i = count - 1;
    CRelatedAgentsTable* table = (CRelatedAgentsTable*)((char*)outer + 0x58);
    for (;;)
    {
        count = *(unsigned int*)((char*)outer + 0x74);
        if (i >= count)
            i = count - 1;

        void** data = *(void***)((char*)outer + 0x64);
        void** slot = data + i;
        if (*slot != 0)
        {
            void* entry = *slot;
            CAgentBaseAbsSlot4Fn slot4 = (CAgentBaseAbsSlot4Fn)(*(void***)entry)[4];
            slot4(entry, 0);
        }

        if (i == 0)
            break;
        i -= 1;
    }

    unsigned int liveCount = table->m_count;
    if (liveCount != 0)
        table->RemoveRange(0, liveCount);
}

//----------------------------------------------------------------------------
//  0x6F4A5A70 - NIpse::CRlAgent::Slot4, reached through CAgentBaseAbs's own
//  vtable slot 4 (+0x10, confirmed by docs/targets/vtables/CAgentBaseAbs.txt)
//  but owned by CRlAgent - see this file's own CORRECTION note above.  Sets
//  the 0x1000000 flags bit
//  (m_flags, +0x4C, agentrelation.h), calls two callees out of this
//  session's own batch (0x6F4A4DF0/0x6F4A4C40, owned by a parallel
//  session - forward-declared as raw-address indirect calls the same way
//  Agent/cagentbaseabs_module_batch2.cpp's own out-of-batch calls are),
//  cascades to every related agent and clears the table
//  (RelatedAgentsCascade above), unlinks this object from whatever
//  `CLinkedNode<CPresence>`-shaped list it is on (+0x04/+0x08 - the base
//  class this whole chain's own file header names, agentrelation.h's
//  m_reserved00 covers this range too and this is the first evidence of
//  what two of those bytes are), zeroes CRlAgent's own +0x50/+0x54
//  (crlagent.h), and finally tears the relation itself down
//  (CAgentRelation::Teardown).
//----------------------------------------------------------------------------
namespace NIpse {

void CRlAgent::Slot4(int arg)
{
    void* self = this;
    *(unsigned int*)((char*)self + 0x4C) |= 0x1000000;

    ((SAgentBaseAbsNotifySubject*)self)->NotifyField1(0);
    ((CAgentBaseAbsRelationLists*)self)->DetachAllRelationLists(0);

    RelatedAgentsCascade(self);

    void** node = (void**)self;
    void* prevNode = node[1];   // +0x04
    void* nextNode = node[2];   // +0x08
    if (prevNode != 0)
        ((void**)prevNode)[2] = nextNode;
    if (nextNode != 0)
        ((void**)nextNode)[1] = prevNode;
    node[1] = 0;
    node[2] = 0;

    *(void**)((char*)self + 0x50) = 0;
    *(void**)((char*)self + 0x54) = 0;

    ((CAgentRelation*)self)->Teardown(arg);
}

}  // namespace NIpse
