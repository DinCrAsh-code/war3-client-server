//============================================================================
//  0x6F267580 - build a plane through three points: normal =
//  normalize((c-a) x (b-a)) - note the operand order, cross of the
//  "c minus a" leg with the "b minus a" leg, not the more usual b-a/c-a -
//  then m_d = -(normal . a).  Reached from CUnit::Method_0x1B8 (slot 110,
//  0x6F282160, still a thunk - 570 instructions of its own, not attempted
//  this session).
//
//  FVec3::Normalize() (fvec3normalize.cpp) is called directly on `this`
//  reinterpreted as an FVec3*, the same way the shipped body does - the
//  first three fields of FPlane and FVec3 line up exactly, and the shipped
//  call site does nothing to make a real FVec3 copy first.
//============================================================================
#include "fplane.h"
#include "fvec3.h"

void FPlane::FromThreePoints(const FVec3* a, const FVec3* b, const FVec3* c)
{
    float v1x = c->m_x - a->m_x;
    float v1y = c->m_y - a->m_y;
    float v1z = c->m_z - a->m_z;
    float v2x = b->m_x - a->m_x;
    float v2y = b->m_y - a->m_y;
    float v2z = b->m_z - a->m_z;

    float cx = v2y * v1z - v2z * v1y;
    float cy = v2z * v1x - v2x * v1z;
    float cz = v2x * v1y - v2y * v1x;
    m_x = cx;
    m_y = cy;
    m_z = cz;

    ((FVec3*)this)->Normalize();

    m_d = -(m_y * a->m_y + a->m_x * m_x + m_z * a->m_z);
}
