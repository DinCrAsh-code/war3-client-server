//============================================================================
//  0x6F144B50 - CGameWorld::TestRelationBitClear.
//
//  Two instructions of difference from CGameWorld::TestRelationBit
//  (0x6F3A3400, gameuiplayerenum.cpp) and they are the whole meaning: that
//  one ends `neg`/`sbb`/`neg`, which is `(bits & mask) != 0`, and this one
//  ends `neg`/`sbb`/`add 1`, which is `== 0` (docs/msvc-vc8-idioms.md,
//  "`!x` on a non-bool `int` is `neg`/`sbb`/`add 1`").  So this is "these
//  two players are *not* in that relation" - which is why the pause
//  highlight sweep (gameuihighlightsweep.cpp) uses TestRelationBit for its
//  ally pass and this one for its enemy pass.
//
//  Its own module is 0x6F14xxxx, nowhere near TestRelationBit's, so it gets
//  its own translation unit whichever directory it lands in; it is filed
//  with the rest of the game-wide player state rather than with the UI that
//  calls it.
//
//  Declared as a CGameWorld member for symmetry with TestRelationBit and
//  because that is the shape every caller uses (`mov ecx, world` before the
//  call).  Neither body actually reads `ecx` - both take their two
//  arguments off the stack and `retn 8` - so the receiver is the caller's
//  convention rather than something either function needs.
//============================================================================
#include "gameui.h"

int CGameWorld::TestRelationBitClear(unsigned int player, int bit)
{
    return TestRelationBit(player, bit) == 0;
}
