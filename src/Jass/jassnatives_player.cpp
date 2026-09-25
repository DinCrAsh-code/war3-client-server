//============================================================================
//  The `player` JASS natives at 0x6F3C0Exx-0x6F3C12xx: another contiguous run
//  in the registration table, all of them resolving their handle through
//  ResolvePlayerHandle.  See jassnatives.h.
//============================================================================
#include "jassnatives.h"

//----------------------------------------------------------------------------
//  0x6F3C0EE0 - `SetPlayerTeam` "(Hplayer;I)V".  The player's own index, not
//  the resolved object, is what the game state is handed.
//----------------------------------------------------------------------------
void __cdecl JASS_SetPlayerTeam(int hPlayer, int team)
{
    SJassPlayer* player = ResolvePlayerHandle(hPlayer);

    if (player)
        g_pJassGameState->SetPlayerTeam(team, player->m_playerIndex);
}

//----------------------------------------------------------------------------
//  0x6F3C0F10 - `SetPlayerName` "(Hplayer;S)V": resolve the JASS string to a
//  C string, then assign it into the counted string embedded at +0x24.
//----------------------------------------------------------------------------
void __cdecl JASS_SetPlayerName(int hPlayer, int hName)
{
    SJassPlayer* player = ResolvePlayerHandle(hPlayer);

    if (player)
        ((STStringField*)((char*)player + 0x24))->Assign(JassStringToCString(hName));
}

//----------------------------------------------------------------------------
//  0x6F3C0F40 - `GetPlayerTeam` "(Hplayer;)I".
//----------------------------------------------------------------------------
int __cdecl JASS_GetPlayerTeam(int hPlayer)
{
    SJassPlayer* player = ResolvePlayerHandle(hPlayer);

    if (!player)
        return 0;

    return g_pJassGameState->GetPlayerTeam(player->m_playerIndex);
}

//----------------------------------------------------------------------------
//  0x6F3C0F80 - `SetPlayerStartLocation` "(Hplayer;I)V".  The field written
//  here at +0x274 is exactly the one GetPlayerStartLocation below reads
//  back, and the write is *direct*: this native does not go through
//  CJassGameState::ForcePlayerStartLocation the way
//  `ForcePlayerStartLocation` (0x6F3C0FA0, jassnatives_misc.cpp) does, so
//  none of that member's other effects - the two flag words on the start
//  location record, the publication through SetField78 - happen.  The two
//  natives are not the same operation and must not be collapsed into one.
//----------------------------------------------------------------------------
void __cdecl JASS_SetPlayerStartLocation(int hPlayer, int startLocation)
{
    SJassPlayer* player = ResolvePlayerHandle(hPlayer);

    if (player)
        player->m_startLocation = startLocation;
}

//----------------------------------------------------------------------------
//  0x6F3C0FD0 - `GetPlayerStartLocation` "(Hplayer;)I".
//----------------------------------------------------------------------------
int __cdecl JASS_GetPlayerStartLocation(int hPlayer)
{
    SJassPlayer* player = ResolvePlayerHandle(hPlayer);

    if (!player)
        return 0;

    return player->m_startLocation;
}

//----------------------------------------------------------------------------
//  0x6F3C0FF0 / 0x6F3C1020 - `GetPlayerStartLocationX` / `...Y`, both
//  "(Hplayer;)R".  Indexing one 0x2C-byte record out of the game state's own
//  table; the Y form folds its +4 field offset into the index (4 * 0x2C is
//  exactly 0xB0), which is the compiler's arithmetic, not a different table.
//----------------------------------------------------------------------------
int __cdecl JASS_GetPlayerStartLocationX(int hPlayer)
{
    SJassPlayer* player = ResolvePlayerHandle(hPlayer);

    if (!player)
        return 0;

    //  The record's address is formed first and dereferenced second (`lea`
    //  then `mov`), where the Y native below folds the whole thing into one
    //  addressing mode - so the X native reads the record itself rather than
    //  a named field of it.  Spelled that way here.
    const int* x = (const int*)&g_pJassGameState->m_startLocations[player->m_startLocation];
    return *x;
}

int __cdecl JASS_GetPlayerStartLocationY(int hPlayer)
{
    SJassPlayer* player = ResolvePlayerHandle(hPlayer);

    if (!player)
        return 0;

    return g_pJassGameState->m_startLocations[player->m_startLocation].m_y;
}

//----------------------------------------------------------------------------
//  0x6F3C11A0 - `SetPlayerRacePreference` "(Hplayer;I)V".  The whole flag
//  word is replaced *except* bit 6, which SetPlayerRaceSelectable owns: the
//  shipped code's `f ^ v` / `& 0x40` / `^ v` is MSVC's "take these bits from
//  a, the rest from b" idiom, with the mask naming the bits kept from the
//  old value.  Written out in that form because the equivalent
//  `(v & ~0x40) | (f & 0x40)` spelling needs an extra `not`.
//----------------------------------------------------------------------------
void __cdecl JASS_SetPlayerRacePreference(int hPlayer, unsigned int preference)
{
    SJassPlayer* player = ResolvePlayerHandle(hPlayer);

    if (player)
        player->m_raceFlags =
            preference ^ ((player->m_raceFlags ^ preference) & kPlayerFlagRaceSelectable);
}

//----------------------------------------------------------------------------
//  0x6F3C11D0 - `SetPlayerRaceSelectable` "(Hplayer;B)V", the one bit the
//  native above preserves.  Two separate returns rather than one masked
//  write: the dump branches and each arm has its own `retn`.
//----------------------------------------------------------------------------
void __cdecl JASS_SetPlayerRaceSelectable(int hPlayer, int selectable)
{
    SJassPlayer* player = ResolvePlayerHandle(hPlayer);

    if (!player)
        return;

    if (selectable)
        player->m_raceFlags |= kPlayerFlagRaceSelectable;
    else
        player->m_raceFlags &= ~kPlayerFlagRaceSelectable;
}

//----------------------------------------------------------------------------
//  0x6F3C1200 - `SetPlayerController` "(Hplayer;I)V".  The notify call is a
//  tail jump for the same reason RemoveLocation's is.
//----------------------------------------------------------------------------
void __cdecl JASS_SetPlayerController(int hPlayer, int controller)
{
    SJassPlayer* player = ResolvePlayerHandle(hPlayer);

    if (player)
    {
        player->m_controller = controller;
        NotifyPlayerControllerChanged(player);
    }
}

//----------------------------------------------------------------------------
//  0x6F3C1240 - `GetPlayerColor` "(Hplayer;)I".
//----------------------------------------------------------------------------
int __cdecl JASS_GetPlayerColor(int hPlayer)
{
    SJassPlayer* player = ResolvePlayerHandle(hPlayer);

    if (!player)
        return 0;

    return player->m_color;
}

//----------------------------------------------------------------------------
//  0x6F3C1260 - `IsPlayerRacePrefSet` "(Hplayer;I)B".  Bit 6 is masked back
//  out of the test, so the selectable flag never answers a race query even
//  when the caller asks for it.  The neg/sbb/neg tail is MSVC's `!= 0`.
//----------------------------------------------------------------------------
int __cdecl JASS_IsPlayerRacePrefSet(int hPlayer, unsigned int preference)
{
    SJassPlayer* player = ResolvePlayerHandle(hPlayer);

    if (!player)
        return 0;

    return (player->m_raceFlags & preference & ~kPlayerFlagRaceSelectable) != 0;
}

//----------------------------------------------------------------------------
//  0x6F3C1290 - `GetPlayerSelectable` "(Hplayer;)B", the same bit read back.
//----------------------------------------------------------------------------
int __cdecl JASS_GetPlayerSelectable(int hPlayer)
{
    SJassPlayer* player = ResolvePlayerHandle(hPlayer);

    if (!player)
        return 0;

    return (player->m_raceFlags >> 6) & 1;
}
