//============================================================================
//  CCursorFrame - the screen-space frame CScreenFrame draws its cursor
//  through.  0x6F629170 is its constructor.
//
//  **Why this is not in frame.h.**  `src/GameUI/gameui.h` already declares a
//  class called `CCursorFrame`, and it is a *different* object: 0x624 bytes,
//  derived straight from CObserver, with a camera slot at +0x19C and a
//  cursor-mode word at +0x1AC.  This one is 0x1B0 bytes - the exact size
//  CScreenFrame::EnsureRenderObject allocates before calling 0x6F629170 -
//  and RTTI settles its shape completely: `??_R2CCursorFrame@@8` lists seven
//  classes, CCursorFrame, CSpriteFrame, CFrame, CLayer, CObserver, TRefCnt
//  and CLayoutFrame (the last at offset `LE@`, 0xB4), so CCursorFrame adds
//  no data of its own to CSpriteFrame at all.
//
//  So one of the two names was wrong, and the binary says which: batch A
//  inferred `CCursorFrame` for CGameUI's +0x3BC from the vtable symbol
//  0x6F97063C, while 0x6F629170's own `mov [esi], offset
//  ??_7CCursorFrame@@6B@` names *this* class outright.  That is settled now:
//  the PanCameraToTimedWithZ world-frame slice found CGameUI's +0x3BC being
//  built by 0x6F39A910, which stamps `??_7CWorldFrameWar3@@6B@` and tags its
//  allocations `".\\CWorldFrameWar3.cpp"`, and renamed batch A's class to
//  `CWorldFrameWar3` - so `CCursorFrame` now means only what is declared
//  below.  This header stays separate all the same: only src/Frame includes
//  it, and nothing in GameUI needs it.  See docs/targets/JASS_PauseGame.md,
//  batch D.
//============================================================================
#ifndef CURSORFRAME_H
#define CURSORFRAME_H

#include "frame.h"

class CCursorFrame : public CSpriteFrame
{
public:
    CCursorFrame(CFrame* owner, int a, int b);   // 0x6F629170
};

#endif
