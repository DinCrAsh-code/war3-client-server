//============================================================================
//  0x6F48E630 - SAgentLiveList::InsertAtHead, the live-instance-list root
//  `NIpse::CPrCluster::Load` (prcluster_saveload.cpp) publishes `this` onto
//  at `g_pTimeSync + 0x3B0` (Game/game.h).  A plain doubly-linked list with
//  the head structure itself acting as the "previous" node for whatever
//  currently sits first: `node->m_prevLink = head` (the head pointer
//  itself, not null), `node->m_nextLink = head->m_first` (the old first
//  node, or null), the old first node's own `m_prevLink` repointed at
//  `node` if it existed, and finally `head->m_first = node`.  Same
//  +0x04/+0x08 prev/next field pair every class in this whole family's own
//  intrusive list already uses (Agent/presence.h's own
//  `CPresence::ReturnToStaticPool`) - `SAgentLiveList` here is just that
//  same node shape used as a list head instead of an element.
//
//  Own translation unit from its one caller (prcluster_saveload.cpp) - a
//  real `call` in the shipped dump.
//============================================================================
#ifndef PRCLUSTERLIVELIST_H
#define PRCLUSTERLIVELIST_H

struct SAgentLiveList
{
    char         m_reserved00[8]; // +0x00..+0x08 - untouched by this call tree
    void*        m_first;         // +0x08 - the list's own first element

    //  0x6F48E630 - `this` in `ecx`, the node to insert in the one stack
    //  argument, `retn 4`.
    void InsertAtHead(void* node);
};

#endif
