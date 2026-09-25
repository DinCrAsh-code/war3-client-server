//============================================================================
//  The fog-modifier and trigger natives at 0x6F3C1BC0..0x6F3C1C20, plus the
//  player-slot getters that sit with them.  See jassnatives.h.
//============================================================================
#include "jassnatives.h"

//----------------------------------------------------------------------------
//  0x6F3C1BC0 / 0x6F3C1BE0 - `FogModifierStart` / `FogModifierStop`, both
//  "(Hfogmodifier;)V": one bit of the modifier's own flag word.
//----------------------------------------------------------------------------
void __cdecl JASS_FogModifierStart(int hFogModifier)
{
    SJassFogModifier* modifier = ResolveFogModifierHandle(hFogModifier);

    if (modifier)
        modifier->m_flags |= kFogModifierRunning;
}

void __cdecl JASS_FogModifierStop(int hFogModifier)
{
    SJassFogModifier* modifier = ResolveFogModifierHandle(hFogModifier);

    if (modifier)
        modifier->m_flags &= ~kFogModifierRunning;
}

//----------------------------------------------------------------------------
//  0x6F3C1C20 - `ResetTrigger` "(Htrigger;)V".  The zero is materialised once
//  and reused as both the null comparand and the value stored into both
//  counters, which is what naming it does and what two literal `0`s does not.
//----------------------------------------------------------------------------
void __cdecl JASS_ResetTrigger(int hTrigger)
{
    SJassTrigger* trigger = ResolveTriggerHandle(hTrigger);

    int zero = 0;
    if (trigger != (SJassTrigger*)zero)
    {
        trigger->m_evalCount = zero;
        trigger->m_execCount = zero;
    }
}

//----------------------------------------------------------------------------
//  0x6F3C12B0 - `GetPlayerController` "(Hplayer;)Hmapcontrol;".  The
//  unresolvable answer is 5, not 0 - a real mapcontrol value, so a JASS
//  script cannot tell "no such player" from a player that really is that
//  control type.
//----------------------------------------------------------------------------
int __cdecl JASS_GetPlayerController(int hPlayer)
{
    SJassPlayer* player = ResolvePlayerHandle(hPlayer);

    if (!player)
        return 5;

    return player->m_controller;
}

//----------------------------------------------------------------------------
//  0x6F3C12D0 - `GetPlayerSlotState` "(Hplayer;)Hplayerslotstate;".
//----------------------------------------------------------------------------
int __cdecl JASS_GetPlayerSlotState(int hPlayer)
{
    SJassPlayer* player = ResolvePlayerHandle(hPlayer);

    if (!player)
        return 0;

    return *(int*)((char*)player + 0x270);
}

//----------------------------------------------------------------------------
//  0x6F3C12F0 - `SetPlayerUnitsOwner` "(Hplayer;I)V".  Past the resolve the
//  whole body is a call to an empty function (nullsub_14): the native is a
//  no-op in the shipped build.
//----------------------------------------------------------------------------
void __cdecl JASS_SetPlayerUnitsOwner(int hPlayer, int newOwner)
{
    SJassPlayer* player = ResolvePlayerHandle(hPlayer);

    if (player)
        ((SJassPlayerUnits*)player)->SetOwner(newOwner);
}
