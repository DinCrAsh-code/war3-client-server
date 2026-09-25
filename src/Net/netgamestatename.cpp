//============================================================================
//  0x6F537930 - the printable name of a game state.
//
//  A seven-case jump-table switch whose case 3 shares the default arm, which
//  is why "GAME_STATE_UNKNOWN?" answers both an out-of-range value and the
//  one in-range value with no name of its own.  The strings are the shipped
//  binary's, read out of 0x6F9582E0..0x6F958358 with `ida_query bytes`; the
//  question mark is part of the literal.
//
//  `__fastcall`: the state arrives in ecx and nothing is pushed, so this is
//  `retn 0` and its two call sites in CNetSession::SetGameState leave the
//  value in ecx for it.
//
//  Own translation unit: SetGameState calls it twice for real.
//============================================================================
#include "netdata.h"

const char* __fastcall GameStateName(int state)
{
    switch (state)
    {
    case kGameStateNone:    return "GAME_STATE_NONE";
    case kGameStateJoined:  return "GAME_STATE_JOINED";
    case kGameStateFaked:   return "GAME_STATE_FAKED";
    case kGameStatePlaying: return "GAME_STATE_PLAYING";
    case kGameStatePausing: return "GAME_STATE_PAUSING";
    case kGameStatePaused:  return "GAME_STATE_PAUSED";
    }

    return "GAME_STATE_UNKNOWN?";
}
