//============================================================================
//  The 0x6F3B2xxx converter module: every "turn this integer into a handle
//  of type X" native the registration table registers.
//
//  A JASS handle of an *enumerated* type is **the** integer value behind it,
//  so every one of these natives is the identity function: one
//  `mov eax,[esp+4]` and a bare `retn`.  They are separate shipped functions
//  at separate addresses even though their bodies are byte-identical, which
//  is what registering one entry point per JASS type name costs.
//
//  All forty-three were read out of their own dumps rather than assumed:
//  every one of the 0x6F3B2600 - 0x6F3B28B0 range really is those same two
//  instructions, with no mask, no bias and no bounds test anywhere in the
//  family.  The type names on the right of each comment are the registration
//  table's own (docs/targets/jass-natives-registration-table.md), which is
//  also what fixes the C++ name of each one.
//
//  0x6F3B27F0 sits inside this address run and is *not* one of these - the
//  range is dense but not gapless, and the four holes (0x6F3B26A0+0x10 is
//  taken, 0x6F3B2740 arrived with the location family, 0x6F3B27F0 and
//  0x6F3B28C0 onwards are other things) are exactly the entries the table
//  does not register as a converter.
//
//  One translation unit for all of them because they are one shipped module
//  and none of them calls another; nothing can be inlined away.  They are in
//  address order, which is also registration order.
//============================================================================
#include "jassnatives.h"

//----------------------------------------------------------------------------
//  0x6F3B2600 - `ConvertRace` "(I)Hrace;".
//----------------------------------------------------------------------------
int __cdecl JASS_ConvertRace(int value)
{
    return value;
}

//----------------------------------------------------------------------------
//  0x6F3B2610 - `ConvertAllianceType` "(I)Halliancetype;".
//----------------------------------------------------------------------------
int __cdecl JASS_ConvertAllianceType(int value)
{
    return value;
}

//----------------------------------------------------------------------------
//  0x6F3B2620 - `ConvertRacePref` "(I)Hracepreference;".
//----------------------------------------------------------------------------
int __cdecl JASS_ConvertRacePref(int value)
{
    return value;
}

//----------------------------------------------------------------------------
//  0x6F3B2630 - `ConvertIGameState` "(I)Higamestate;".
//----------------------------------------------------------------------------
int __cdecl JASS_ConvertIGameState(int value)
{
    return value;
}

//----------------------------------------------------------------------------
//  0x6F3B2640 - `ConvertFGameState` "(I)Hfgamestate;".
//----------------------------------------------------------------------------
int __cdecl JASS_ConvertFGameState(int value)
{
    return value;
}

//----------------------------------------------------------------------------
//  0x6F3B2650 - `ConvertPlayerState` "(I)Hplayerstate;".
//----------------------------------------------------------------------------
int __cdecl JASS_ConvertPlayerState(int value)
{
    return value;
}

//----------------------------------------------------------------------------
//  0x6F3B2660 - `ConvertPlayerScore` "(I)Hplayerscore;".
//----------------------------------------------------------------------------
int __cdecl JASS_ConvertPlayerScore(int value)
{
    return value;
}

//----------------------------------------------------------------------------
//  0x6F3B2670 - `ConvertPlayerGameResult` "(I)Hplayergameresult;".
//----------------------------------------------------------------------------
int __cdecl JASS_ConvertPlayerGameResult(int value)
{
    return value;
}

//----------------------------------------------------------------------------
//  0x6F3B2680 - `ConvertUnitState` "(I)Hunitstate;".
//----------------------------------------------------------------------------
int __cdecl JASS_ConvertUnitState(int value)
{
    return value;
}

//----------------------------------------------------------------------------
//  0x6F3B2690 - `ConvertAIDifficulty` "(I)Haidifficulty;".
//----------------------------------------------------------------------------
int __cdecl JASS_ConvertAIDifficulty(int value)
{
    return value;
}

//----------------------------------------------------------------------------
//  0x6F3B26A0 - `ConvertGameEvent` "(I)Hgameevent;".
//----------------------------------------------------------------------------
int __cdecl JASS_ConvertGameEvent(int value)
{
    return value;
}

//----------------------------------------------------------------------------
//  0x6F3B26B0 - `ConvertPlayerEvent` "(I)Hplayerevent;".
//----------------------------------------------------------------------------
int __cdecl JASS_ConvertPlayerEvent(int value)
{
    return value;
}

//----------------------------------------------------------------------------
//  0x6F3B26C0 - `ConvertPlayerUnitEvent` "(I)Hplayerunitevent;".
//----------------------------------------------------------------------------
int __cdecl JASS_ConvertPlayerUnitEvent(int value)
{
    return value;
}

//----------------------------------------------------------------------------
//  0x6F3B26D0 - `ConvertUnitEvent` "(I)Hunitevent;".
//----------------------------------------------------------------------------
int __cdecl JASS_ConvertUnitEvent(int value)
{
    return value;
}

//----------------------------------------------------------------------------
//  0x6F3B26E0 - `ConvertWidgetEvent` "(I)Hwidgetevent;".
//----------------------------------------------------------------------------
int __cdecl JASS_ConvertWidgetEvent(int value)
{
    return value;
}

//----------------------------------------------------------------------------
//  0x6F3B26F0 - `ConvertDialogEvent` "(I)Hdialogevent;".
//----------------------------------------------------------------------------
int __cdecl JASS_ConvertDialogEvent(int value)
{
    return value;
}

//----------------------------------------------------------------------------
//  0x6F3B2700 - `ConvertLimitOp` "(I)Hlimitop;".
//----------------------------------------------------------------------------
int __cdecl JASS_ConvertLimitOp(int value)
{
    return value;
}

//----------------------------------------------------------------------------
//  0x6F3B2710 - `ConvertUnitType` "(I)Hunittype;".
//----------------------------------------------------------------------------
int __cdecl JASS_ConvertUnitType(int value)
{
    return value;
}

//----------------------------------------------------------------------------
//  0x6F3B2720 - `ConvertGameSpeed` "(I)Hgamespeed;".
//----------------------------------------------------------------------------
int __cdecl JASS_ConvertGameSpeed(int value)
{
    return value;
}

//----------------------------------------------------------------------------
//  0x6F3B2730 - `ConvertPlacement` "(I)Hplacement;".
//----------------------------------------------------------------------------
int __cdecl JASS_ConvertPlacement(int value)
{
    return value;
}

//----------------------------------------------------------------------------
//  0x6F3B2740 - `ConvertStartLocPrio` "(I)Hstartlocprio;".
//----------------------------------------------------------------------------
int __cdecl JASS_ConvertStartLocPrio(int value)
{
    return value;
}

//----------------------------------------------------------------------------
//  0x6F3B2750 - `ConvertGameDifficulty` "(I)Hgamedifficulty;".
//----------------------------------------------------------------------------
int __cdecl JASS_ConvertGameDifficulty(int value)
{
    return value;
}

//----------------------------------------------------------------------------
//  0x6F3B2760 - `ConvertGameType` "(I)Hgametype;".
//----------------------------------------------------------------------------
int __cdecl JASS_ConvertGameType(int value)
{
    return value;
}

//----------------------------------------------------------------------------
//  0x6F3B2770 - `ConvertMapFlag` "(I)Hmapflag;".
//----------------------------------------------------------------------------
int __cdecl JASS_ConvertMapFlag(int value)
{
    return value;
}

//----------------------------------------------------------------------------
//  0x6F3B2780 - `ConvertMapVisibility` "(I)Hmapvisibility;".
//----------------------------------------------------------------------------
int __cdecl JASS_ConvertMapVisibility(int value)
{
    return value;
}

//----------------------------------------------------------------------------
//  0x6F3B2790 - `ConvertMapSetting` "(I)Hmapsetting;".
//----------------------------------------------------------------------------
int __cdecl JASS_ConvertMapSetting(int value)
{
    return value;
}

//----------------------------------------------------------------------------
//  0x6F3B27A0 - `ConvertMapDensity` "(I)Hmapdensity;".
//----------------------------------------------------------------------------
int __cdecl JASS_ConvertMapDensity(int value)
{
    return value;
}

//----------------------------------------------------------------------------
//  0x6F3B27B0 - `ConvertMapControl` "(I)Hmapcontrol;".
//----------------------------------------------------------------------------
int __cdecl JASS_ConvertMapControl(int value)
{
    return value;
}

//----------------------------------------------------------------------------
//  0x6F3B27C0 - `ConvertPlayerColor` "(I)Hplayercolor;".
//----------------------------------------------------------------------------
int __cdecl JASS_ConvertPlayerColor(int value)
{
    return value;
}

//----------------------------------------------------------------------------
//  0x6F3B27D0 - `ConvertPlayerSlotState` "(I)Hplayerslotstate;".
//----------------------------------------------------------------------------
int __cdecl JASS_ConvertPlayerSlotState(int value)
{
    return value;
}

//----------------------------------------------------------------------------
//  0x6F3B27E0 - `ConvertVolumeGroup` "(I)Hvolumegroup;".
//----------------------------------------------------------------------------
int __cdecl JASS_ConvertVolumeGroup(int value)
{
    return value;
}

//----------------------------------------------------------------------------
//  0x6F3B2800 - `ConvertRarityControl` "(I)Hraritycontrol;".
//----------------------------------------------------------------------------
int __cdecl JASS_ConvertRarityControl(int value)
{
    return value;
}

//----------------------------------------------------------------------------
//  0x6F3B2810 - `ConvertBlendMode` "(I)Hblendmode;".
//----------------------------------------------------------------------------
int __cdecl JASS_ConvertBlendMode(int value)
{
    return value;
}

//----------------------------------------------------------------------------
//  0x6F3B2820 - `ConvertTexMapFlags` "(I)Htexmapflags;".
//----------------------------------------------------------------------------
int __cdecl JASS_ConvertTexMapFlags(int value)
{
    return value;
}

//----------------------------------------------------------------------------
//  0x6F3B2830 - `ConvertFogState` "(I)Hfogstate;".
//----------------------------------------------------------------------------
int __cdecl JASS_ConvertFogState(int value)
{
    return value;
}

//----------------------------------------------------------------------------
//  0x6F3B2840 - `ConvertEffectType` "(I)Heffecttype;".
//----------------------------------------------------------------------------
int __cdecl JASS_ConvertEffectType(int value)
{
    return value;
}

//----------------------------------------------------------------------------
//  0x6F3B2850 - `ConvertVersion` "(I)Hversion;".
//----------------------------------------------------------------------------
int __cdecl JASS_ConvertVersion(int value)
{
    return value;
}

//----------------------------------------------------------------------------
//  0x6F3B2860 - `ConvertItemType` "(I)Hitemtype;".
//----------------------------------------------------------------------------
int __cdecl JASS_ConvertItemType(int value)
{
    return value;
}

//----------------------------------------------------------------------------
//  0x6F3B2870 - `ConvertAttackType` "(I)Hattacktype;".
//----------------------------------------------------------------------------
int __cdecl JASS_ConvertAttackType(int value)
{
    return value;
}

//----------------------------------------------------------------------------
//  0x6F3B2880 - `ConvertDamageType` "(I)Hdamagetype;".
//----------------------------------------------------------------------------
int __cdecl JASS_ConvertDamageType(int value)
{
    return value;
}

//----------------------------------------------------------------------------
//  0x6F3B2890 - `ConvertWeaponType` "(I)Hweapontype;".
//----------------------------------------------------------------------------
int __cdecl JASS_ConvertWeaponType(int value)
{
    return value;
}

//----------------------------------------------------------------------------
//  0x6F3B28A0 - `ConvertSoundType` "(I)Hsoundtype;".
//----------------------------------------------------------------------------
int __cdecl JASS_ConvertSoundType(int value)
{
    return value;
}

//----------------------------------------------------------------------------
//  0x6F3B28B0 - `ConvertPathingType` "(I)Hpathingtype;".
//----------------------------------------------------------------------------
int __cdecl JASS_ConvertPathingType(int value)
{
    return value;
}

//----------------------------------------------------------------------------
//  0x6F3B27F0 - `ConvertCameraField` "(I)Hcamerafield;".  The tenth of these
//  and the only one outside the 0x6F3B26xx/0x6F3B27xx run above; a
//  `camerafield` is its integer id the same way, and the numbering is the
//  one CCameraWar3::GetProperty switches on (cameragetproperty.cpp).
//----------------------------------------------------------------------------
int __cdecl JASS_ConvertCameraField(int value)
{
    return value;
}
