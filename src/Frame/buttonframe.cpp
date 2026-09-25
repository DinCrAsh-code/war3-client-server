//============================================================================
//  0x6F6220C0 - CButtonFrame::CButtonFrame.
//
//  Its base is CControl (0x6F602DE0, controlframe.cpp), whose own name the
//  shipped code does spell - that constructor stamps `??_7CControl@@6B@`.
//  What is shipped fact here too is the pair of vtables
//  this constructor stamps, ??_7CButtonFrame@@6B@ at +0 and its `_0` twin at
//  +0xB4, which is what says the base is a two-vptr CFrame and not a
//  CSimpleFrame.
//
//  Its one statement is a *virtual* dispatch through CObserver slot 2 with
//  the frame's own event table (0x40060064) as both the message and its first
//  argument, and the frame itself as the second.
//
//  Score capped by the unified EH frame; see framecore.cpp.
//============================================================================
#include "frame.h"
#include "framethunks.h"


CButtonFrame::CButtonFrame(int a, int b, int c)
    : CControl(a, b, c)
{
    Dispatch(0x40060064, 0x40060064, this);
}
