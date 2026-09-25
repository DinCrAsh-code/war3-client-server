//============================================================================
//  0x6F4A7CB0 - SPositionSink::SetTarget.
//
//  Was a thunk (GameUI/gameui_thunks.cpp); the redirect is deleted in the
//  same change, so one shipped address still carries one C++ name.
//
//  The sink remembers the last point it published at +0x50, and a new target
//  is published as the *difference* from it, not as an absolute - which is
//  why the old point has to survive until after the publish and is only
//  overwritten at the very end.  With no publisher hanging off +0x40 the
//  whole publish is skipped and the field is still updated, so the next
//  target measures from the right place.
//
//  Two orderings are load-bearing:
//
//  * the y difference is computed first and the x difference second, but the
//    pair is packed {x, y}.  The shipped code computes them in that order
//    and then copies the *second* result into the low word of the pair; the
//    two subtractions are independent, so nothing but the shipped stream
//    says which comes first.
//  * each difference is `target - remembered`, in that order.  operator-
//    negates its second operand, and reversing the two would publish the
//    movement backwards.
//
//  Both differences are bound as references to what operator- filled rather
//  than copied into named CFloats, the idiom
//  Unit/unittrackedref_setvalue.cpp records: the shipped code carves one
//  stack slot per subtraction and one for the pair, and a named copy makes
//  MSVC reuse a slot and carve fewer.
//============================================================================
#include "positionmodifier.h"
#include "game.h"

//  0x6F4A7840 - Misc/trivialaccessors_04.cpp: the publisher at +0x40.
int __fastcall GetField0x40_6F4A7840(const void* self);

void SPositionSink::SetTarget(const CGridVec2* point)
{
    //  The publisher at +0x40 is a CPathTrace: 0x6F4A73B0, the only thing
    //  reached through it here, calls CPathTrace::RecomputeOrigin on the
    //  receiver it is handed unaltered (Pathfinding/pathtracepublishvelocity.cpp).
    CPathTrace* publisher = (CPathTrace*)GetField0x40_6F4A7840(this);

    if (publisher != 0)
    {
        //  The remembered point is bound to its own pointer: the shipped
        //  stream forms `&m_target` once into a callee-saved register and
        //  reaches both components off it, and going through the member
        //  twice makes MSVC re-form the address instead.
        CGridVec2* remembered = &m_target;

        //  One constructor call and not three statements: MSVC evaluates the
        //  arguments right to left, so the y difference is computed first and
        //  the x difference second - which is the shipped order - and the
        //  constructor then copies x into the low word and y into the high
        //  one, which is the shipped pair.
        CGridVec2 delta(point->m_x - remembered->m_x,
                        point->m_y - remembered->m_y);

        publisher->PublishVelocityDelta(&delta, 1);
    }

    m_target.m_x = point->m_x;
    m_target.m_y = point->m_y;
}
