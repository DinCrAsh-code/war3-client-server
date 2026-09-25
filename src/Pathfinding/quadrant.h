//============================================================================
//  Snapping a facing direction to the nearest of the footprint's four cardinal
//  quadrants.  0x6F00CDE0/0x6F00C3F0, close to CMiscCustom's module but a
//  distinct pair of functions - kept in their own translation unit.
//============================================================================
#ifndef QUADRANT_H
#define QUADRANT_H

#include "CFloat.h"

//  Four direction markers this call tree tests a value against: due east,
//  the +90 degree quadrant boundary, the +180 boundary and the -90/+270
//  boundary, in the order the shipped code checks them.  Encoded values, not
//  named constants recovered from anywhere but this call tree.
extern const CFloat g_quadrantEast;      // dword_6FAAE5FC
extern const CFloat g_quadrantNorth;     // dword_6FAAE600
extern const CFloat g_quadrantWest;      // dword_6FAAE604 (compared via NearlyEqual)
extern const float  g_quadrantEpsilon;   // flt_6FAAE47C

//----------------------------------------------------------------------------
//  0x6F00C3F0 - "close enough to call the same direction".  A free function
//  in its own right (not just NearlyEqual() inlined): it is reached from
//  ClassifyQuadrant() as a genuine call as well as being what that function's
//  own first three tests look like once the compiler inlines them - see
//  quadrant.cpp.
//----------------------------------------------------------------------------
int __fastcall NearlyEqual(const CFloat& a, const CFloat& b);

//----------------------------------------------------------------------------
//  0x6F00CDE0 - which of the four quadrants `value` is nearest.  The last
//  test's result is discarded: the shipped code still makes the call (an
//  always-on sanity check, not an assert an NDEBUG build would have already
//  removed), and returns 3 regardless of what it answers.
//----------------------------------------------------------------------------
//  Takes its CFloat by value on the stack and cleans it up itself
//  (`retn 4`), not by reference in ecx - see quadrant.cpp.
int __stdcall ClassifyQuadrant(CFloat value);

#endif
