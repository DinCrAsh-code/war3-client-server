//============================================================================
//  0x6F4975C0 - NIpse::CRlProperty::Slot4, vtable slot 4.  Sets the
//  0x1000000 flags bit (+0x4C, the same bit Pathfinding/crlagent.h's own
//  `CRlAgent::Slot4` sets), detaches every relation still threaded onto
//  this object's own two dependency lists (DetachDependents,
//  rlproperty_detachdependents.cpp - kept in its own translation unit so
//  the real out-of-line `call` the dump shows is not inlined away, per
//  CLAUDE.md's "put a callee in its own TU"), then tears the underlying
//  relation down.
//============================================================================
#include "rlproperty.h"
#include "agentrelation.h"

namespace NIpse {

void CRlProperty::Slot4(int arg)
{
    *(unsigned int*)((char*)this + 0x4C) |= 0x1000000;

    DetachDependents();

    ((CAgentRelation*)this)->Teardown(arg);
}

}  // namespace NIpse
