//============================================================================
//  0x6F302A70 - SetCameraPreferenceFractionSecondary.
//  0x6F302A30 - SetCameraPreferenceFractionPrimary.
//  0x6F3029F0 / 0x6F3029B0 - their own two callees (no prior name).
//
//  The record-builder's own small twin cluster CCameraWar3::CCameraWar3()
//  (cameraconstructor.cpp) calls once each, to fill its own +0x4FC/+0x500
//  reserved weight slots from preference ids 41/39 - the same two functions
//  CPreferencesWar3::ApplyPreference (Config/preferenceapply.cpp) already
//  calls live for those same two ids, and which were thunked (naked
//  redirects) in Misc/spritehandlethunks.cpp until this session.
//
//  Two matched pairs, exactly the "identical body, no /OPT:ICF, distinct
//  address" shape agenttypedslots.h's own eighteen-member family already
//  documents:
//    - SetCameraPreferenceFractionSecondary/Primary (0x6F302A70/0x6F302A30)
//      guard their float argument into [0, 1], add a small constant
//      (g_unkDblQueryCellBias) and forward it;
//    - the two callees (0x6F3029F0/0x6F3029B0) snap that value to K
//      (g_dblZero) itself when it is within tolerance T
//      (g_frameScaleEpsilon) of K, else force it to 1.0f, and store the
//      result into the receiver's own +0x500/+0x4FC - reserved
//      CCameraWar3 fields nothing in either tree reads back.
//  K and T are the already-named GameUI/camerapan.cpp / Config/preferenceapply.cpp
//  constants - matching them by name rather than by raw address is what
//  lets the generated listing canonicalise to the same symbol the target's
//  own does.
//
//  Named locals reused in place (rather than fresh temporaries per step)
//  reproduce the dump's own repeated store/reload through the same stack
//  slot at every stage (docs/msvc-vc8-idioms.md, "Named locals for every
//  intermediate, when the shipped code stores and reloads").
//============================================================================
#include "cameraweightclamp.h"
#include "gameui.h"

extern "C" double __cdecl fabs(double x);
#pragma intrinsic(fabs)

//  dbl_6F876620 - GameUI/camerapan.cpp.
extern const double g_dblZero;
//  flt_6F92E790 - Config/preferenceapply.cpp's own comment ("same
//  comparison against flt_6F92E790 (2^-22, read with ida_query)").
extern const float g_frameScaleEpsilon;
//  dbl_6F875120 - StormSingletonA/stormsingletonaquery.cpp.
extern const double g_unkDblQueryCellBias;

//----------------------------------------------------------------------------
//  Stand-in receiver purely to spell the __thiscall leaf pair at
//  0x6F3029F0/0x6F3029B0 - the same technique this codebase already uses
//  wherever a real `__thiscall` leaf's own class was never otherwise
//  reconstructed (grep `__thiscall` in GameUI/*.cpp for the pattern). The
//  real receiver at both addresses is a CCameraWar3*, cast through this
//  type only for the duration of the call.
//----------------------------------------------------------------------------
struct SCameraWeightStoreHost
{
    //  0x6F3029F0 - retn 4.
    void __thiscall StoreClampedWeight500(float value);
    //  0x6F3029B0 - retn 4.
    void __thiscall StoreClampedWeight4FC(float value);
};

void __thiscall SCameraWeightStoreHost::StoreClampedWeight500(float value)
{
    value = (float)((double)value - g_dblZero);
    value = (float)fabs((double)value);
    value = ((double)value > (double)g_frameScaleEpsilon) ? 1.0f : (float)g_dblZero;
    *(float*)((char*)this + 0x500) = value;
}

//  0x6F3029B0 - retn 4.  Same body as StoreClampedWeight500, distinct
//  address and destination offset only.
void __thiscall SCameraWeightStoreHost::StoreClampedWeight4FC(float value)
{
    value = (float)((double)value - g_dblZero);
    value = (float)fabs((double)value);
    value = ((double)value > (double)g_frameScaleEpsilon) ? 1.0f : (float)g_dblZero;
    *(float*)((char*)this + 0x4FC) = value;
}

//----------------------------------------------------------------------------
//  0x6F302A70 - retn 4.  Guards `value` into [0, 1] (skips the store below
//  entirely, doing nothing, outside that range), nudges it by
//  g_unkDblQueryCellBias and forwards it to StoreClampedWeight500.
//----------------------------------------------------------------------------
void __fastcall SetCameraPreferenceFractionSecondary(CCameraWar3* camera,
                                                      float value)
{
    if (0.0f > value)
        return;
    if (value > 1.0f)
        return;

    float adjusted = (float)((double)value + g_unkDblQueryCellBias);
    ((SCameraWeightStoreHost*)camera)->StoreClampedWeight500(adjusted);
}

//----------------------------------------------------------------------------
//  0x6F302A30 - retn 4.  Same body as SetCameraPreferenceFractionSecondary,
//  into StoreClampedWeight4FC instead.
//----------------------------------------------------------------------------
void __fastcall SetCameraPreferenceFractionPrimary(CCameraWar3* camera,
                                                    float value)
{
    if (0.0f > value)
        return;
    if (value > 1.0f)
        return;

    float adjusted = (float)((double)value + g_unkDblQueryCellBias);
    ((SCameraWeightStoreHost*)camera)->StoreClampedWeight4FC(adjusted);
}
