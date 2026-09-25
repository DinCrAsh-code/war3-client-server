//============================================================================
//  CTaskTarget's own destructor and its slot-1 DeleteSelf override, both
//  clustered at 0x6F2802xx/0x6F28AAxx.
//
//  Own translation unit, separate from tasktarget_identity.cpp/
//  tasktarget_save.cpp: unlike CTask's/CTaskParam's own slot 1 (agentwar3_
//  dtor.cpp), the shipped DeleteSelf here makes a *real* out-of-line
//  `call sub_6F2802C0` rather than inlining ~CAgentWar3's vtable-stamp-and-
//  tail-call shape - because ~CTaskTarget itself has real work to do
//  (release m_target) and is therefore a real, separately-addressed,
//  SEH-framed function of its own, not something for DeleteSelf's own
//  translation unit to make disappear by inlining.  Exactly the same split
//  CTaskPoint's own taskpoint_deleteself.cpp already establishes for
//  ~CTaskPoint/m_moveRequest - see tasktarget.h.
//============================================================================
#include "tasktarget.h"

//  Storm.dll ordinal 403.
void __stdcall SMemFree(void* ptr, const char* logfilename, int logline,
                        unsigned int flags);

static const char kDelete[] = "delete";

//----------------------------------------------------------------------------
//  0x6F2802C0 - CTaskTarget::~CTaskTarget.  THUNK: releases m_target
//  (test/decrement-refcount/dispatch-slot-0-if-zero, the same shape
//  presenceref.h's own TRefCnt-chain release already documents) then tail-
//  calls into ~CAgentWar3 (stamp ??_7CAgentWar3@@6B@, call sub_6F0017F0) -
//  but behind a real `__except_handler4`-shaped SEH frame
//  (`push -1 / push offset SEH_6F2802C0 / mov eax, fs:0 / ...`) this repo's
//  fixed `/GS- /EHs-c-` toolchain cannot emit for any function, the same
//  reason ConstructCTaskTarget (tasktargetinstancegenerator.cpp) is a THUNK
//  rather than a hand-written body.
//----------------------------------------------------------------------------
#define TASKTARGET_DTOR_THUNK(addr) \
    {                               \
        __asm { mov eax, addr }    \
        __asm { jmp eax }          \
    }

__declspec(naked) CTaskTarget::~CTaskTarget() TASKTARGET_DTOR_THUNK(0x6F2802C0)

//----------------------------------------------------------------------------
//  0x6F28AA20 - slot 1, DeleteSelf.  `retn 4`.  Destroy, then free only if
//  both the caller asked (`flags & 1`) AND `this` is non-null - the same
//  explicit null-check-in-front-of-the-free shape CItem::DeleteSelf has
//  (agentwar3_dtor.cpp's own header comment: "a real null check in front
//  of the free, not part of the flags test"), not the plain
//  CWidget/CSelectable/CAgentWar3/CWar3Image/CTask shape (which has none -
//  those bodies are inlined into ~CAgentWar3's own tail call, so the
//  compiler already knows `this` is non-null by the time it gets there;
//  this one makes a real out-of-line call into ~CTaskTarget first, so it
//  cannot make the same assumption).
//----------------------------------------------------------------------------
CTaskTarget* CTaskTarget::DeleteSelf(int flags)
{
    this->CTaskTarget::~CTaskTarget();
    if ((flags & 1) != 0 && this != 0)
        SMemFree(this, kDelete, -1, 0);
    return this;
}
