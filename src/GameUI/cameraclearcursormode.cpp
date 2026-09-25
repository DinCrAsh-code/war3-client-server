//============================================================================
//  0x6F309690 - CCameraWar3::ClearCursorMode: leave whatever cursor mode is
//  active, with no blend time.
//
//  Eight instructions.  The zero blend is a *float* stack argument - the
//  shipped `push ecx` reserves the slot and `fstp [esp]` fills it - which is
//  why SetCursorMode takes one and not an int.
//
//  Own translation unit: SetCursorMode is a real call into the same module.
//============================================================================
#include "gameui.h"

void CCameraWar3::ClearCursorMode()
{
    if (m_cursorMode != 0)
        SetCursorMode(0, 0.0f);
}
