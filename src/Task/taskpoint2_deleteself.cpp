//============================================================================
//  CTaskPoint2's slot-1 DeleteSelf override, 0x6F28A9C0.
//
//  Unlike CTaskPoint's own slot 1 (taskpoint_deleteself.cpp), this one does
//  *not* need its own out-of-line destructor: CTaskPoint2 adds only two
//  FloatMini sub-objects, whose own destructors are trivial vtable-stamping
//  no-ops that /O2 drops as dead stores (same shape CTaskRotate's own
//  FloatMini field already documents, taskrotate.h) - so the
//  compiler-generated ~CTaskPoint2() reduces to a tail-call straight into
//  ~CTaskPoint() (0x6F280250, taskpoint.h - a `__declspec(naked)` THUNK,
//  never inlinable regardless of translation unit either way), which is
//  exactly the one real out-of-line `call` this body makes.
//============================================================================
#include "taskpoint2.h"

//  Storm.dll ordinal 403.
void __stdcall SMemFree(void* ptr, const char* logfilename, int logline,
                        unsigned int flags);

static const char kDelete[] = "delete";

//----------------------------------------------------------------------------
//  0x6F28A9C0 - slot 1, DeleteSelf.  `retn 4`.  Standard shape: destroy,
//  conditionally free, return `this`.
//----------------------------------------------------------------------------
CTaskPoint2* CTaskPoint2::DeleteSelf(int flags)
{
    this->CTaskPoint2::~CTaskPoint2();
    if ((flags & 1) != 0 && this != 0)
        SMemFree(this, kDelete, -1, 0);
    return this;
}
