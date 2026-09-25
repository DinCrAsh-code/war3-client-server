//============================================================================
//  0x6F4728D0 - CAgent::SetTrackedTarget.  See agent.h.
//
//  Was a naked redirect in Unit/unitorderpaththunks.cpp until its dump
//  arrived; this is the body.
//
//  Both this agent and the target go through LookupHandle on their own
//  +0x0C/+0x10 pair and then the same branchless '+agl' tag check
//  CAgent::Method_0x18 and CAgent::Method_0x5C use - `xor`/`cmp`/`setnz`/
//  `sub 1`/`and`, docs/msvc-vc8-idioms.md's tag-or-null idiom - so the tag
//  check is spelled as the conditional expression it is, not as an `if`.
//
//  What this agent resolves to is a CAgentRelation: it is handed to
//  AttachEndpointA as `this` on one path and to DetachRelation as the
//  relation on the other.
//
//  Two things about the shape are the shipped code's and not this file's:
//
//  * the target's tag check is guarded by a null test but the *call after it
//    is not* - a null target object still reaches AttachEndpointA, with a
//    list head of `0 + 0x78` - so the compiler clones the whole call into
//    both arms of that test.  Writing the check as a statement that narrows
//    `object` in place and then calling once is what produces the two
//    copies; hoisting the call into each arm by hand would say the same
//    thing less clearly.
//  * the null-target path opens with a `mov ecx,[esi+2Ch]` whose value
//    nothing ever reads: DetachRelation is `__stdcall` and takes both of its
//    arguments on the stack.  It is a live-range leftover in the shipped
//    build - the one instruction of 47 no C++ spelling of this function
//    reproduces, since a load with no use is exactly what /O2 deletes.  See
//    the BEHAVIOUR entry in tools/funcmap.py.
//
//  Own translation unit: CTaskActionFields::Init (unittaskactioninit.cpp) calls it
//  for real, and so do the two relation functions below it.
//============================================================================
#include "agent.h"
#include "agentrelation.h"

//  '+agl' read low byte first - the agile type id every handle-registered
//  object of this family carries.  Same constant as agent_slot18.cpp's
//  kAgileLTag and agentregistry.h's kAgileHandleTag, which carries the note.
static const int kAgileLTag = 0x2B61676C;

void CAgent::SetTrackedTarget(void* target)
{
    CHandleObject* self = LookupHandle(m_handle, m_typeTag);
    CAgentRelation* relation;
    if (self == 0)
        relation = 0;
    else
        relation = (CAgentRelation*)
            ((self->m_kindTag == kAgileLTag) ? self : 0);

    if (target != 0)
    {
        CHandleObject* object = LookupHandle(((CAgent*)target)->m_handle,
                                            ((CAgent*)target)->m_typeTag);

        //  +0x78 is the endpoint's own relation-list head, the node
        //  SRelationLink::InsertAfter splices onto.
        if (object == 0)
        {
            relation->AttachEndpointA(object,
                                      (SRelationLink*)((char*)object + 0x78));
        }
        else
        {
            object = (object->m_kindTag == kAgileLTag) ? object : 0;
            relation->AttachEndpointA(object,
                                      (SRelationLink*)((char*)object + 0x78));
        }
    }
    else
    {
        DetachRelation(relation, 0);
    }
}
