//============================================================================
//  0x6F3DB270 - SGameConfig::SetGameSpeed, the one member behind the
//  `SetGameSpeed` JASS native (src/Jass/jassnatives_gameconfig.cpp).
//  `retn 4`, `this` in ecx.
//
//  Two things happen: the speed is clamped into 0..2 and latched at +0x47 -
//  which is the byte `GetGameSpeed` reads straight back - and the clamped
//  value is handed to ApplyGameSpeedScale, which is what actually changes
//  the simulation's tick rate.
//
//  The clamp is two chained selects and not one range test, which is what
//  the shipped code's two comparisons say: `cmp al,2` / `jb` takes anything
//  at or above 2 to 2, and only the *other* arm then runs `test al,al` /
//  `ja` / `xor al,al`.  Written as one `if`/`else if` chain instead, MSVC
//  keeps a single tail; written as the pair of selects below it proves the
//  high arm's result is already above zero, drops the second test there and
//  duplicates the four-instruction tail into both arms - which is exactly
//  the shipped body, `retn 4` and all, twice.
//
//  Own translation unit: ApplyGameSpeedScale is a real call into another
//  module (0x6F53Exxx) and eleven instructions is well inside what /Ob2
//  would inline given the chance.
//============================================================================
#include "gameconfig.h"

void SGameConfig::SetGameSpeed(int speed)
{
    unsigned char value = (unsigned char)speed;

    value = (value < 2) ? value : 2;
    value = (0 < value) ? value : 0;

    m_speed = value;

    ApplyGameSpeedScale(value);
}
