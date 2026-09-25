//============================================================================
//  0x6F07B870 - CGridVec2::ScaleToLength.
//
//  `v *= length / |v|`, spelled the way the shipped code computes it: one
//  reciprocal square root of the squared length (0x6F6EF800), multiplied by
//  the wanted length once, and that single scale factor applied to both
//  components.  There is no divide anywhere in it and no zero-length guard -
//  the only caller (CPathTrace::AddVelocity, Pathfinding/pathtracevelocity.cpp)
//  has already compared |v| against the cap it hands over, so |v| is known
//  non-zero here.
//
//  A long way from its caller in the image (0x6F07xxxx against 0x6F4Axxxx) -
//  it is a CGridVec2 primitive shipped with the vector module, not with the
//  path trace, which is why it gets its own translation unit here too.
//
//  Two orderings are read straight off the shipped stream and are
//  load-bearing:
//
//  * the squared length is `m_x * m_x + m_y * m_y` in that spelling.  MSVC
//    evaluates the operand it pushes (operator+'s right-hand one) first, so
//    this is what puts the y*y multiply ahead of the x*x one in the stream;
//    writing it the other way round swaps the two and costs five
//    instructions;
//  * the scale is `*length * invLen`, that way round.  operator* takes its
//    left operand in edx and pushes the right, and the shipped call has the
//    caller's pointer in edx.
//============================================================================
#include "game.h"

void CGridVec2::ScaleToLength(const CFloat* length)
{
    CFloat invLen = CFloatInvSqrt(m_x * m_x + m_y * m_y);
    CFloat scale = *length * invLen;

    m_x = m_x * scale;
    m_y = m_y * scale;
}
