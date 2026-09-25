//============================================================================
//  Redirects for the callees the JASS_PanCameraToTimedWithZ closure's
//  preference and sprite slice calls but does not reconstruct - the
//  preference sinks, the two
//  preference tables' own constructors and growth, and the sprite engine's
//  leaves.  Same shape and same rules as Misc/unreconstructed_thunks.cpp:
//  each body jumps to the shipped function at its real address, so an
//  unreconstructed callee simply runs original code, and each one goes away
//  as its real body lands.
//
//  NOT reconstructions.  The declared argument bytes have to agree with the
//  shipped `retn <n>` of every one of these - that is what
//  tools/thunk_abi_audit.py checks and what two live crashes came from -
//  and the `retn` used for each is in the comment beside it.
//============================================================================
#include "refcnt.h"
#include "preferences.h"
#include "gameui.h"
#include "stormsingletona.h"   // g_unkDblQueryCellBias (dbl_6F875120)

#define ADDR_THUNK(sig, addr)                     \
    __declspec(naked) sig                         \
    {                                             \
        __asm { mov eax, addr }                   \
        __asm { jmp eax }                         \
    }

//----------------------------------------------------------------------------
//  The preference store's own leaves.
//----------------------------------------------------------------------------
//  0x6F0047B0 / 0x6F004AB0 - `retn 0`, the two tables' constructors.
ADDR_THUNK(void __fastcall ConstructPreferenceTable(void*), 0x6F0047B0)
ADDR_THUNK(void __fastcall ConstructPreferenceCache(void*), 0x6F004AB0)
//  0x6F0055A0, 0x6F004FE0, 0x6F004E10 - real bodies now, in
//  Config/preferencehashtable.cpp (TSHashTable<T,K>'s own NewNode/Rehash,
//  instantiated for both tables).
//  0x6F005A30 - `retn 0Ch`.  CItem-batch1 traced this to
//  GameMainHashLookupB3::Method_6F004330 (already a thunk) plus a new,
//  not-yet-declared CConfigSource member (0x6F6F3520) - not chased to
//  ground in that session's remaining budget, so still a redirect here.
ADDR_THUNK(int CPreferencesWar3::GetStringValue(int, char*, unsigned int),
           0x6F005A30)

//----------------------------------------------------------------------------
//  The subsystems CPreferencesWar3::ApplyPreference pushes values at.  Named
//  in preferenceapply.cpp for the value each takes; the shipped preference-id
//  table is not in this dump.
//----------------------------------------------------------------------------
//  `retn 0` - one integer, in ecx.
ADDR_THUNK(void __fastcall SetClampedModeSetting(int), 0x6F001020)
//  0x6F00FC00/BFA0/FB00/FBC0/F960/FB60/FA60/F720 - real bodies now,
//  Storm/stormsingletonapreferencesinks.cpp (CItem-batch1).
ADDR_THUNK(void __fastcall SetFrameIntervalSetting(int), 0x6F62B040)
//  0x6F525EA0/EC0/EE0/F30 ("SetSoundProviderSetting"/"SetSoundVolumeSetting"/
//  "SetMusicState"/"QueryMusicState" here previously) are not a sound
//  subsystem at all: CItem vtable-closure batch 6 traced all four to real
//  disassembly and found each is a forward onto g_renderDevice's own
//  vtable. Real bodies now in Render/renderdeviceslot18_1c_20_dc.cpp,
//  called under their disassembly-confirmed names from
//  Config/preferenceapply.cpp; not re-thunked here under the old guess.
//  0x6F30A7A0 - CItem vtable-closure batch 2 also reaches this one, but its
//  body (an 8-category flt_6FAB5068[]/dword_6FA76058[] blend loop feeding an
//  unreconstructed sub_6F6C8950) is real, unmapped game logic well past this
//  batch's budget - left `TODO` deliberately rather than guessed at; still
//  thunked here so SetTerrainPreferenceFraction's own neighbour keeps
//  linking.
ADDR_THUNK(void __stdcall SetEffectsPreferenceFraction(float), 0x6F30A7A0)
//  0x6F32A210 (SetRenderFeatureSetting) already has a real body below in
//  this same file; 0x6F302A30/0x6F302A70 have real bodies in
//  GameUI/cameraweightclamp.cpp (see this file's own note further down) -
//  neither is thunked here.

//----------------------------------------------------------------------------
//  CItem vtable-closure batch 2: the rest of CPreferencesWar3::
//  ApplyPreference's own sinks that were still thunked above.  Every callee
//  below (sub_6F324A60/6F324B10/6F3029B0/6F3029F0/6F6C8950/6F33AC00/
//  6F314DD0/6F2F3000) sits outside this batch's 36 addresses and stays a
//  bare extern - out of scope for this pass, not missed.
//----------------------------------------------------------------------------

//  0x6F30A650 - `retn 4`: one float on the stack, no `this`.  Caches the
//  raw fraction in a module-global (flt_6FAB50A4) then hands
//  `f * flt_6FAB507C` on to sub_6F6C8950 with a constant `7` in ecx (an
//  unreconstructed leaf outside this batch - the constant's meaning does
//  not surface in this call tree).
extern float g_terrainPreferenceFractionCache;   // flt_6FAB50A4
extern const float g_terrainPreferenceScale;     // flt_6FAB507C
//  `retn 4` (verified: `mov eax,1 / retn 4`).  A naked ADDR_THUNK-shaped
//  redirect, not a bodyless extern: nothing on the real link line defines
//  a bare `sub_<addr>` symbol either.
ADDR_THUNK(void __fastcall ApplyScaledTerrainFraction(int, float), 0x6F6C8950)
void __stdcall SetTerrainPreferenceFraction(float f)
{
    g_terrainPreferenceFractionCache = f;
    ApplyScaledTerrainFraction(7, f * g_terrainPreferenceScale);
}

//  0x6F32A210 - `retn 4`: two in registers, one on the stack.  `value`
//  picks which of the two shipped setters gets `feature`/`flags`; nothing
//  in this call tree gives either one a real name.
//  Both `retn 0` (verified: `pop esi / pop ecx / retn`) - naked
//  ADDR_THUNK-shaped redirects, not bodyless externs: nothing on the real
//  link line defines a bare `sub_<addr>` symbol either.
ADDR_THUNK(void __fastcall EnableRenderFeature(unsigned int, int), 0x6F324A60)
ADDR_THUNK(void __fastcall DisableRenderFeature(unsigned int, int), 0x6F324B10)
void __fastcall SetRenderFeatureSetting(unsigned int feature, int value,
                                        int flags)
{
    if (value != 0)
        EnableRenderFeature(feature, flags);
    else
        DisableRenderFeature(feature, flags);
}

//  0x6F302A30 / 0x6F302A70 and their own two callees (0x6F3029B0/0x6F3029F0)
//  all have real bodies now, in GameUI/cameraweightclamp.cpp (a sibling
//  batch's own reconstruction, which went one level deeper than this batch
//  did and gave the two callees real bodies too, rather than leaving them
//  thunked) - not duplicated here.

//  0x6F2F3ED0 - `retn 4`, __thiscall on CGameUI itself: `[ecx+0x3C0]` is
//  `m_pMinimapObject` (gameui.h).  A three-way `value` dispatch (1 / 2 /
//  anything else) toggles the minimap's own +0x638 field
//  (SPlainFieldOwner::SetField638, 0x6F33AC00 - already named, generic
//  field-owner setter family) and a second, still-unreconstructed mode
//  setter (0x6F314DD0, out of this batch), then refreshes the ally-color
//  minimap button skin.
//  `retn 0` (verified: its own tail-jmp body ends `... / retn`) - a naked
//  ADDR_THUNK-shaped redirect, not a bodyless extern: nothing on the real
//  link line defines a bare `sub_<addr>` symbol either.
ADDR_THUNK(void __fastcall SetAllyColorModeFlag(int), 0x6F314DD0)

//  0x6F33AC00 - SPlainFieldOwner::SetField638, already named and bodied
//  (Misc/sweep_field_setters.cpp) - `__thiscall`, one stack `int`.
struct SPlainFieldOwner
{
    void __thiscall SetField638(int value);
};
//  0x6F33ABB0 - CMinimap::SetShowCreeps, already named and bodied
//  (GameUI/minimaprefresh.cpp) - `__thiscall`, one stack `int`.
class CMinimap
{
public:
    void __thiscall SetShowCreeps(int show);
};

struct SGameUiPreferenceSink
{
    void SetPrimary(int value);
    void SetSecondary(int value);

    char m_reserved000[0x3C0];
    SPlainFieldOwner* m_pMinimapObject;   // +0x3C0
};

//  0x6F2F3000 / 0x6F2F30C0 - CGameUI::RefreshMiniMapAllyButtonSkin /
//  RefreshMiniMapCreepButtonSkin, already declared and bodied on the real
//  class (gameui.h) - reached through `this` cast back to `CGameUI*` rather
//  than re-declared here, since a re-declaration under this local struct's
//  own name would mangle to a different, bodyless symbol.
void SGameUiPreferenceSink::SetPrimary(int value)
{
    SPlainFieldOwner* minimap = m_pMinimapObject;
    switch (value)
    {
    case 1:
        minimap->SetField638(0);
        SetAllyColorModeFlag(1);
        break;
    case 2:
        minimap->SetField638(0);
        SetAllyColorModeFlag(0);
        break;
    default:
        minimap->SetField638(1);
        SetAllyColorModeFlag(1);
        break;
    }
    ((CGameUI*)this)->RefreshMiniMapAllyButtonSkin();
}
void SGameUiPreferenceSink::SetSecondary(int value)
{
    ((CMinimap*)m_pMinimapObject)->SetShowCreeps(value != 0);
    ((CGameUI*)this)->RefreshMiniMapCreepButtonSkin();
}

//----------------------------------------------------------------------------
//  The four handle-object constructors proper.  Each takes its `this` in ecx
//  and cleans nothing (`retn 0`), and each is the call the matching creator
//  in the same family makes after SMemAlloc - MakeCameraController,
//  MakeCameraEventSink, CreateFogObject, CreateLightObject.  Their own
//  bodies are the next slice's; these are redirects.
//----------------------------------------------------------------------------
ADDR_THUNK(TRefCnt* __fastcall ConstructCameraControllerObject(void*), 0x6F4E7790)
ADDR_THUNK(TRefCnt* __fastcall ConstructCameraEventSinkObject(void*), 0x6F4E9DA0)
ADDR_THUNK(TRefCnt* __fastcall ConstructFogObject(void*), 0x6F4EA730)
ADDR_THUNK(TRefCnt* __fastcall ConstructLightObject(void*), 0x6F4EAA30)

#undef ADDR_THUNK
