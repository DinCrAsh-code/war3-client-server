//============================================================================
//  SJassPlayerRates - the two per-player-pair rate tables GetPlayerTaxRate
//  (jassnatives_misc.cpp) reads and SetPlayerTaxRate (jasssetplayertaxrate.cpp)
//  writes.  `this` is the JASS player record (SJassPlayer, cast), indexed by
//  the *other* player's own index; each table is 0x64 (100) dwords, one per
//  possible other player, so nothing here needs a bound beyond the [0,100]
//  clamp SetRateA/SetRateB apply to the stored value itself.
//============================================================================
#include "jassnatives.h"

//----------------------------------------------------------------------------
//  0x6F40F7F0 - `retn 4`.
//----------------------------------------------------------------------------
int SJassPlayerRates::GetRateA(int otherPlayerIndex)
{
    return *(int*)((char*)this + 0x1E4 + otherPlayerIndex * 4);
}

//----------------------------------------------------------------------------
//  0x6F40F820 - `retn 4`.
//----------------------------------------------------------------------------
int SJassPlayerRates::GetRateB(int otherPlayerIndex)
{
    return *(int*)((char*)this + 0x218 + otherPlayerIndex * 4);
}

//----------------------------------------------------------------------------
//  0x6F40F800 - `retn 8`.  The stored value is clamped to at most 100; the
//  shipped code does this with an unsigned `cmp`/`jb`, so a negative `value`
//  (out of JASS's own int range for a percentage, but not out of an `int`'s)
//  compares as huge and is clamped the same as anything else >= 100.
//----------------------------------------------------------------------------
void SJassPlayerRates::SetRateA(int value, int otherPlayerIndex)
{
    if ((unsigned int)value >= 0x64)
        value = 0x64;

    *(int*)((char*)this + 0x1E4 + otherPlayerIndex * 4) = value;
}

//----------------------------------------------------------------------------
//  0x6F40F830 - `retn 8`, same clamp as SetRateA above.
//----------------------------------------------------------------------------
void SJassPlayerRates::SetRateB(int value, int otherPlayerIndex)
{
    if ((unsigned int)value >= 0x64)
        value = 0x64;

    *(int*)((char*)this + 0x218 + otherPlayerIndex * 4) = value;
}
