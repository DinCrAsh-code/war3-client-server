//============================================================================
//  0x6F53EED0 - may this slot still take part?
//
//  Two questions, and the answer is yes unless both say no: the game data
//  has to be carrying a real tag (0x6F53E670, gamedatatag.cpp) *and* the
//  slot has to have used up its pause allowance - three or more.  With no
//  game data the allowance is irrelevant and everyone participates, which is
//  why the tag test comes first and short-circuits.
//
//  `__fastcall` with the slot in cl and the record index in edx, and both
//  are forwarded to GetSlotPauseCount in the same two registers.
//
//  The allowance test is *unsigned* (`jb`, not `jl`), which is what the
//  cast is for: the counter cannot be negative and the shipped code knows
//  it.
//
//  Own translation unit: two real calls out of it.
//============================================================================
#include "netdata.h"

int IsGameDataTagSet_6F53E670();        //  0x6F53E670 - gamedatatag.cpp.

int __fastcall CNetData::IsSlotParticipating(unsigned char slot,
                                                     int index)
{
    if (IsGameDataTagSet_6F53E670() != 0
        && (unsigned int)GetSlotPauseCount(slot, index) >= 3)
        return 0;

    return 1;
}
