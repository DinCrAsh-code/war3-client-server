//============================================================================
//  0x6F474A20 - PositionTrack::SetTopByteBits3070: the other one-line
//  front end onto StoreTopByteBits (positiontrackflagbyte.cpp), setting
//  bits 4 and 5 of the pathing record's flag byte and clearing bit 6
//  (value 0x30 under mask 0x70).
//
//  Moved here from Widget/position_flagbits3070.cpp once Widget/position.h's
//  own `Position` was confirmed to be this exact class - see
//  positiontrack.h's own header comment.
//
//  Own translation unit for the same reason positiontrackflagbyte2.cpp is
//  one: the shipped code reaches StoreTopByteBits with a real `call`.
//============================================================================
#include "positiontrack.h"

void PositionTrack::SetTopByteBits3070()
{
    StoreTopByteBits(0x30, 0x70);
}
