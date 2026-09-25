//============================================================================
//  0x6F4974A0 - NIpse::CRlProperty::DetachDependents, called only from
//  rlproperty_slot4.cpp's own Slot4 - own translation unit so that real,
//  out-of-line `call` stays out of line (CLAUDE.md's "put a callee in its
//  own TU"; inlined into the same file, this cost Slot4 16 instructions of
//  score - see rlproperty_slot4.cpp's own header).
//
//  Walks a raw head pointer (+0x64, then +0x54), each node a
//  `SRelationEndpointSlot`-shaped record (agentrelationsyncsave.cpp's own
//  struct: m_prev/m_next/m_owner/m_endpoint) - `node->m_owner` is the
//  relation actually attached, and `CAgentRelation::DetachEndpointB` is
//  what splices the node back out and advances the head; the loop simply
//  re-reads the head until DetachEndpointB has emptied it.
//============================================================================
#include "rlproperty.h"
#include "agentrelation.h"

namespace NIpse {

void CRlProperty::DetachDependents()
{
    char* self = (char*)this;

    void** headA = (void**)(self + 0x64);
    while (*headA != 0)
    {
        void* owner = *(void**)((char*)*headA + 8);
        ((CAgentRelation*)owner)->DetachEndpointB();
    }

    void** headB = (void**)(self + 0x54);
    while (*headB != 0)
    {
        void* owner = *(void**)((char*)*headB + 8);
        ((CAgentRelation*)owner)->DetachEndpointB();
    }
}

}  // namespace NIpse
