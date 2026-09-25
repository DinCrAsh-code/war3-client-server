//============================================================================
//  0x6F022610 - CAgentField's own slot-1 override, the scalar deleting
//  destructor. ~CAgentField() is implicit and trivial (this class adds no
//  fields of its own, only the four vtable slots in agentfield_core.cpp
//  plus this one), so it collapses into CPresenceRef's own base destructor
//  one level up - same "reset the vtable to TRefCnt's own, conditionally
//  SMemFree('delete')" 13-instruction shape every level of this chain
//  repeats (Agent/refcnt_deleteself.cpp, Agent/presenceref_deleteself.cpp,
//  Pathfinding/positiontrackdeleteself.cpp and siblings), different
//  address, because each class needs its own slot 1 to invoke the right
//  derived destructor.
//
//  Own translation unit, not agentfield_thunk.cpp (which now carries
//  nothing - see that file's own updated comment) - same "keep the
//  destructor call a real out-of-line call" reasoning as
//  agent_deleteself.cpp/observer_deleteself.cpp.
//============================================================================
#include "agentfield.h"

//  Storm.dll ordinal 403.
void __stdcall SMemFree(void* ptr, const char* logfilename, int logline,
                        unsigned int flags);

static const char kDelete[] = "delete";

CAgentField* CAgentField::DeleteSelf(int flags)
{
    this->CAgentField::~CAgentField();
    if (flags & 1)
        SMemFree(this, kDelete, -1, 0);
    return this;
}
