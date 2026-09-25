//============================================================================
//  0x6F6EF330 - "add half, then floor" - i.e. round-half-up, the standard
//  encode of `(int)(x + 0.5f)` in this build's own software float.
//
//  No name or dump of its own existed anywhere else in this repo before
//  CPlayerWar3UpkeepRecompute's own dump surfaced one (Storm/
//  textsinkscratch.cpp's own header comment previously named it only as
//  "sub_6F6EF330, no dump of its own", among the leaves FormatCFloat's own
//  naked redirect declines to chase).
//
//  Own translation unit, matching this function's real address neighbourhood
//  (0x6F6Exxxx, alongside operator+/CFloatFloor) rather than either of its
//  two known callers' modules - the same "one TU per original module" rule
//  CLAUDE.md's own house rules ask for, and the only thing that stops /Ob2
//  from inlining a two-call-site helper the shipped stream keeps as a real,
//  separately addressable call (see the entry in docs/msvc-vc8-idioms.md
//  this session added).
//
//  `edx` carries `value` (there is no separate "b" - CFloatFloor takes one
//  argument), matching CFloatFloor's own `ecx`-hidden-pointer/`edx`-value
//  convention exactly.
//============================================================================
#include "CFloat.h"
#include "boxmath.h"

//  0x6FAAE4D0 - the same "add half a unit" constant Pathfinding/
//  pathcellcentre.cpp's own g_CFloatHalf already names; redeclared
//  identically for the same reason every other CFloat constant in this
//  codebase is (own translation unit, own redeclaration).
extern const CFloat g_CFloatHalf;

CFloat __fastcall RoundCFloatHalfUp(const CFloat& value)
{
    return CFloatFloor(value + g_CFloatHalf);
}
