//============================================================================
//  CPresenceRef's own two vtable slots, both real now:
//
//  0x6F02F010 - slot 1, DeleteSelf. Byte-identical in shape to every other
//  slot-1 override in this TRefCnt chain (Agent/refcnt_deleteself.cpp,
//  Pathfinding/positiontrackdeleteself.cpp and siblings) - ~CPresenceRef is
//  implicit and trivial (CPresenceRef adds no members with destructors of
//  their own, just the two plain ints in presenceref.h), so it collapses
//  into TRefCnt's own base destructor: reset the vtable to TRefCnt's own,
//  conditionally SMemFree("delete").
//
//  0x6F021900 - slot 2, Detach. The base version of the same slot
//  CAgentField::Detach (agentfield_core.cpp) overrides: no LookupHandle
//  resolve, no AgentSlot5CGate notification, just the unconditional
//  m_handle/m_typeTag clear those fields share. See presenceref.h's own
//  header comment for why this has to keep the name `Detach` rather than
//  something more descriptive of "just the clear".
//
//  Own translation unit, not presenceref_thunks.cpp (which still carries
//  nothing now that both slots are real - see that file's own updated
//  comment) - same "keep the destructor call a real out-of-line call"
//  reasoning as agent_deleteself.cpp/observer_deleteself.cpp.
//============================================================================
#include "presenceref.h"

//  Storm.dll ordinal 403.
void __stdcall SMemFree(void* ptr, const char* logfilename, int logline,
                        unsigned int flags);

static const char kDelete[] = "delete";

CPresenceRef* CPresenceRef::DeleteSelf(int flags)
{
    this->CPresenceRef::~CPresenceRef();
    if (flags & 1)
        SMemFree(this, kDelete, -1, 0);
    return this;
}

void CPresenceRef::Detach()
{
    m_typeTag = m_handle = (unsigned int)-1;
}
