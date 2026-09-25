//============================================================================
//  Three small, address-neighbourhood-unrelated leaves this batch's own
//  callers reach: the camera's own "last constructed" self-registration
//  setter, a tick-count-to-seconds helper, and a plain degrees-to-radians
//  conversion already used inline by ResetToDefaults and
//  Method_0x20/cameraregisterproperties.cpp (spelled out again here as its
//  own function, since 0x6F301B70 is the shipped image's own separate
//  entry point for the identical arithmetic).
//============================================================================
#include "CFloat.h"
#include "gameui.h"

extern "C" unsigned int __stdcall GetTickCount();

//----------------------------------------------------------------------------
//  0x6F3783E0 - CCameraWar3::GetField84.  `retn 0`.
//----------------------------------------------------------------------------
int CCameraWar3::GetField84() const
{
    return *(int*)((char*)this + 0x84);
}

//  dword_6FAB542C - g_pLastCameraWar3 (funcmap.py DATA; also declared in
//  GameUI/cameraconstructor.cpp, which writes it directly rather than
//  through this setter).
extern void* g_pLastCameraWar3;

//  dbl_6F932C60 - GameUI/cameratickseconds.cpp's own scale; not otherwise
//  named in this codebase yet.
extern const double g_ticksToSeconds;   // dbl_6F932C60

extern const double g_pi;                  // dbl_6F876608
extern const double g_degreesPerHalfTurn;  // dbl_6F92EF60

//----------------------------------------------------------------------------
//  0x6F30A860 - `retn 0`, `__fastcall`.  Stamps the fastcall receiver
//  straight into dword_6FAB542C.  No caller in this batch's own reach
//  (0x6F308C60, which passes a small clamped index rather than a
//  CCameraWar3*, is out of this pass's scope) says whether that is really
//  a re-registration or a reuse of the same global for something else;
//  reconstructed to the letter of what it does, not what it is for.
//----------------------------------------------------------------------------
void __fastcall SetLastCameraWar3Raw(void* value)
{
    g_pLastCameraWar3 = value;
}

//----------------------------------------------------------------------------
//  0x6F6C4E80 - GetTickCount() as seconds, hardware float.  `retn 0`.
//  Casting the unsigned tick count to `double` is what produces the
//  shipped `test eax,eax`/`jge`/`fadd 4294967296.0` unsigned fix-up around
//  the `fild` on its own - see docs/msvc-vc8-idioms.md.
//----------------------------------------------------------------------------
float __fastcall GetTickCountSeconds()
{
    return (float)((double)GetTickCount() * g_ticksToSeconds);
}

//----------------------------------------------------------------------------
//  0x6F301B70 - degrees to radians, hardware float.  `retn 4`.
//----------------------------------------------------------------------------
float __fastcall DegreesToRadiansHW(float degrees)
{
    return (float)((double)degrees * g_pi / g_degreesPerHalfTurn);
}

//  flt_6F92F230 - g_cameraBlendRate (funcmap.py DATA).
extern const float g_cameraBlendRate;

//----------------------------------------------------------------------------
//  0x6F301E50 - pick the blend weight for one property write: full weight
//  when the caller says "immediate", the small per-frame step otherwise.
//  `retn 4`, no receiver at all (no ecx use) - a plain function, not a
//  member.  Address-neighbours DegreesToRadiansHW above and the other
//  0x6F301xxx leaves already in this file.
//
//  `retn 4` with the argument read off the stack (not a register) is
//  `__stdcall`, not the default `__cdecl` a bare declaration would give -
//  cdecl leaves the caller to clean the 4 bytes and this build emits
//  `ret 0` for it, which is the ABI mismatch thunk_abi_audit.py exists to
//  catch on the *callee* side of a thunk; here it is this reconstruction's
//  own signature, so a wrong convention would just show up as a score
//  miss, which is how this one was caught.
//
//  Both arms write the float result back through `&immediate` rather than
//  into a plain local: the shipped body reuses the incoming argument's own
//  stack slot for the `fstp`/`fld` round trip before the `ret`, which is
//  what assignment through the argument's own address compiles to - a
//  genuinely new local gets a slot of its own and the store/reload
//  disappears at this build's optimisation level.
//----------------------------------------------------------------------------
float __stdcall SelectCameraBlendRate(int immediate)
{
    float* slot = (float*)&immediate;
    if (immediate == 0)
        *slot = g_cameraBlendRate;
    else
        *slot = 1.0f;
    return *slot;
}
