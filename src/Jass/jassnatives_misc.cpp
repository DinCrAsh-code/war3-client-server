//============================================================================
//  The remaining player-side natives of this batch: start locations, the
//  cripple flag and the tax-rate pair.  See jassnatives.h.
//============================================================================
#include "jassnatives.h"
#include "gameconfig.h"

//----------------------------------------------------------------------------
//  0x6F3C0EA0 - `DefineStartLocationLoc` "(IHlocation;)V".  The location's
//  coordinate pair is copied into two locals whose addresses are what the
//  game state receives, the same by-pointer convention every real takes in
//  this module.  The singleton is fetched before the resolve because the
//  shipped code keeps it in esi across the call.
//----------------------------------------------------------------------------
void __cdecl JASS_DefineStartLocationLoc(int index, int hLocation)
{
    CJassGameState* state = g_pJassGameState;

    SJassLocation* location = ResolveLocationHandle(hLocation);

    if (location)
    {
        //  y is read before x: the shipped code loads +0x28 into ecx and
        //  +0x24 into edx, in that order, and the two locals land in the
        //  same slots either way.
        int y = location->m_y;
        int x = location->m_x;

        state->DefineStartLocation(index, &x, &y);
    }
}

//----------------------------------------------------------------------------
//  0x6F3C0FA0 - `ForcePlayerStartLocation` "(Hplayer;I)V".  Unlike every
//  other native here it tests the singleton for null first: the whole body is
//  skipped when the game state is not up yet.
//----------------------------------------------------------------------------
void __cdecl JASS_ForcePlayerStartLocation(int hPlayer, int startLocation)
{
    CJassGameState* state = g_pJassGameState;

    if (state)
    {
        SJassPlayer* player = ResolvePlayerHandle(hPlayer);

        if (player)
            state->ForcePlayerStartLocation(player->m_playerIndex, startLocation);
    }
}

//----------------------------------------------------------------------------
//  0x6F3C1310 - `CripplePlayer` "(Hplayer;Hforce;B)V".  The force is resolved
//  to an object whose own handle ref at +0x24 is queried
//  (HandleRefFieldOwner_6F473170::QueryField78, queryfield.cpp) and that
//  result, not the force, is what the player is given.
//----------------------------------------------------------------------------
void __cdecl JASS_CripplePlayer(int hPlayer, int hForce, int crippled)
{
    SJassPlayer* player = ResolvePlayerHandle(hPlayer);

    if (player)
    {
        SJassForce* force = ResolveForceHandle(hForce);

        if (force)
        {
            int flag = ((HandleRefFieldOwner_6F473170*)((char*)force + 0x24))->QueryField78();
            ((SJassPlayerRates*)player)->SetCrippled(crippled, flag);
        }
    }
}

//----------------------------------------------------------------------------
//  0x6F3C1150 - `GetPlayerTaxRate` "(Hplayer;Hplayer;Hplayerstate;)I".  The
//  playerstate handle selects between two rate tables and anything other than
//  the two it knows returns 0; the shipped code spells that selection as two
//  chained `sub ecx,1`, which is what a `switch` on consecutive small
//  constants compiles to.
//----------------------------------------------------------------------------
int __cdecl JASS_GetPlayerTaxRate(int hPlayer, int hOtherPlayer, int state)
{
    SJassPlayer* player = ResolvePlayerHandle(hPlayer);

    if (!player)
        return 0;

    SJassPlayer* other = ResolvePlayerHandle(hOtherPlayer);

    if (!other)
        return 0;

    switch (state)
    {
    case 1:
        return ((SJassPlayerRates*)player)->GetRateA(other->m_playerIndex);
    case 2:
        return ((SJassPlayerRates*)player)->GetRateB(other->m_playerIndex);
    }

    return 0;
}

//----------------------------------------------------------------------------
//  0x6F3C10E0 - `SetPlayerTaxRate` "(Hplayer;Hplayer;Hplayerstate;I)V".  The
//  same two-table switch GetPlayerTaxRate makes, gated by a config flag this
//  native alone checks: bit 0x100 of the world's own game-configuration
//  flags (Game/gameconfig.h's `m_flags`, the same field SetGameSpeed's own
//  bit 0x4000 lives in) makes the whole native a no-op when set.
//----------------------------------------------------------------------------
void __cdecl JASS_SetPlayerTaxRate(int hPlayer, int hOtherPlayer, int state,
                                   int rate)
{
    if (g_pGameConfig->m_flags & 0x100)
        return;

    SJassPlayer* player = ResolvePlayerHandle(hPlayer);

    if (!player)
        return;

    SJassPlayer* other = ResolvePlayerHandle(hOtherPlayer);

    if (!other)
        return;

    switch (state)
    {
    case 1:
        ((SJassPlayerRates*)player)->SetRateA(rate, other->m_playerIndex);
        return;
    case 2:
        ((SJassPlayerRates*)player)->SetRateB(rate, other->m_playerIndex);
        return;
    }
}
