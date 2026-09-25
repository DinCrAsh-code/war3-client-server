//============================================================================
//  0x6F308CA0 - CCameraWar3::SetCursorMode: enter one cursor mode, over
//  `blend` seconds.  `retn 8`.
//
//  `mode` is clamped through an unsigned compare (`(unsigned)mode >= 2`
//  clamps to 2), which also happens to fold a negative `mode` up to itself
//  unchanged via a branchless `xor/cmp/sbb/and` - the shipped compiler's own
//  way of writing "the low bits of an int stay the low bits of an int" that
//  survives whatever earlier form of the clamp the source really had.  A
//  mode equal to what is already active is a no-op; only a genuine change
//  stores the new mode and starts the blend (0x6F306B80, out of this
//  batch - the transition driver itself).
//
//  Own translation unit: ClearCursorMode (cameraclearcursormode.cpp) calls
//  this one for real, and this one calls 0x6F306B80 for real.
//============================================================================
#include "gameui.h"

//  `this`-in-ecx with two stack arguments is plain __thiscall, but this
//  compiler only accepts that keyword on an actual member function - so
//  0x6F306B80 (out of this batch) is declared as a member of CCameraWar3
//  itself rather than a free function.  Real, not a stand-in: the shipped
//  call site's `this` is genuinely the same camera.

void CCameraWar3::SetCursorMode(int mode, float blend)
{
    unsigned int clamped = 0 - (unsigned int)(0 < mode);
    clamped &= (unsigned int)mode;
    if (clamped >= 2)
        clamped = 2;

    if ((int)clamped == m_cursorMode)
        return;

    m_cursorMode = clamped;
    StartCursorModeTransition(blend, 0);
}

//  0x6F306B80 - `retn 8` (verified against its own raw bytes: `pop esi /
//  add esp,8 / retn 8`).  Out of this batch and not reconstructed anywhere
//  in this tree; a naked ADDR_THUNK-shaped redirect stands in for it,
//  declared as a real CCameraWar3 member (gameui.h) rather than a bodyless
//  free-function extern, so the real link (`tools/link_check.py`) has
//  something to bind SetCursorMode's own call to.
__declspec(naked) void CCameraWar3::StartCursorModeTransition(float, int)
{
    __asm { mov eax, 0x6F306B80 }
    __asm { jmp eax }
}
