//============================================================================
//  0x6F3412F0 - SSelectionHost::SetColonMessage: put the right "COLON_MESSAGE"
//  prefix in front of the chat bar's editable line and remember which one it
//  is.
//
//  The requested target is clamped first, and the two clamps are not the same
//  test:
//
//    * a referee cannot whisper its allies, so target 1 becomes 2;
//    * a spectator - anything that is not a full player - is forced to 2
//      whatever it asked for, and a full player keeps its request.
//
//  A game with no network data at all (IsGameDataTagSet_6F53E670 answering 0)
//  is single-player, where the only prefix is the single-player one and the
//  remembered target is reset to 0.
//
//  The prefix key is picked into a variable and fetched once at the end,
//  which is what the shipped code's single shared `push 40h` / `lea edx` /
//  GetGameString tail is - every arm but the private-message one jumps into
//  it.  A `switch` and not an if-chain: MSVC lowers this one as the three
//  `sub`/`jz` pairs the dump shows, followed by the range test on
//  `target - 3`, and an if-chain gives three `cmp`s instead.
//
//  The shipped prologue loads a /GS cookie for the two 0x40-byte buffers;
//  this repo builds at a fixed /GS-, so the cookie load, the `xor esp` and
//  the check call cannot come back.
//
//  Own translation unit: SetEntryActive calls it for real.
//============================================================================
#include "gameui.h"
#include "gamestrings.h"

//  0x6F53E670 - Game/gamedatatag.cpp.
int IsGameDataTagSet_6F53E670();
//  Storm ordinal 578 - bounded formatted print.
int Storm_578(char* dest, unsigned int size, const char* format, ...);

void SSelectionHost::SetColonMessage(int target)
{
    char prefix[0x40];
    char format[0x40];
    int  chosen;

    if (IsLocalPlayerReferee() != 0)
    {
        chosen = target;
        if (chosen == 1)
            chosen = 2;
    }
    else
    {
        chosen = 2;
        if (IsLocalPlayerFullPlayer() == 0)
            chosen = target;
    }

    const char* key;

    if (IsGameDataTagSet_6F53E670() == 0)
    {
        chosen = 0;
        key = "COLON_MESSAGE_SINGLEPLAYER";
    }
    else
    {
        switch (chosen)
        {
        case 0:
            key = "COLON_MESSAGE_ALL";
            break;
        case 1:
            key = "COLON_MESSAGE_ALLIES";
            break;
        case 2:
            key = IsLocalPlayerReferee() != 0 ? "COLON_MESSAGE_REFEREES"
                                              : "COLON_MESSAGE_OBSERVERS";
            break;
        default:
            //  A private message to one of the twelve player slots.  This is
            //  the only arm that formats rather than copying, so it is also
            //  the only one that skips the shared GetGameString below - which
            //  is what the shipped `jmp` past it is.
            //  The range test is spelled against `chosen` and the argument
            //  against `chosen - 3`: writing `chosen - 3` in both places
            //  makes MSVC common-subexpression it into a third callee-saved
            //  register, which costs the function a `push ebx`/`pop ebx` the
            //  shipped stream does not have.
            if (chosen >= 3 && chosen <= 3 + 0xB)
            {
                GetGameString("COLON_MESSAGE_PLAYER", format, sizeof(format));
                Storm_578(prefix, sizeof(prefix), format,
                          GetPlayerDisplayName(chosen - 3, 1));
                goto show;
            }
            key = "UNKNOWN";
            break;
        }
    }

    GetGameString(key, prefix, sizeof(prefix));

show:
    m_colonTarget = chosen;
    m_pPrefixLabel->SetString(prefix);
}
