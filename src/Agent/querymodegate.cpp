//============================================================================
//  0x6F468FC0 - IsQueryModeGateOpen.  A naked redirect in
//  agenteventbinding_thunks.cpp until now.
//
//  One field off the game-data singleton, with a null singleton answering
//  no.  Six instructions and no `xor eax, eax` in them: the pointer that
//  failed the test *is* the zero the function returns, which is why this is
//  written as an early `return 0` on the null rather than as a
//  `singleton != 0 ? singleton->m_queryModeGate : 0` (that spelling
//  materialises the constant a second time).
//
//  Own translation unit: BuildAndRegisterEventBinding calls it for real.
//============================================================================
#include "game.h"

int IsQueryModeGateOpen()
{
    CGameData* game = g_pGameData;
    if (game == 0)
        return 0;
    return game->m_queryModeGate;
}
