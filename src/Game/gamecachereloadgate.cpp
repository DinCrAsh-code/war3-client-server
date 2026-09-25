//============================================================================
//  0x6F3B28C0 - may the object-data caches be reloaded right now?
//
//  Only when neither of two conditions holds: the current game data carrying
//  a tag (0x6F53E670, gamedatatag.cpp) and whatever 0x6F53F160 answers.  Two
//  separate tests sharing one `xor eax,eax` / `retn` block, which is what two
//  ifs compile to.
//============================================================================
#include "gamerestart.h"

//  0x6F53F160 - Game/gamemode.cpp already reconstructs this one.
int __fastcall IsGameModeOne();

//  Nested rather than two early returns: written as
//  `if (A()) return 0; if (B()) return 0; return 1;` MSVC keeps the first
//  branch but if-converts the second into `neg`/`sbb`/`add 1`, where the
//  shipped stream has two `jnz` into one shared `xor eax,eax` / `retn`.
int ReloadGateOpen()
{
    if (!IsGameDataTagSet_6F53E670())
    {
        if (!IsGameModeOne())
            return 1;
    }

    return 0;
}
