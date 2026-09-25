//============================================================================
//  0x6F48E610 - SAgentLiveInstanceListHead::InsertAtHead.  The FIRST
//  reconstructed writer of this shape anywhere in the whole
//  CPresence-derived family: every ReturnToStaticPool this codebase
//  already has (Agent/presence_returntopool.cpp and every sibling in
//  Agent/agentbasechain_staticpools.{h,cpp}'s own per-class getter list)
//  only ever UNLINKS `this` from a doubly-linked list of live instances via
//  its own +0x04/+0x08 prev/next pair - nothing until NIpse::CPrClusterGroup's
//  own Apply (Agent/prcluster.h, vtable slot 3) ever INSERTS into one.
//
//  Reached with `this` (ecx) = the list's own head - a plain node-shaped
//  four/eight bytes, not a separate bookkeeping struct: the head's own
//  +0x08 IS the "first live element" pointer, exactly the offset a node's
//  own `m_nextLink` (+0x08) sits at, and the head's own +0x04 is never
//  touched here (this call tree never reads it back either) - so the head
//  is simply the same CPresence-shaped node struct used as a sentinel,
//  matching how a doubly-linked list with a plain-node head is usually
//  built in this codebase's own style elsewhere.  `node` (the sole stack
//  argument, `retn 4`) is inserted at the head end: `node->prev = this`,
//  `node->next = this->first`, and if `this->first` was non-null its own
//  `prev` is repointed at `node` before `this->first` becomes `node`.
//
//  `CPrClusterGroup`'s own call site computes its list head inline, both
//  places it is used (Apply and Load, Agent/prclustergroup_slots2.cpp) as
//  `(char*)g_pTimeSync + 0x384` - no separate getter function exists for
//  it in the shipped binary (unlike the static-pool getters in
//  Agent/agentbasechain_staticpools.h, which ARE their own out-of-line
//  leaves) because the shipped code computes this one address inline at
//  both call sites rather than through a call.
//============================================================================
#ifndef AGENTLIVEINSTANCELIST_H
#define AGENTLIVEINSTANCELIST_H

struct SAgentLiveInstanceListHead
{
    void InsertAtHead(void* node);
};

#endif  // AGENTLIVEINSTANCELIST_H
