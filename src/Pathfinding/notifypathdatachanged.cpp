//============================================================================
//  0x6F4A7520 - NotifyPathDataChanged: re-register a path trace's two grid
//  boxes at wherever the move has got to by now.  A naked redirect in
//  unreconstructed_thunks.cpp until now; pathrefflags.cpp is what calls it.
//
//  Three steps and no state of its own: how far the move has travelled,
//  the point that far along the line, and the same {radius, point} pair
//  handed to the pathing grid and then to the collision grid.  All four
//  callees are CPathTrace's own, which is what says the argument is a
//  CPathTrace - the same conclusion CPathRef::ToWorldVec3 reached about
//  its own resolved object.
//
//  **The radius is passed by address and the address is computed once.**
//  The shipped code keeps `esi+0x90` in edi across both calls
//  (`lea edi,[esi+90h]` / `push edi` twice), which is what one named
//  pointer gives; `&trace->m_radius` written out at each call site makes
//  MSVC re-`lea` for the second one.
//
//  Declared `__fastcall` taking the trace in ecx, which is what the
//  shipped `mov esi, ecx` says and what its `retn 0` allows.
//
//  Own translation unit: four real calls out of it, and pathrefflags.cpp
//  reaches it with a fifth.
//============================================================================
#include "game.h"

void __fastcall NotifyPathDataChanged(void* obj)
{
    CPathTrace* trace = (CPathTrace*)obj;

    //  Through the reference GetTravelDistance returned rather than a named
    //  copy - the shipped code pushes that pointer straight on.  Same idiom
    //  as CPathTrace::RampFractionAt (pathtraceramp.cpp).
    const CFloat& travelled = trace->GetTravelDistance();
    CGridVec2 pos = trace->PointAtDistance(travelled);

    const CFloat* radius = &trace->m_radius;
    trace->UpdatePathGridBox(radius, &pos);
    trace->UpdateCollisionGridBox(radius, &pos);
}
