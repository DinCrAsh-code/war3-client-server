//============================================================================
//  CTaskPoint's own destructor and its slot-1 DeleteSelf override, both
//  clustered at 0x6F2802xx/0x6F28A9xx.
//
//  Own translation unit, separate from taskpoint_identity.cpp/
//  taskpoint_save.cpp: unlike CTask's/CTaskParam's own slot 1 (agentwar3_
//  dtor.cpp), the shipped DeleteSelf here makes a *real* out-of-line
//  `call sub_6F280250` rather than inlining ~CAgentWar3's vtable-stamp-and-
//  tail-call shape - because ~CTaskPoint itself has real work to do
//  (release m_moveRequest) and is therefore a real, separately-addressed,
//  SEH-framed function of its own, not something for DeleteSelf's own
//  translation unit to make disappear by inlining. See taskpoint.h.
//============================================================================
#include "taskpoint.h"

//  Storm.dll ordinal 403.
void __stdcall SMemFree(void* ptr, const char* logfilename, int logline,
                        unsigned int flags);

static const char kDelete[] = "delete";

//----------------------------------------------------------------------------
//  0x6F280250 - CTaskPoint::~CTaskPoint.  THUNK: releases m_moveRequest
//  (test/decrement-refcount/dispatch-slot-0-if-zero, the same shape
//  presenceref.h's own TRefCnt-chain release already documents) then tail-
//  calls into ~CAgentWar3 (stamp ??_7CAgentWar3@@6B@, call sub_6F0017F0) -
//  but behind a real `__except_handler4`-shaped SEH frame
//  (`push -1 / push offset SEH_6F280250 / mov eax, fs:0 / ...`) this repo's
//  fixed `/GS- /EHs-c-` toolchain cannot emit for any function, the same
//  reason ConstructCTaskPoint (taskpointinstancegenerator.cpp) is a THUNK
//  rather than a hand-written body.
//----------------------------------------------------------------------------
#define TASKPOINT_DTOR_THUNK(addr)  \
    {                               \
        __asm { mov eax, addr }     \
        __asm { jmp eax }           \
    }

__declspec(naked) CTaskPoint::~CTaskPoint() TASKPOINT_DTOR_THUNK(0x6F280250)

//----------------------------------------------------------------------------
//  0x6F28A990 - slot 1, DeleteSelf.  `retn 4`.  Standard shape: destroy,
//  conditionally free, return `this`.
//----------------------------------------------------------------------------
CTaskPoint* CTaskPoint::DeleteSelf(int flags)
{
    this->CTaskPoint::~CTaskPoint();
    if ((flags & 1) != 0 && this != 0)
        SMemFree(this, kDelete, -1, 0);
    return this;
}
