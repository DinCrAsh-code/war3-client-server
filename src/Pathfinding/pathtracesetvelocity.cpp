//============================================================================
//  0x6F493A40 - CPathTrace::SetVelocity: the absolute form of
//  PublishVelocityDelta (0x6F4A73B0).
//
//  Everything downstream of this point works in deltas - AddVelocity *adds*
//  to +0x80 - so setting a velocity means publishing the difference from the
//  one the trace already carries.  That is the whole body: one CGridVec2 of
//  `wanted - current`, handed straight on with the caller's own notify flag.
//
//  Instruction for instruction the same shape as
//  Widget/positionsinksettarget.cpp, which measures its target against the
//  point it last published in exactly this way, and the same two orderings
//  are load-bearing here:
//
//  * the y difference is computed first and the x difference second, because
//    MSVC evaluates constructor arguments right to left, while the pair is
//    packed {x, y}.  The two subtractions are independent, so nothing but
//    the shipped stream says which comes first;
//  * each difference is `wanted - current`, that way round.  CFloat's
//    operator- negates its right-hand operand, so reversing them would set
//    the velocity to `2 * current - wanted`.
//
//  Reached from CAgentTickRelay::Reload (Agent/agenttickrelay_tick.cpp) via
//  the agent it resolves, with a zeroed pair and notify=1 - i.e. "stop, and
//  tell the observer" - which is what a relay reload does to a unit that has
//  arrived.  Own translation unit: the publish is a real `call`.
//============================================================================
#include "game.h"

void CPathTrace::SetVelocity(const CGridVec2* velocity, int notify)
{
    //  The current velocity is bound to its own pointer: the shipped stream
    //  forms `&m_direction` once into a callee-saved register and reaches
    //  both components off it, and going through the member twice makes MSVC
    //  re-form the address instead.
    CGridVec2* current = &m_direction;

    CGridVec2 delta(velocity->m_x - current->m_x,
                    velocity->m_y - current->m_y);

    PublishVelocityDelta(&delta, notify);
}
