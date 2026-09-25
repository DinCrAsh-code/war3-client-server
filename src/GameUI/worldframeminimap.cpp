//============================================================================
//  0x6F38C770 - CWorldFrameWar3::SetMinimap.
//
//  Eight instructions: remember the minimap and immediately advance its
//  refresh timers by 5000 seconds, which is how a minimap that has just been
//  installed repaints on the very next frame instead of up to 0.3s later.
//  Nothing else in the shipped body; the constant really is 5000.0f
//  (0x459C4000 at 0x6F936460).
//
//  Own translation unit: the AdvanceRefreshTimers call is real, and
//  CMinimap's own module is 0x6F33ABxx (GameUI/minimaprefresh.cpp).
//============================================================================
#include "gameui.h"

//  The one CMinimap member this module calls.  Declared locally for the
//  reason minimaprefresh.cpp records: the class has no shared header, and a
//  local declaration mangles identically, so the call still binds to the one
//  definition over there.
class CMinimap
{
public:
    void AdvanceRefreshTimers(float elapsed);   // 0x6F33AB00
};

void CWorldFrameWar3::SetMinimap(CMinimap* minimap)
{
    m_pMinimap = minimap;
    //  5000.0f, written as a literal.  The shipped image pools that constant
    //  at 0x6F936460 and shares it with the camera's default far plane, so
    //  funcmap.DATA prints the target's operand as `g_cameraDefaultFarZ`
    //  while this side emits its own anonymous pool entry - that naming is
    //  the single instruction this function does not match.  Spelling it as
    //  the extern instead costs *two*: an extern symbol is not hoisted ahead
    //  of the field store the way the pooled literal is.
    minimap->AdvanceRefreshTimers(5000.0f);
}
