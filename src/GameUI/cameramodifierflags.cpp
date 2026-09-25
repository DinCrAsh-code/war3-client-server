//============================================================================
//  0x6F305580 - CCameraWar3::ApplyModifierFlags.
//
//  Two independent jobs behind one four-argument call, both keyed off the
//  low nibble / whole value of the same `flags` argument:
//
//   1. each of the four low bits installs (or clears) one of the two
//      paired ramp/offset modifiers at `index` - see InstallRampModifier /
//      InstallOffsetModifier (gameui.h; both thunked, genuine
//      __except_handler4 SEH frames this build cannot reproduce);
//   2. when `suppressCursor` is nonzero, `flags` is read a *second* time,
//      whole, as a 1..10 selector into the world frame's own cursor-state
//      table (CWorldFrameWar3::SetCursorState, already reconstructed) -
//      values 3 and 7 fall through to the same "no explicit mode, reuse
//      whatever ebp already is" default the initial `xor ebp, ebp` gives
//      every out-of-range value.  Nothing in this closure calls
//      ApplyModifierFlags with a `flags` that exercises this second
//      reading with any particular semantic meaning attached, so the
//      cursor-mode constants below are transcribed as literals, not named.
//
//  `mov [esi+1C0h], ebp`/`, 1` writes CWorldFrameWar3::m_suppressed
//  directly (gameui.h) rather than through a setter - there is not one in
//  this closure either.
//============================================================================
#include "gameui.h"

void CCameraWar3::ApplyModifierFlags(int flags, int index, float duration,
                                     int suppressCursor)
{
    if (flags & 1)
        InstallOffsetModifier(index, 1, duration);
    if (flags & 2)
        InstallOffsetModifier(index, 0, duration);
    if (flags & 4)
        InstallRampModifier(index, 0, duration);
    if (flags & 8)
        InstallRampModifier(index, 1, duration);

    CGameUI* ui = (CGameUI*)GetGameUI(1, 0);
    CWorldFrameWar3* wf = ui->m_pWorldFrameWar3;

    if (suppressCursor == 0)
        return;

    if (index == 0)
    {
        wf->m_suppressed = 0;
        wf->PopCursorMode();
        return;
    }

    int cursorMode;
    switch (flags)
    {
    case 1:  cursorMode = 0x0C; break;
    case 2:  cursorMode = 0x0D; break;
    case 4:  cursorMode = 0x0A; break;
    case 5:  cursorMode = 0x0E; break;
    case 6:  cursorMode = 0x10; break;
    case 8:  cursorMode = 0x0B; break;
    case 9:  cursorMode = 0x0F; break;
    case 10: cursorMode = 0x11; break;
    default: cursorMode = 0; break;
    }

    wf->ResetTrack();
    wf->PushCursorMode(0, 0, 1);
    wf->SetCursorState(cursorMode, 0, 0);
    wf->m_suppressed = 1;
}
