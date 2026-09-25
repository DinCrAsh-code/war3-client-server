//============================================================================
//  0x6F4A70D0 - CPathTrace::AddVelocity.
//
//  The one place a trace's velocity actually changes.  It adds the delta it
//  is handed to the pair at +0x80, and then does three things to the result
//  that nothing else in this module does:
//
//   * a squared length under the module's equality tolerance is flushed to
//     exactly zero, both components, rather than left as a denormal
//     stagger.  The tolerance test is the *inlined* body of NearlyEqual
//     (Pathfinding/quadrant.cpp): the difference against g_CFloatZero is
//     taken through CFloat::operator- even though subtracting zero cannot
//     change the bits, its magnitude is masked with 0x7FFFFFFF, and only
//     that is compared - as a hardware float - against flt_6FAAE614.  The
//     dead subtraction is in the shipped stream and is what makes this an
//     inline of that function rather than a bare `& 0x7FFFFFFF`;
//   * a length over m_maxSpeed is clamped back to exactly m_maxSpeed by
//     CGridVec2::ScaleToLength (Math/gridvec2scaletolength.cpp).  Only over:
//     a shorter velocity is left alone, so this is a cap and not a set;
//   * the 0x20000000 bit of the collision registration's move flags is
//     published from `m_maxSpeed > 0`, not from the velocity - a trace with
//     a zero cap reads as not moving whatever is in +0x80.  Both arms end at
//     the same "clear the bit" instructions in the shipped code, which is
//     the tail merge MSVC does for itself; the source has the two arms
//     written out.
//
//  Own translation unit: it is a real `call` from
//  CPathTrace::PublishVelocityDelta (0x6F4A73B0), which shipped in the same
//  module, and it makes real calls of its own into ScaleToLength.
//
//  Both remaining comparisons are `>` with the left operand `fld`ed first,
//  the order NormalizeVec2 (pathnormalize.cpp) already establishes for this
//  compiler; reversing either gives `test ah, 41h` instead of `test ah, 5`.
//============================================================================
#include "pathmove.h"
#include "spatialgrid.h"

//  flt_6FAAE614 - the module's "close enough to zero" threshold, compared as
//  a real float and never through a CFloat operation.  Same declaration
//  Math/cfloatnearlyequal.cpp and Widget/positionmodifier.h give it.
extern const float g_CFloatEqualityTolerance;

void CPathTrace::AddVelocity(const CGridVec2* delta, int notify)
{
    m_direction.m_x = m_direction.m_x + delta->m_x;
    m_direction.m_y = m_direction.m_y + delta->m_y;

    CFloat lenSq = m_direction.m_x * m_direction.m_x +
                   m_direction.m_y * m_direction.m_y;

    //  NearlyEqual(lenSq, g_CFloatZero), inlined - see the header comment
    //  for why the subtraction of zero is not dead code to be removed.
    int magnitude = (lenSq - g_CFloatZero).m_bits & 0x7FFFFFFF;
    if (*(const float*)&magnitude < g_CFloatEqualityTolerance)
    {
        //  y before x: the shipped code loads the zero once and stores it
        //  through the object base for the high component and through the
        //  vector base for the low one, in that order.
        //  y before x, and the global assigned twice rather than read into
        //  a local first: the shipped code loads it once and stores it from
        //  two registers, and every spelling that names a local (one temp,
        //  two temps, a CGridVec2 constructed in place) folds the pair into
        //  a single register and emits one instruction fewer.
        m_direction.m_y = g_CFloatZero;
        m_direction.m_x = g_CFloatZero;
        m_collisionGrid->m_moveFlags &= ~0x20000000u;
    }
    else
    {
        CFloat len = CFloatSqrt(lenSq);
        if (*(const float*)&len.m_bits > *(const float*)&m_maxSpeed.m_bits)
            m_direction.ScaleToLength(&m_maxSpeed);

        if (*(const float*)&m_maxSpeed.m_bits > *(const float*)&g_CFloatZero.m_bits)
            m_collisionGrid->m_moveFlags |= 0x20000000u;
        else
            m_collisionGrid->m_moveFlags &= ~0x20000000u;
    }

    if (notify != 0)
    {
        //  vtable slot 0x50, taking nothing but `this` - the sibling of the
        //  slot 0x54 observer CPathTrace::ApplyDelta calls (see
        //  Sync/timesyncphysics.cpp for why this class's virtuals are
        //  reached as raw slot reads rather than declared).
        typedef void (__thiscall *VelocityChangedFn)(void*);
        (*(VelocityChangedFn**)this)[0x50 / 4](this);
    }
}
