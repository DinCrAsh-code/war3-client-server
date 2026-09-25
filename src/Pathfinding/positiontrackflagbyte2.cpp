//============================================================================
//  0x6F4749E0 - PositionTrack::SetTopByteFlag80.
//
//  The one-bit front end of StoreTopByteBits (positiontrackflagbyte.cpp):
//  turn bit 7 of the pathing record's flag byte on or off according to a
//  plain int predicate.
//
//  `on ? 0x80 : 0` really is the shipped spelling and not `(on != 0) << 7`:
//  MSVC turns the ternary into `neg eax` / `sbb al, al` / `and eax, 80h`,
//  which is exactly the three instructions the target has - the shift form
//  produces a `test`/`setne`/`shl` sequence instead.
//
//  Moved here from Widget/position_shadowflag.cpp once Widget/position.h's
//  own `Position` was confirmed to be this exact class - see
//  positiontrack.h's own header comment.
//
//  Own translation unit - see positiontrackflagbyte.cpp's own header for
//  why.
//============================================================================
#include "positiontrack.h"

void PositionTrack::SetTopByteFlag80(int on)
{
    StoreTopByteBits((unsigned char)(on ? 0x80 : 0), 0x80);
}
