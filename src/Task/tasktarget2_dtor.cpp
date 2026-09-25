//============================================================================
//  CTaskTarget2's own slot-1 DeleteSelf override, 0x6F28AA50 - own
//  translation unit, mirroring tasktarget_dtor.cpp's own placement rule.
//
//  Unlike CTaskTarget's own DeleteSelf (which makes a real out-of-line call
//  into its own, separately-addressed ~CTaskTarget() because that
//  destructor has real work to do - release m_target), CTaskTarget2 adds no
//  field of its own that needs destroying beyond what ~CTaskTarget already
//  tears down: the compiler-generated ~CTaskTarget2() has nothing to do
//  itself, so it collapses down to a plain call straight through to
//  CTaskTarget's own real destructor (0x6F2802C0, tasktarget.h) - the same
//  shape task.h's own DeleteSelf takes into ~CAgentWar3(), just one level
//  further down this chain and by a real out-of-line `call` rather than an
//  inlined tail (~CTaskTarget is SEH-framed, so it can never be inlined
//  anywhere, this TU included).
//============================================================================
#include "tasktarget2.h"

//  Storm.dll ordinal 403.
void __stdcall SMemFree(void* ptr, const char* logfilename, int logline,
                        unsigned int flags);

static const char kDelete[] = "delete";

//----------------------------------------------------------------------------
//  0x6F28AA50 - slot 1, DeleteSelf.  `retn 4`.  Same explicit
//  `this != 0` null-check-in-front-of-the-free shape CTaskTarget's own
//  DeleteSelf has (tasktarget_dtor.cpp) - this one also makes a real
//  out-of-line call first, so it cannot assume `this` is non-null the way
//  the plain-inlined-tail-call shapes elsewhere in this family do.
//----------------------------------------------------------------------------
CTaskTarget2* CTaskTarget2::DeleteSelf(int flags)
{
    this->CTaskTarget::~CTaskTarget();
    if ((flags & 1) != 0 && this != 0)
        SMemFree(this, kDelete, -1, 0);
    return this;
}
