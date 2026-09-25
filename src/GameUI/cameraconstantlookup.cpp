//============================================================================
//  Three small "index with a clamp-to-index-0 fallback" constant tables in
//  the immediate neighbourhood of CCameraWar3::Method_0x20
//  (cameraregisterproperties.cpp, 0x6F301EE0), which is their only caller in
//  either dump.  Each is the same shape: load N compile-time floats onto the
//  FPU stack (into consecutive stack slots, highest address first - which is
//  why the source below lists them highest-index-first, matching the
//  fld/fstp order), then either index by the caller's own `ecx` (when it is
//  in range) or fall back to index 0.
//
//  sub_6F301920's own 18-entry table is the same three constants each
//  repeated six times running, which the source spells as a real 18-entry
//  array rather than three separate three-branch tests, because that is
//  what the shipped `fst` chain (five non-popping stores, one popping)
//  builds - a genuine array initialised in one FPU sweep, not three
//  independent scalars.
//============================================================================
#include "CFloat.h"

//  Index 0 is the same address ResetToDefaults already names.
extern const float g_cameraDefaultTargetDistance;   // flt_6F93645C
extern const float g_camPresetTargetDistance1;       // flt_6F936458
extern const float g_camPresetTargetDistance2;       // flt_6F936454
extern const float g_camPresetTargetDistance3;       // flt_6F936450
extern const float g_camPresetTargetDistance4;       // flt_6F93644C
extern const float g_camPresetTargetDistance5;       // flt_6F936448

extern const float g_camCursorModeRotation0;         // flt_6F936444
extern const float g_camCursorModeRotation1;         // flt_6F936440
extern const float g_camCursorModeRotation2;         // flt_6F93643C

extern const float g_camPresetAngleOfAttack0;        // flt_6F936438
extern const float g_camPresetAngleOfAttack1;        // flt_6F936434
extern const float g_camPresetAngleOfAttack2;        // flt_6F936430
extern const float g_camPresetAngleOfAttack3;        // flt_6F93642C
extern const float g_camPresetAngleOfAttack4;        // flt_6F936428
extern const float g_camPresetAngleOfAttack5;        // flt_6F936424

//  0x6F3019A0 - clamp `index` into [0,6) or use index 0.  The table is a
//  local (not `static const`): the shipped body builds it fresh on the
//  stack every call (a chained `fld`/`fstp` per entry), not a read out of
//  a preinitialised .rdata array.
float CameraLookupTargetDistancePreset(unsigned int index)
{
    float table[6] =
    {
        g_cameraDefaultTargetDistance, g_camPresetTargetDistance1,
        g_camPresetTargetDistance2,    g_camPresetTargetDistance3,
        g_camPresetTargetDistance4,    g_camPresetTargetDistance5,
    };
    if (index < 6)
        return table[index];
    return table[0];
}

//  0x6F301920 - clamp `index` into [0,18) or use index 0.  Three groups of
//  six identical entries - see the file header.
float CameraLookupRotationByCursorMode(unsigned int index)
{
    float table[18] =
    {
        g_camCursorModeRotation0, g_camCursorModeRotation0,
        g_camCursorModeRotation0, g_camCursorModeRotation0,
        g_camCursorModeRotation0, g_camCursorModeRotation0,
        g_camCursorModeRotation1, g_camCursorModeRotation1,
        g_camCursorModeRotation1, g_camCursorModeRotation1,
        g_camCursorModeRotation1, g_camCursorModeRotation1,
        g_camCursorModeRotation2, g_camCursorModeRotation2,
        g_camCursorModeRotation2, g_camCursorModeRotation2,
        g_camCursorModeRotation2, g_camCursorModeRotation2,
    };
    if (index < 18)
        return table[index];
    return table[0];
}

//  0x6F3018B0 - clamp `index` into [0,6) or use index 0.
float CameraLookupAngleOfAttackPreset(unsigned int index)
{
    float table[6] =
    {
        g_camPresetAngleOfAttack0, g_camPresetAngleOfAttack1,
        g_camPresetAngleOfAttack2, g_camPresetAngleOfAttack3,
        g_camPresetAngleOfAttack4, g_camPresetAngleOfAttack5,
    };
    if (index < 6)
        return table[index];
    return table[0];
}
