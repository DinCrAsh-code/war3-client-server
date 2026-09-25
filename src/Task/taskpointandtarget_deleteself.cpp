//============================================================================
//  CTaskPointAndTarget's own destructor and its slot-1 DeleteSelf override,
//  both clustered at 0x6F2884xx/0x6F28A9xx.
//
//  Own translation unit, separate from taskpointandtarget_identity.cpp/
//  taskpointandtarget_save.cpp: unlike CTask's/CTaskParam's own slot 1
//  (agentwar3_dtor.cpp), the shipped DeleteSelf here makes a *real*
//  out-of-line `call sub_6F288460` rather than inlining ~CAgentWar3's
//  vtable-stamp-and-tail-call shape - because ~CTaskPointAndTarget itself
//  has real work to do (release m_target, then tail into
//  CTaskPoint::~CTaskPoint to release m_moveRequest) and is therefore a
//  real, separately-addressed, SEH-framed function of its own, not
//  something for DeleteSelf's own translation unit to make disappear by
//  inlining.  Same split CTaskPoint's own taskpoint_deleteself.cpp and
//  CTaskTarget's own tasktarget_dtor.cpp already establish. See
//  ctaskpointandtarget.h.
//============================================================================
#include "ctaskpointandtarget.h"

//  Storm.dll ordinal 403.
void __stdcall SMemFree(void* ptr, const char* logfilename, int logline,
                        unsigned int flags);

static const char kDelete[] = "delete";

//----------------------------------------------------------------------------
//  0x6F288460 - CTaskPointAndTarget::~CTaskPointAndTarget.  THUNK: releases
//  m_target (test/decrement-refcount/dispatch-slot-0-if-zero, the same
//  shape presenceref.h's own TRefCnt-chain release already documents) then
//  makes a real out-of-line `call sub_6F280250` into CTaskPoint::~CTaskPoint
//  (itself a THUNK, taskpoint_deleteself.cpp - releases m_moveRequest and
//  tail-calls into ~CAgentWar3) - but behind a real `__except_handler4`
//  -shaped SEH frame (`push -1 / push offset SEH_6F288460 / mov eax, fs:0 /
//  ...`) this repo's fixed `/GS- /EHs-c-` toolchain cannot emit for any
//  function, the same reason ConstructCTaskPointAndTarget
//  (taskpointandtargetinstancegenerator.cpp) is a THUNK rather than a
//  hand-written body.
//----------------------------------------------------------------------------
#define TASKPOINTANDTARGET_DTOR_THUNK(addr)  \
    {                                        \
        __asm { mov eax, addr }              \
        __asm { jmp eax }                    \
    }

__declspec(naked) CTaskPointAndTarget::~CTaskPointAndTarget() TASKPOINTANDTARGET_DTOR_THUNK(0x6F288460)

//----------------------------------------------------------------------------
//  0x6F28A9F0 - slot 1, DeleteSelf.  `retn 4`.  Standard shape: destroy,
//  conditionally free, return `this`.
//----------------------------------------------------------------------------
CTaskPointAndTarget* CTaskPointAndTarget::DeleteSelf(int flags)
{
    this->CTaskPointAndTarget::~CTaskPointAndTarget();
    if ((flags & 1) != 0 && this != 0)
        SMemFree(this, kDelete, -1, 0);
    return this;
}
