//============================================================================
//  0x6F4A73F0 - CPathTrace::SetMaxSpeed.
//
//  Replace m_maxSpeed outright - the caller (PositionTrack::SetMaxSpeed,
//  positiontrackspeed.cpp, the only caller in this repo) has already
//  clamped and world-to-grid converted the value - then re-run the current
//  velocity through exactly the length calculation AddVelocity's own tail
//  makes (pathtraceaddvelocity.cpp: `len = sqrt(vx*vx + vy*vy)`), masking
//  the sign bit off the result before the comparison.  If that length now
//  exceeds the *new* cap, force a re-clamp by republishing a zero velocity
//  delta - PublishVelocityDelta settles the position first and then calls
//  AddVelocity(zero, 0), whose own "over the cap" branch does the actual
//  ScaleToLength.
//
//  Own translation unit: it is a real `call` from PositionTrack's own slot
//  6 (positiontrackspeed.cpp), which shipped nowhere near this 0x6F4A7xxx
//  module, and it makes a real call of its own into PublishVelocityDelta
//  (0x6F4A73B0, pathtracepublishvelocity.cpp) - same module, but still not
//  safe to fold together: PublishVelocityDelta itself calls RecomputeOrigin
//  and AddVelocity for real and must not be inlined into either of them.
//
//  The magnitude mask (`& 0x7FFFFFFF`) is a bare bit-and on the sqrt
//  result, not NearlyEqual's inlined "subtract zero, then mask" shape
//  (quadrant.cpp) - there is no subtraction in this stream, only the mask,
//  so nothing here claims this is the same source idiom as that one.
//
//  Squaring through `dir = &m_direction; dir->m_x*dir->m_x +
//  dir->m_y*dir->m_y` (not `m_direction.m_x*...`/`m_direction.m_y*...`
//  directly) matters: two independent member accesses off `this` each emit
//  their own `lea`, but a single pointer to the pair lets the second
//  component's address fold to `[base+4]` off the first - the shape the
//  shipped code has (`lea edi,[esi+128]` once, `lea edx,[edi+4]` off it).
//
//  DIFFERS, not EXACT: `int magnitude`'s own stack slot lands at a
//  different displacement (`[esp+12]` here, `[esp+28]` shipped) - the
//  shipped code reuses `speed`'s own incoming argument slot (dead once its
//  value is copied into `m_maxSpeed` above) rather than allocating a fresh
//  one, the same "argument's own home slot used as scratch" idiom
//  PositionTrack::SetMaxSpeed's own writeup (positiontrackspeed.cpp) hits
//  for the identical reason. A compiler-temporary stack-slot choice, not
//  a semantic gap - every instruction and operand otherwise matches;
//  pending angr IDENTICAL verification.
//============================================================================
#include "pathmove.h"
#include "spatialgrid.h"

void CPathTrace::SetMaxSpeed(const CFloat* speed)
{
    m_maxSpeed = *speed;

    CGridVec2* dir = &m_direction;
    CFloat lenSq = dir->m_x * dir->m_x + dir->m_y * dir->m_y;
    CFloat len = CFloatSqrt(lenSq);

    int magnitude = len.m_bits & 0x7FFFFFFF;
    if (*(const float*)&magnitude > *(const float*)&m_maxSpeed.m_bits)
    {
        CGridVec2 zero(g_CFloatZero, g_CFloatZero);
        PublishVelocityDelta(&zero, 0);
    }
}
