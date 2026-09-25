//============================================================================
//  0x6F02F4F0 - CRelation's own slot-1 override, the scalar deleting
//  destructor. ~CRelation() is implicit and trivial (this class adds no
//  fields of its own, only the three vtable slots relation.h declares), so
//  it collapses into CAgentField's own base destructor one level up - same
//  "reset the vtable to TRefCnt's own, conditionally SMemFree('delete')"
//  13-instruction shape every level of this chain repeats
//  (Agent/refcnt_deleteself.cpp, Agent/presenceref_deleteself.cpp,
//  Agent/agentfield_deleteself.cpp and siblings), different address, because
//  each class needs its own slot 1 to invoke the right derived destructor.
//
//  Own translation unit - same "keep the destructor call a real out-of-line
//  call" reasoning as agentfield_deleteself.cpp/presenceref_deleteself.cpp.
//============================================================================
#include "relation.h"

//  Storm.dll ordinal 403.
void __stdcall SMemFree(void* ptr, const char* logfilename, int logline,
                        unsigned int flags);

static const char kDelete[] = "delete";

CRelation* CRelation::DeleteSelf(int flags)
{
    this->CRelation::~CRelation();
    if (flags & 1)
        SMemFree(this, kDelete, -1, 0);
    return this;
}

//  0x6F02F4C0 / 0x6F02F4D0 - genuine bare no-op bodies (`retn 4` / `retn 8`).
//  See relation.h's own comment on why these are defined here, out of line,
//  rather than inline in the class body.
void CRelation::AttachRelationEndpointB(void* context)
{
}

void CRelation::AttachRelationEndpointA(void* context, CAgentRelation* relation)
{
}
