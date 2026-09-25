//============================================================================
//  0x6F3BBC00 - JASS's `RestartGame (B)V`.
//
//  ReloadGame with two arguments changed: the caller's "show the score
//  screen" boolean goes in first, and the second argument - "re-resolve the
//  path through the save game it names" - is 1 rather than 0.  That second
//  one is the whole difference between the two natives: a restart that finds
//  a `.w3z` in the map-path slot opens it and puts the *map* it was saved
//  from back there, so the restart restarts the map rather than the save.
//
//  See docs/targets/JASS_ReloadGame.md.
//============================================================================
#include "gamerestart.h"

void __cdecl JASS_RestartGame(int doScoreScreen)
{
    if (IsGameDataTagSet_6F53E670())
        return;

    void* world = g_unk6FAB65F4;
    if (world == 0)
        return;

    ((SWorldRestart*)world)->RequestRestart(doScoreScreen, 1, 0, 0);
}
