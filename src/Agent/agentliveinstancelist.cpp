//============================================================================
//  0x6F48E610 - see agentliveinstancelist.h.  Own translation unit: called
//  from Agent/prclustergroup_slots2.cpp's own Apply and Load, and keeping
//  a one-instruction-shaped callee out of line the same way
//  Agent/agentbasechain_staticpools.cpp's own pool getters are kept out of
//  the ReturnToStaticPool bodies that call them (CLAUDE.md's "put a callee
//  in its own TU" rule) - this body is small enough that a same-TU /Ob2
//  build could otherwise inline it into both callers and reshuffle their
//  own register allocation for a reason that has nothing to do with
//  correctness.
//============================================================================
#include "agentliveinstancelist.h"

void SAgentLiveInstanceListHead::InsertAtHead(void* node)
{
    char* n = (char*)node;
    char* self = (char*)this;
    *(void**)(n + 4) = this;

    *(void**)(n + 8) = *(void**)(self + 8);

    void* first = *(void**)(self + 8);
    if (first != 0)
        *(void**)((char*)first + 4) = n;

    *(void**)(self + 8) = n;
}
