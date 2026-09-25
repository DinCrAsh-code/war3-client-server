//============================================================================
//  0x6F0028C0 - CPreferencesWar3::ApplyPreference: push one preference
//  id/value pair at whichever subsystem owns that preference.  432
//  instructions and the largest thing in this batch.
//
//  One flat `switch (key)` over ids 5..63, with 30 live cases and a default
//  that answers 0.  MSVC compiles it through the byte index table at
//  0x6F002EBC into the 59-entry jump table at 0x6F0028EA, and the case
//  bodies come out in source order (docs/msvc-vc8-idioms.md, "Switch case
//  blocks come out in source order").
//
//  So the cases below are **not** in numeric order: they are in the order
//  the shipped blocks appear, which IDA states outright (every block in the
//  dump carries a `jumptable 6F0028EA case <n>` comment).  That order is
//  6, 8, 48, 10, 11, 12, 9, 5, 14, 15, 21, 22, 24..32, 39..43, 56, 57, 63,
//  and writing them numerically instead costs ~50 instructions - case 5's
//  whole float block lands at the top of the function where the shipped
//  code has case 6's three-instruction range test.  Case 48 has no body of
//  its own at all: the shipped jump table sends it at case 8's
//  `mov esi,1` tail, which is what writing it directly after case 8
//  reproduces.
//
//  A `switch` nested *inside* a case (ids 11 and 12) is small enough that
//  MSVC turns it into the `sub eax,0` / `jz` / `sub eax,1` / `jz` chain the
//  same file documents - which is why case 12 is a switch with a `default`
//  and not the if/else-if chain it reads as (that spelling emits
//  `test`/`cmp` against each value and costs six instructions).
//
//  Three shapes recur and each is an idiom rather than a choice here:
//
//  * `test eax,eax` / `fild` / `jge` / `fadd 4294967296.0` is an *unsigned*
//    int-to-float conversion, so every percentage below is divided as
//    `(unsigned)value`;
//  * `xor ecx,ecx` / `test` / `setnz cl` is a `!= 0` passed as an argument,
//    and `mov ecx,1` / `cmp ecx,esi` / `sbb` / `neg` is `(unsigned)value > 1`;
//  * `xor edx,edx` / `lea ecx,[edx+1]` is `GetGameUI(1, 0)` - the same
//    constant-off-a-zeroed-register the camera natives already record.
//
//  What the *ids* mean is not recoverable from this call tree: the shipped
//  defaults array (0x6F8755B0) holds them, but nothing in this dump reads
//  its strings, so the setters below are named for the value each one takes
//  and the subsystem it lives in, and nothing more.  Every one of them is a
//  redirect (b5thunks.cpp); the names are this file's, not the binary's.
//
//  Cases 56 and 57 deliberately answer 0 - they run their setter and then
//  return the still-zero result variable - which is why the two are written
//  without `result = 1`.  That is in the shipped stream (`mov eax, esi`
//  with esi never assigned on those paths), not a transcription slip.
//
//  Own translation unit: every one of these is a real call.
//============================================================================
#include "preferences.h"
#include "gameui.h"

//  --- the 0x6F00Fxxx / 0x6F00Bxxx / 0x6F001020 options block ---------------
void __fastcall SetClampedModeSetting(int mode);          // 0x6F001020
void __fastcall SetSoundActiveSetting(int on);            // 0x6F00FC00
void __fastcall SetSoundIdleSetting(int off);             // 0x6F00BFA0
//  Returns 1 when the scale was inside (0, 1) and actually stored, 0
//  otherwise - Storm/stormsingletonapreferencesinks.cpp.  This caller
//  discards it, which compiles identically either way.
int  __stdcall  SetQualityScaleSetting(float scale);      // 0x6F00F2D0
void __fastcall SetQualityAboveOneSetting(int on);        // 0x6F00FB00
void __fastcall SetQualityNonZeroSetting(int on);         // 0x6F00FBC0
void __fastcall SetQualityLightingSetting(int on);        // 0x6F00F960
void __fastcall SetQualityEffectsSetting(int on);         // 0x6F00FB60
void __fastcall SetBooleanDisplaySetting(int on);         // 0x6F00FA60
void __fastcall SetDisplayModeSetting(int mode);          // 0x6F00F720

//  --- the 0x6F525xxx block ---------------------------------------------
//  Cases 9/5/10 do NOT reach a sound subsystem: CItem vtable-closure batch
//  6 traced all four addresses to real disassembly and found each one is a
//  forward onto g_renderDevice's own vtable (Render/renderdeviceslot18_1c_
//  20_dc.cpp) - graphics-option setters/getters, not sound ones. The names
//  below were this file's own guess before that reconstruction landed
//  ("the setters below are named for the value each one takes... nothing
//  in this dump reads [the id table's] strings" - see this file's own
//  header above); they were wrong, not just approximate, so the real,
//  disassembly-confirmed names replace them here rather than being kept as
//  a second, redundant redirect under the old guess.
unsigned int GetRenderDeviceField0xDC();                    // 0x6F525F30 (__cdecl, matching the real definition's own unmarked convention - it takes no arguments, so this has no ABI effect on the naked body, but the declared convention must match the definition's for name mangling to agree)
void __fastcall RenderDeviceSlot0x20Forward(unsigned int);  // 0x6F525EE0
void __fastcall RenderDeviceSlot0x18Forward(unsigned int);  // 0x6F525EA0
void __stdcall  RenderDeviceSlot0x1CForward(float);         // 0x6F525EC0

//  --- the render / world blocks -------------------------------------------
//  0x6F32A210 - `retn 4`: the feature bit in ecx, the value in edx and one
//  stack dword the caller always passes as zero.
void __fastcall SetRenderFeatureSetting(unsigned int feature, int value,
                                        int flags);       // 0x6F32A210
void __stdcall  SetTerrainPreferenceFraction(float f);    // 0x6F30A650
void __stdcall  SetEffectsPreferenceFraction(float f);    // 0x6F30A7A0
void __fastcall SetFrameIntervalSetting(int interval);    // 0x6F62B040

//  0x6F7B4400 / 0x6F021740 - Misc/sweep_predicates.cpp and
//  Misc/trivialaccessors_02_03.cpp got to these two addresses first; one
//  shipped address carries one C++ name, so both keep theirs.
//  Spelled with exactly the parameter types those two files give them -
//  `unsigned int` and `void*`, not `int` - because MSVC puts the parameter
//  types in the mangled name: an `int` re-declaration is a second symbol that
//  compiles, scores and never links (link_check.py, cause 1).
void __fastcall SetClampedTo8_6F7B4400(unsigned int level);   // 0x6F7B4400
void __fastcall PublishGlobal_6F021740(void* value);          // 0x6F021740

//  --- CGameUI's and CCameraWar3's own preference sinks ---------------------
//  0x6F2F3ED0 / 0x6F2F3F20 are `retn 4` with the singleton in ecx and the
//  value on the stack, which is __thiscall and not __fastcall - a __fastcall
//  pair would take the value in edx and clean four bytes too few, the exact
//  mismatch tools/thunk_abi_audit.py exists to catch.  They are spelled on a
//  receiver struct of their own rather than added to CGameUI, because
//  nothing establishes that CGameUI is what the shipped declaration named.
struct SGameUiPreferenceSink
{
    void SetPrimary(int value);      // 0x6F2F3ED0
    void SetSecondary(int value);    // 0x6F2F3F20
};

//  The camera pair is genuinely __fastcall: this convention never puts a
//  float in a register, so the receiver takes ecx and the value still lands
//  on the stack (docs/msvc-vc8-idioms.md, "`__fastcall` is how you get
//  'pointer in ecx, floats on the stack'").
void __fastcall SetCameraPreferenceFractionPrimary(CCameraWar3* camera,
                                                   float f);   // 0x6F302A30
void __fastcall SetCameraPreferenceFractionSecondary(CCameraWar3* camera,
                                                     float f); // 0x6F302A70

//  The three quality scales preference id 11 picks between, and the two
//  doubles every percentage below is folded through.  flt_6F875AB8 is the
//  2^32 the unsigned int-to-float idiom adds back; it is declared, not
//  spelled, because the operand the canonicaliser compares is the symbol.
extern const float  g_preferenceQualityScale0;   // flt_6F87558C
extern const float  g_preferenceQualityScale1;   // flt_6F875588
extern const float  g_preferenceQualityScale2;   // flt_6F875584
extern const double g_preferencePercentDivisor;  // dbl_6F875AB0
extern const double g_preferenceScaleBias;       // dbl_6F875AA8
extern const double g_timeScaleUnitsPerSecond;   // dbl_6F875AA0

int CPreferencesWar3::ApplyPreference(int key, int value)
{
    if (key >= 0x4F)
        return 0;

    int result = 0;

    switch (key)
    {
    case 6:
        if ((unsigned int)value > 2)
            break;
        SetClampedModeSetting(value);
        result = 1;
        break;

    case 8:
        SetSoundActiveSetting(value != 0);
        SetSoundIdleSetting(value == 0);
        result = 1;
        break;

    case 48:
        result = 1;
        break;

    case 10:
    {
        //  Only the two bits 0x10 and 0x20 mean anything here, and the
        //  shipped code refuses everything else outright rather than
        //  falling through to the default answer.
        if (value != 0x10 && value != 0x20)
            return 0;

        int playing = GetRenderDeviceField0xDC();
        if (value == 0x10)
        {
            if (playing)
                RenderDeviceSlot0x20Forward(0);
        }
        else if (value == 0x20)
        {
            if (!playing)
                RenderDeviceSlot0x20Forward(value - 0x1F);
        }
        result = 1;
        break;
    }

    case 11:
    {
        float scale = 1.0f;
        switch (value)
        {
        case 0: scale = g_preferenceQualityScale0; break;
        case 1: scale = g_preferenceQualityScale1; break;
        case 2: scale = g_preferenceQualityScale2; break;
        }
        SetQualityScaleSetting(scale);
        SetQualityAboveOneSetting((unsigned int)value > 1);

        int on = (value != 0);
        SetQualityNonZeroSetting(on);
        SetQualityLightingSetting(on);
        SetQualityEffectsSetting(on);
        result = 1;
        break;
    }

    case 12:
        switch (value)
        {
        case 0:  SetClampedTo8_6F7B4400(1); break;
        case 1:  SetClampedTo8_6F7B4400(4); break;
        default: SetClampedTo8_6F7B4400(8); break;
        }
        result = 1;
        break;

    case 9:
        RenderDeviceSlot0x18Forward(value);
        result = 1;
        break;

    case 5:
        //  The one percentage that is inverted and biased rather than
        //  plainly scaled: 100 - value, over the divisor, plus the bias.
        RenderDeviceSlot0x1CForward(
            (float)((unsigned int)(100 - value) / g_preferencePercentDivisor
                    + g_preferenceScaleBias));
        result = 1;
        break;

    case 14:
        SetBooleanDisplaySetting(value != 0);
        result = 1;
        break;

    case 15:
        SetDisplayModeSetting(value);
        result = 1;
        break;

    case 21:
        PublishGlobal_6F021740((void*)value);
        result = 1;
        break;

    case 22:
        //  A rate expressed as a period: the reciprocal of the value, in
        //  the same units per second the sprite time scale uses.
        //
        //  Truncated to **unsigned** and not to `int`: the shipped block is
        //  the inline `fnstcw` / `or 0C00h` / `fldcw` / `fistp qword` /
        //  `fldcw` sequence, which is what an unsigned destination lowers
        //  to on this toolchain where an `int` one is a bare call to
        //  `__ftol2_sse` (docs/msvc-vc8-idioms.md, "A float-to-`unsigned`
        //  truncation is inline; a float-to-`int` one is a call").  The
        //  qword the `fistp` writes is this function's whole 8-byte frame:
        //  spelling this `(int)` loses the `sub esp,8` prologue, all 34
        //  `add esp,8` epilogues and the 8-byte displacement on every
        //  [esp+N] in the other 28 cases - 90 instructions for one cast.
        SetFrameIntervalSetting(
            (unsigned int)(1.0 / (unsigned int)value * g_timeScaleUnitsPerSecond));
        result = 1;
        break;

    case 24:
        SetRenderFeatureSetting(1, value, 0);
        result = 1;
        break;

    case 25:
        SetRenderFeatureSetting(2, value, 0);
        result = 1;
        break;

    case 26:
        SetRenderFeatureSetting(0x200, value, 0);
        result = 1;
        break;

    case 27:
        SetTerrainPreferenceFraction(
            (float)((unsigned int)value / g_preferencePercentDivisor));
        result = 1;
        break;

    case 28:
        SetRenderFeatureSetting(8, value, 0);
        result = 1;
        break;

    case 29:
        SetEffectsPreferenceFraction(
            (float)((unsigned int)value / g_preferencePercentDivisor));
        result = 1;
        break;

    case 30:
        SetRenderFeatureSetting(0x400, value, 0);
        result = 1;
        break;

    case 31:
        SetRenderFeatureSetting(0x10, value, 0);
        result = 1;
        break;

    case 32:
        SetRenderFeatureSetting(0x20, value, 0);
        result = 1;
        break;

    case 39:
    {
        CGameUI* ui = (CGameUI*)GetGameUI(1, 0);
        if (ui == 0)
            break;
        SetCameraPreferenceFractionPrimary(
            ui->GetCamera(),
            (float)((unsigned int)value / g_preferencePercentDivisor));
        result = 1;
        break;
    }

    case 40:
    {
        CGameUI* ui = (CGameUI*)GetGameUI(1, 0);
        if (ui == 0)
            break;
        ui->GetCamera()->m_preferenceField30 = value;
        result = 1;
        break;
    }

    case 41:
    {
        CGameUI* ui = (CGameUI*)GetGameUI(1, 0);
        if (ui == 0)
            break;
        SetCameraPreferenceFractionSecondary(
            ui->GetCamera(),
            (float)((unsigned int)value / g_preferencePercentDivisor));
        result = 1;
        break;
    }

    case 42:
    {
        CGameUI* ui = (CGameUI*)GetGameUI(1, 0);
        if (ui == 0)
            break;
        ui->m_option1D0 = value;
        result = 1;
        break;
    }

    case 43:
    {
        CGameUI* ui = (CGameUI*)GetGameUI(1, 0);
        if (ui == 0)
            break;
        ui->m_cursorLocked = value;
        result = 1;
        break;
    }

    case 56:
    {
        //  No `result = 1`: the shipped tail returns the still-zero result
        //  variable whether or not the singleton was there.
        SGameUI* ui = GetGameUI(1, 0);
        if (ui != 0)
            ((SGameUiPreferenceSink*)ui)->SetPrimary(value);
        break;
    }

    case 57:
    {
        SGameUI* ui = GetGameUI(1, 0);
        if (ui != 0)
            ((SGameUiPreferenceSink*)ui)->SetSecondary(value);
        break;
    }

    case 63:
    {
        CGameUI* ui = (CGameUI*)GetGameUI(1, 0);
        if (ui == 0)
            break;
        ui->m_option1D4 = value;
        result = 1;
        break;
    }

    default:
        result = 0;
        break;
    }

    return result;
}
