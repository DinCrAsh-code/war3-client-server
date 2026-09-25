//============================================================================
//  0x6F3B2360 - SJassRectCentre::MoveTo, the body behind `MoveRectTo` and
//  `MoveRectToLoc`: slide a rect so that its centre lands on a given point.
//
//  `retn 8`, `this` being the rect.  The point arrives *by value* as two
//  stack dwords, which is why the two natives carve eight bytes off the
//  stack and write the pair into them rather than pushing twice.
//
//  Everything the body does is done to the four-word bounds block at +0x24,
//  never to the rect itself: the shipped code forms `lea esi,[ecx+24h]` once
//  and hands that same receiver to both of its callees.  The move is the
//  obvious one - ask the block where its centre is, subtract that from the
//  requested centre, and offset the block by the difference - and both
//  subtractions go through the software float at 0x6F6EEEF0, not the FPU.
//
//  Two things the disassembly settles rather than the shape:
//
//   * GetCentre's *return value* is used.  The shipped code keeps eax
//     (`mov edi,eax`) and reads both halves of the centre back through it
//     rather than through the buffer it just passed, so the buffer is named
//     once and the pointer once - reading `centre[0]` / `centre[1]` directly
//     loads them off `esp` and loses the pointer;
//
//   * the y difference is computed *first*.  MSVC evaluates a constructor's
//     arguments right to left, so `SJassPoint(x - centre[0], y - centre[1])`
//     is what issues the two calls in the shipped order and then copies the
//     two results into the outgoing point x-half first.
//
//  Own translation unit in the 0x6F3B1xxx/0x6F3B2xxx rect-geometry module:
//  both of its callees live in that same module and it issues a genuine
//  `call` to each, so they must not be in a position to be inlined into it.
//============================================================================
#include "jassnatives.h"

void SJassRectCentre::MoveTo(SJassPoint point)
{
    //  The bounds block, which is what both callees take as `this`.
    SJassRectCentre* bounds = (SJassRectCentre*)((char*)this + 0x24);

    int centre[2];
    const int* at = bounds->GetCentre(centre);

    SJassPoint delta(point.m_x - *(const CFloat*)&at[0],
                     point.m_y - *(const CFloat*)&at[1]);

    bounds->OffsetBy(&delta);
}
