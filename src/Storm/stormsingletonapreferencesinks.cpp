//============================================================================
//  CItem-vtable-closure batch 1 - the nine `void __fastcall/__stdcall
//  SetXxxSetting(...)` bodies CPreferencesWar3::ApplyPreference pushes
//  values at (Config/preferenceapply.cpp declares every one of these; they
//  used to be redirects in Misc/spritehandlethunks.cpp - see that file's
//  own remaining entries for the rest of the family this batch does not
//  own).
//
//  Five of them (0x6F00FA60/FB00/FB60/FBC0/FC00) share one shape exactly:
//  fetch the lazily-constructed Storm singleton (GetStormSingletonA), set
//  or clear one bit of its +0x970 option word depending on the incoming
//  bool, and poke SStormTerrainReleaseTarget::ReleaseGate4 with that same
//  bit value either way (own translation unit there,
//  Jass/jassstormterraingates.cpp - already a real member, not a redirect
//  this batch has to add).  The two branches genuinely duplicate the
//  ReleaseGate4 call in the shipped stream rather than sharing one tail; this
//  is the shipped shape's own choice, not something these five have in
//  common with any other function this repo has needed to explain.
//
//  0x6F00BFA0 and 0x6F00F720/0x6F00F960 are a different, smaller shape: no
//  null test at all, straight into a one-argument member call on the
//  singleton.  0x6F00F2D0 stores the incoming float and asks the sound
//  subsystem to refresh only when it is strictly greater than 1.0 - both
//  x87 comparisons are `test ah,5`/`jnp`, i.e. "bail unless strictly
//  greater" (docs/msvc-vc8-idioms.md's `!(a > b)` idiom), chained so the
//  survivors are `scale > 0.0f && scale > 1.0f`.
//
//  The three genuinely out-of-batch callees this needs
//  (SStormSingletonA::Method_6F744220_SetOption964,
//  SStormSingletonA::Method_6F75A2E0_SetOption21E4, and the qualityscale
//  refresh 0x6F4E3D60) are outside this batch's 36 addresses, still
//  `TODO`, and not this session's to claim - each is redirected here the
//  same way Jass/jassstormterraingates.cpp redirects its own out-of-batch
//  neighbours, so the tree still links and still runs the genuine shipped
//  body.
//============================================================================
#include "stormsingletona.h"

SStormSingletonA* __fastcall GetStormSingletonA();      // 0x6F01F5A0

//  0x6F765240 - already a real member (Jass/jassstormterraingates.cpp);
//  redeclared here, matching that definition exactly, to link against it.
struct SStormTerrainReleaseTarget
{
    void ReleaseGate4(int which);
};

//  0x6F73AED0 - already a real member (SetField04,
//  Misc/sweep_field_setters.cpp); redeclared here to link against it.
struct SPlainFieldOwner
{
    void __thiscall SetField04(int value);
};

//  Out-of-batch callees, redirected so this TU links against the genuine
//  shipped body.  Each is still `TODO` in agent_worktrees/ - not claimed or
//  reconstructed here.
namespace
{
    #define OUT_OF_BATCH_THUNK(sig, addr)              \
        __declspec(naked) sig                          \
        {                                               \
            __asm { mov eax, addr }                     \
            __asm { jmp eax }                            \
        }
    //  0x6F4E3D60 - `retn 4`, one stack float, no register argument.
    OUT_OF_BATCH_THUNK(int __stdcall RefreshQualityScale(float value),
                        0x6F4E3D60)
    #undef OUT_OF_BATCH_THUNK
}

//  0x6F744220 - `this` in ecx only, one stack dword, `retn 4` - a plain
//  thiscall member, not `__fastcall`.
struct SStormSingletonAOptionField964Sink
{
    void SetOptionField964(int value);
};
__declspec(naked) void SStormSingletonAOptionField964Sink::SetOptionField964(int)
{
    __asm { mov eax, 06F744220h }
    __asm { jmp eax }
}

//  0x6F75A2E0 - `this` in ecx, one stack dword, `retn 4`.  Genuinely a
//  member of the singleton itself (writes its own +0x21E4 field), so it is
//  declared as one rather than as a free function.
struct SStormSingletonASoundSink
{
    void SetOptionField21E4(int value);        // 0x6F75A2E0
};
__declspec(naked) void SStormSingletonASoundSink::SetOptionField21E4(int)
{
    __asm { mov eax, 06F75A2E0h }
    __asm { jmp eax }
}

//----------------------------------------------------------------------------
//  The five `or/and [singleton+970h], BIT` / ReleaseGate4(BIT) setters.
//----------------------------------------------------------------------------
//  0x6F00FA60
void __fastcall SetBooleanDisplaySetting(int on)
{
    SStormSingletonA* s = GetStormSingletonA();
    if (s == 0)
        return;
    if (on)
    {
        s->m_optionFlags970 |= 0x100;
        ((SStormTerrainReleaseTarget*)s)->ReleaseGate4(0x100);
    }
    else
    {
        s->m_optionFlags970 &= ~0x100u;
        ((SStormTerrainReleaseTarget*)s)->ReleaseGate4(0x100);
    }
}

//  0x6F00FB00
void __fastcall SetQualityAboveOneSetting(int on)
{
    SStormSingletonA* s = GetStormSingletonA();
    if (s == 0)
        return;
    if (on)
    {
        s->m_optionFlags970 |= 0x800;
        ((SStormTerrainReleaseTarget*)s)->ReleaseGate4(0x800);
    }
    else
    {
        s->m_optionFlags970 &= ~0x800u;
        ((SStormTerrainReleaseTarget*)s)->ReleaseGate4(0x800);
    }
}

//  0x6F00FB60
void __fastcall SetQualityEffectsSetting(int on)
{
    SStormSingletonA* s = GetStormSingletonA();
    if (s == 0)
        return;
    if (on)
    {
        s->m_optionFlags970 |= 0x1000;
        ((SStormTerrainReleaseTarget*)s)->ReleaseGate4(0x1000);
    }
    else
    {
        s->m_optionFlags970 &= ~0x1000u;
        ((SStormTerrainReleaseTarget*)s)->ReleaseGate4(0x1000);
    }
}

//  0x6F00FBC0
void __fastcall SetQualityNonZeroSetting(int on)
{
    SStormSingletonA* s = GetStormSingletonA();
    if (s == 0)
        return;
    if (on)
    {
        s->m_optionFlags970 |= 0x2000;
        ((SStormTerrainReleaseTarget*)s)->ReleaseGate4(0x2000);
    }
    else
    {
        s->m_optionFlags970 &= ~0x2000u;
        ((SStormTerrainReleaseTarget*)s)->ReleaseGate4(0x2000);
    }
}

//  0x6F00FC00
void __fastcall SetSoundActiveSetting(int on)
{
    SStormSingletonA* s = GetStormSingletonA();
    if (s == 0)
        return;
    if (on)
    {
        s->m_optionFlags970 |= 0x4000;
        ((SStormTerrainReleaseTarget*)s)->ReleaseGate4(0x4000);
    }
    else
    {
        s->m_optionFlags970 &= ~0x4000u;
        ((SStormTerrainReleaseTarget*)s)->ReleaseGate4(0x4000);
    }
}

//----------------------------------------------------------------------------
//  The plain "get the singleton, call one member on it" trio.  None of the
//  three checks the singleton for null - the shipped stream simply doesn't.
//----------------------------------------------------------------------------
//  0x6F00BFA0
void __fastcall SetSoundIdleSetting(int off)
{
    ((SStormSingletonASoundSink*)GetStormSingletonA())->SetOptionField21E4(off);
}

//  0x6F00F720 - a plain store to the singleton's own +0x04 field
//  (SPlainFieldOwner::SetField04, already real - Misc/sweep_field_setters.cpp).
void __fastcall SetDisplayModeSetting(int mode)
{
    SStormSingletonA* s = GetStormSingletonA();
    if (s != 0)
        ((SPlainFieldOwner*)s)->SetField04(mode);
}

//  0x6F00F960
void __fastcall SetQualityLightingSetting(int on)
{
    SStormSingletonA* s = GetStormSingletonA();
    if (s != 0)
        ((SStormSingletonAOptionField964Sink*)s)->SetOptionField964(on);
}

//----------------------------------------------------------------------------
//  0x6F00F2D0 - store the scale and ask the sound subsystem to refresh only
//  when it is strictly greater than 1.0 (see the file header for the two
//  x87 comparisons this comes from).
//----------------------------------------------------------------------------
float g_preferenceQualityScaleStore;       // flt_6FA4FA98
int   g_qualityScaleRefreshRequested;      // dword_6FAAE664

int __stdcall SetQualityScaleSetting(float scale)
{
    if (!(scale > 0.0f))
        return 0;
    if (!(scale > 1.0f))
        return 0;

    g_preferenceQualityScaleStore = scale;
    g_qualityScaleRefreshRequested = 1;
    RefreshQualityScale(scale);
    return 1;
}
