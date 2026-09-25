//============================================================================
//  0x6F4A73B0 - CPathTrace::PublishVelocityDelta.
//
//  Three instructions of its own and two calls, and the order of the two is
//  the entire function: settle the position the *old* velocity has earned up
//  to now (RecomputeOrigin with a zero delta - `origin += direction *
//  travelled`, which is a no-op on the origin only if no time has passed),
//  and only then let AddVelocity change the velocity.  Reversed, every unit
//  would retroactively have been travelling at its new speed since its last
//  sync sample.
//
//  Was a redirect (GameUI/gameui_thunks.cpp's `SPositionPublisher::Publish`);
//  that thunk is deleted in the same change, so one shipped address still
//  carries exactly one C++ name.  The sink at the other end of it
//  (Widget/positionsinksettarget.cpp) publishes its target as a *difference*
//  from the point it last published, which is what a velocity delta is, and
//  the ecx it hands over is the CPathTrace this call proves it to be: the
//  first call in this body is CPathTrace::RecomputeOrigin on that same
//  receiver, unaltered.
//
//  Own translation unit: RecomputeOrigin (Sync/timesyncphysics.cpp) and
//  AddVelocity (pathtraceaddvelocity.cpp) are both real `call`s here, and
//  all three shipped in the same 0x6F4A7xxx module.
//============================================================================
#include "game.h"

void CPathTrace::PublishVelocityDelta(const CGridVec2* delta, int notify)
{
    //  One constructor and not two assignments: the shipped code reads
    //  g_CFloatZero once and stores that one register into both halves of
    //  the pair, low then high, which is what constructing the vector emits
    //  and what naming a zero CFloat first does not.
    CGridVec2 zero(g_CFloatZero, g_CFloatZero);

    RecomputeOrigin(&zero);
    AddVelocity(delta, notify);
}
