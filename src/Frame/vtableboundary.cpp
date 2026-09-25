//============================================================================
//  Bodies for the four slot-0 virtuals the frame classes declare and this
//  batch does not reconstruct.
//
//  Each of those classes needs the `virtual` to keep its layout - the vptr at
//  +0 is part of the record, and taking it away moves every field after it,
//  which costs no compile error and no score on the class itself while
//  silently breaking every function that reads one of its fields.  But a
//  declared-and-undefined virtual is an unresolved external the moment MSVC
//  emits the class's vtable, and verify.py cannot see that because it never
//  links; tools/link_check.py is what does.
//
//  So each body is a dispatch through whatever is really in slot 0.  That is
//  not a recursion: the objects these run on are allocated and stamped by
//  shipped code, so slot 0 holds the shipped implementation, and nothing in
//  this repo ever installs the vtable MSVC emits here.  It is the same
//  boundary shape the hand-written slot dispatches in frame.h use, written as
//  a body rather than at the call site because the call sites go through the
//  `virtual`.
//============================================================================
#include "frame.h"
#include "framethunks.h"

namespace {

inline void DispatchSlotZero(void* self, int deleteFlags)
{
    typedef void (__thiscall *Fn)(void*, int);
    ((Fn)(*(void***)self)[0])(self, deleteFlags);
}

}  // namespace

void FRAMENODE::Destroy(int deleteFlags)     { DispatchSlotZero(this, deleteFlags); }
void CFramePoint::Destroy(int deleteFlags)   { DispatchSlotZero(this, deleteFlags); }
void CStatus::Destroy(int deleteFlags)       { DispatchSlotZero(this, deleteFlags); }
void CLayoutFrame::DestroyLayout(int deleteFlags) { DispatchSlotZero(this, deleteFlags); }
