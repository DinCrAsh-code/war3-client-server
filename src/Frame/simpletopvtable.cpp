//============================================================================
//  CSimpleTop's own small vtable-slot overrides (slots 1, 3, 10, 14, 16, 45
//  - docs/targets/vtables/CSimpleTop.txt, 0x6F96E2FC).  Slots 11 and 12,
//  the two large ones a prior session left naked thunks, are reconstructed
//  now too - Frame/simpletopflush.cpp and
//  Frame/simpletoprebuilddirtybatches.cpp, declared on the class here.
//
//  Slot 10's own reasoning ties into docs/notes/pause-mouse-capture.md:
//  it is the strongest structural candidate a prior session found for
//  "which method touches m_pCapturedFrame", and it is a read-only query.
//  No write site was found anywhere in this class's own 57 slots (calltree
//  depth 3 on every one of them) - see that file's own dated section.
//  Slots 11 and 12 are now checked the same way, at their own full depth-3
//  closures, and the answer is the same: neither writes it either.  Both
//  classes this whole pause-mouse-capture investigation ever named
//  (CSimpleButton's 29 slots, CSimpleTop's 57) are fully accounted for.
//============================================================================
#include "frame.h"
#include "framethunks.h"

#define SIMPLETOP_THUNK(addr)    \
    {                            \
        __asm { mov eax, addr }  \
        __asm { jmp eax }        \
    }

//----------------------------------------------------------------------------
//  Slot 1 (+0x04) - the deleting destructor: call the real (non-deleting)
//  body, then free the block when bit 0 of `flags` says so.  Returns `this`
//  unchanged (no this-adjustment: CObserver sits at offset 0 through
//  CFrame's own primary base).
//----------------------------------------------------------------------------
CObserver* CSimpleTop::DeleteSelf(int flags)
{
    RealDestructorBody();
    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);
    return (CObserver*)this;
}

//  0x6F60C0F0 - the real, SEH-framed destructor body.  Naked thunk; see
//  frame.h's own comment on the declaration for why.
__declspec(naked) void CSimpleTop::RealDestructorBody() SIMPLETOP_THUNK(0x6F60C0F0)

//----------------------------------------------------------------------------
//  Slot 3 (+0x0C) - forward to my own owner's copy of this slot, or answer
//  0 if I have none.  m_pOwner is CLayer's own field.
//----------------------------------------------------------------------------
int CSimpleTop::Method_0x0C(const SAgentMessage* msg)
{
    CLayer* owner = m_pOwner;
    if (owner)
        return owner->Method_0x0C(msg);
    return 0;
}

//----------------------------------------------------------------------------
//  Slot 10 (+0x28) - "is anything captured".  The one stack argument every
//  sibling slot in this vtable region also carries is never read here.
//----------------------------------------------------------------------------
int CSimpleTop::Method_0x28(int /*unused*/) const
{
    return m_pCapturedFrame != 0;
}

//----------------------------------------------------------------------------
//  Slots 11/12 (+0x2C/+0x30) - reconstructed.  See frame.h's own comment on
//  each declaration for the full breakdown, and Frame/simpletopflush.cpp /
//  Frame/simpletoprebuilddirtybatches.cpp for the real bodies.
//----------------------------------------------------------------------------
//  Slots 14/16 (+0x38/+0x40) - CSimpleTop's own copy of the trivial
//  "tail-jump my own slot 4 (Method_0x10)" forwarder.
//----------------------------------------------------------------------------
int CSimpleTop::Method_0x38(SAgentMessage* msg)
{
    return Method_0x10(msg);
}

int CSimpleTop::Method_0x40(SAgentMessage* msg)
{
    return Method_0x10(msg);
}

//----------------------------------------------------------------------------
//  Slot 27 (+0x6C) - releases m_120, zeroes it, calls the real-but-thunked
//  CLayoutFrame-level teardown+requeue helper against this frame's own
//  +0xB4 sub-object, sets m_11C = 1.  Left a whole naked thunk (rather than
//  a real outer body calling a thunked helper) because that helper's own
//  argument is `this+0xB4` cast to a type this batch has not pinned down -
//  see frame.h's own comment on the declaration.
//----------------------------------------------------------------------------
__declspec(naked) void CSimpleTop::Method_0x6C() SIMPLETOP_THUNK(0x6F607240)

//----------------------------------------------------------------------------
//  Slot 45 (+0xB4) - mark every one of the 50 dirty-grid cells dirty.
//----------------------------------------------------------------------------
void CSimpleTop::MarkAllDirty()
{
    for (int i = 0; i < 50; ++i)
        m_dirty[i] = 1;
}
