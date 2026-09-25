//============================================================================
//  The 0x6F47xxxx box helpers: a cell rectangle in CFloat and in int, their
//  component multiply, the float->cell conversion, and rectangle intersect.
//
//  The field order is recovered, not guessed.  sub_6F4A6D70 builds one of
//  these from a position and a radius as
//
//      { pos[1] - r, pos[0] - r, pos[1] + r, pos[0] + r }
//
//  i.e. {min,min,max,max} with the *second* vector component first, and
//  sub_6F49FCD0 turns the first pair into `stride * i` and the second into
//  `+ j` - so the leading axis is the row (y) and the trailing one the column
//  (x), and the rectangle is half-open (sub_6F47BA20 adds one to the two max
//  fields and to nothing else).
//============================================================================
#ifndef BOXMATH_H
#define BOXMATH_H

#include "CFloat.h"

//  A half-open cell rectangle [y0,y1) x [x0,x1), in world units.
struct SCellBoxF
{
    SCellBoxF() {}
    SCellBoxF(const CFloat& y0, const CFloat& x0, const CFloat& y1,
              const CFloat& x1)
        : m_y0(y0), m_x0(x0), m_y1(y1), m_x1(x1) {}

    CFloat m_y0, m_x0, m_y1, m_x1;
};

//  The same rectangle in cell indices.
//
//  The empty (but user-provided, so non-trivial) default constructor is
//  load-bearing: an array of these makes VC8 emit its own
//  `vector constructor iterator' COMDAT (0x6F002010 in the dump) with a
//  pointer to the out-of-line constructor (0x6F49E630), which is exactly what
//  CGridRegistration::UpdateBox's four-rectangle scratch array does.  A single
//  local gets nothing, which is equally what the dump shows.  Remove the
//  constructor and both facts stop being true.
struct SCellBox
{
    SCellBox() : m_y0(0), m_x0(0), m_y1(0), m_x1(0) {}
    int m_y0, m_x0, m_y1, m_x1;
};

//----------------------------------------------------------------------------
//  0x6F6EF170 - floor(): the largest CFloat integer value not greater than
//  `value`.  Not "round to nearest" - see the sign handling in boxmath.cpp.
//----------------------------------------------------------------------------
CFloat __fastcall CFloatFloor(const CFloat& value);

//  The two constants floor() returns for |value| < 1: -1.0 for a negative
//  non-zero input, +0.0 for everything else.
extern const CFloat g_CFloatMinusOne;   // dword_6FAAE464

//----------------------------------------------------------------------------
//  0x6F47D5D0 - component-wise multiply, `dst = a * b`.
//----------------------------------------------------------------------------
SCellBoxF __fastcall MulCellBoxF(const SCellBoxF& a, const SCellBoxF& b);

//----------------------------------------------------------------------------
//  0x6F47D560 - the intersection of two cell rectangles: the max of the two
//  mins and the min of the two maxes.  An empty result comes back as a
//  rectangle whose min is >= its max, which is exactly the test
//  sub_6F49FF90 makes on it.
//----------------------------------------------------------------------------
SCellBox __fastcall IntersectCellBox(const SCellBox& a, const SCellBox& b);

#endif
