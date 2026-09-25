//============================================================================
//  0x6F3BBBE0 - JASS's `ReloadGame ()V`.
//
//  Twelve instructions, and eleven of them are the shared restart request:
//  refuse outright if the current game data carries a tag (a POOL or ENON
//  session), otherwise hand the world object four zeros.
//
//  The four `push eax` are not a spelling choice - eax is already zero from
//  the tag test, and the shipped code reuses it for every argument rather
//  than materialising four immediates.  Written as four literal zeros here,
//  which is what produces them.
//
//  See docs/targets/JASS_ReloadGame.md.
//============================================================================
#include "gamerestart.h"

void __cdecl JASS_ReloadGame()
{
    if (IsGameDataTagSet_6F53E670())
        return;

    void* world = g_unk6FAB65F4;
    if (world == 0)
        return;

    ((SWorldRestart*)world)->RequestRestart(0, 0, 0, 0);
}
