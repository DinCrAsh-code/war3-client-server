//============================================================================
//  0x6F39C7E0 - CBuildMode::CBuildMode.
//
//  A CObserver with one owned frame.  The frame is 0x19C bytes tagged
//  ".\\CBuildMode.cpp" line 0x24 - which is what names the module and fixes
//  CBuildFrame's size - built with the same parent this constructor is
//  handed, parked at +0x0C with +0x10 cleared, and then asked one question
//  through its own vtable.
//
//  That question is a hand-written dispatch and it is *not* judgeable by
//  tools/vtable_dispatch_audit.py: the receiver is the frame, not `this`.
//  What fixes its argument count is the shipped call site at 0x6F39C86A -
//  `mov ecx, eax` and `call eax` with nothing pushed between the vtable load
//  and the call - so slot 0xD0 takes no stack argument here, and a slot that
//  cleaned any would already have unbalanced the shipped stack.  There is no
//  committed vtable for CBuildFrame to check the slot's real target against.
//
//  The dispatch runs on the allocation's result whether or not it succeeded,
//  which is the shipped body: the null test placement-new puts round the
//  construction guards only the construction.
//
//  Score capped by the __except_handler4 frame the shipped build puts round
//  a constructor with a base and an owned allocation to unwind, exactly as
//  GameUI/timeofdayindicatorctor.cpp's does.
//
//  Own translation unit: the frame constructor it calls is a real call in a
//  neighbouring module.
//============================================================================
#include "buildmode.h"
#include "storm.h"

//  Slot 0xD0 / index 52 of CBuildFrame's own vtable - "you are built; take
//  it from here".  Nothing in this tree reaches the body behind it.
typedef void (__thiscall *BuildFrameReadyFn)(void* self);

static const char kBuildModeFile[] = ".\\CBuildMode.cpp";

CBuildMode::CBuildMode(CFrame* parent)
{
    void* memory = SMemAlloc(0x19C, kBuildModeFile, 0x24, 0);

    m_pFrame = memory ? new (memory) CBuildFrame(parent) : 0;
    m_field10 = 0;

    ((BuildFrameReadyFn)(*(void***)m_pFrame)[0xD0 / 4])(m_pFrame);
}
