//============================================================================
//  0x6F53E0B0 - CNetPlayerRecord::SetGameState, which the shipped
//  diagnostic below calls CNetData::CNetSession::SetGameState.  netdata.h
//  says why this repo keeps the older analysis name for the class.
//
//  The function that names the whole layer: its refusal path formats
//  "CNetData::CNetSession::SetGameState (from %s to %s)" with the old and
//  the new state's own printable names.
//
//  The transition table, read straight off the jump table at 0x6F53E134:
//
//      requested == current        always accepted (re-stores it)
//      -> NONE     (0)             always accepted
//      -> JOINED   (1)             from NONE
//      -> FAKED    (2)             from NONE
//      -> 3                        from JOINED
//      -> PLAYING  (4)             from 3, PAUSING or PAUSED
//      -> PAUSING  (5)             from PLAYING or PAUSED
//      -> PAUSED   (6)             from PAUSING
//      anything else               refused
//
//  Four of the seven arms compute a plain equality and share one
//  `setz al` / `test eax,eax` / `jnz accept` tail (docs/msvc-vc8-idioms.md,
//  "One shared test of a switch's result"), which is why they are written
//  as an assignment to `accepted` and the two multi-value arms - which the
//  compiler lowers to their own compare-and-branch chains and which share
//  their `current == PAUSED` tail with each other - are written as `goto`s.
//  Spelling all seven the same way loses the shared tail either way round.
//
//  Own translation unit: two real calls to GameStateName and one to
//  SysMessagePrintf.
//============================================================================
#include "netdata.h"

int CNetPlayerRecord::SetGameState(int state)
{
    int current = m_gameState;

    //  The "already there" test goes through the same `accepted` flag as
    //  every other arm rather than being an early branch of its own: the
    //  shipped code materialises it with `sete al` / `test eax,eax`, which
    //  is what a flag costs and a straight `if (state == current)` does not
    //  (docs/msvc-vc8-idioms.md, "A validation flag costs a `sete` /
    //  `test al,al` a straight early return does not").
    int accepted = (state == current);
    if (accepted)
        goto accept;

    switch (state)
    {
    case kGameStateNone:
        goto accept;
    case kGameStateJoined:
    case kGameStateFaked:
        accepted = (current == kGameStateNone);
        break;
    case 3:
        accepted = (current == kGameStateJoined);
        break;
    case kGameStatePlaying:
        if (current == 3 || current == kGameStatePausing
            || current == kGameStatePaused)
            goto accept;
        goto refuse;
    case kGameStatePausing:
        if (current == kGameStatePlaying || current == kGameStatePaused)
            goto accept;
        goto refuse;
    case kGameStatePaused:
        accepted = (current == kGameStatePausing);
        break;
    default:
        goto refuse;
    }

    if (accepted)
        goto accept;
    goto refuse;

accept:
    m_gameState = state;
    return 1;

refuse:
    SysMessagePrintf("CNetData::CNetSession::SetGameState (from %s to %s)",
                     GameStateName(current), GameStateName(state));
    return 0;
}
