//============================================================================
//  0x6F4B20E0 - cunit_agent7 batch (CUnit closure BFS depth 1-2), reached
//  from CUnit slot 110's own closure (unit_movementflags.cpp's
//  neighbourhood, funcmap.py's own note next to _CIasin/_CIatan2).
//
//  Decomposes a 9-float orientation basis (three FVec3-shaped triples at
//  +0x00/+0x0C/+0x14, real hardware `float`, not the software CFloat) into
//  three output angles via a top/bottom gimbal-lock branch and a normal
//  branch, `retn 0Ch` (three float* out params).  `this` in ecx; nothing
//  in this closure names the containing type, so the receiver stays a raw
//  offset-cast struct the way unit_activitytimestamps.cpp's own +0x90/+0x94
//  pair does.  `atan2`/`asin` compiled as `#pragma intrinsic` - same idiom
//  GameUI/gameuivectorlength.cpp already uses for `sqrt` to reach `_CIsqrt`
//  - is what reaches `_CIatan2`/`_CIasin` here.
//
//  flt_6F92F230/flt_6F953D20 are two plain numeric constants this function
//  happens to share addresses with unrelated globals elsewhere
//  (g_cameraBlendRate/an unnamed one) - the same "shared address, unrelated
//  read" shape SelectUnitOrBiasConstant already has for flt_6F92F230
//  (tools/funcmap.py's own BEHAVIOUR note), so they are declared here by
//  their raw address rather than borrowing a camera-flavoured name.
//============================================================================
extern const float g_cameraBlendRate;   // flt_6F92F230 (GameUI/cameramiscleaves.cpp)
extern float g_unk6F953D20;             // flt_6F953D20 - this function's own use

extern "C" double atan2(double y, double x);
#pragma intrinsic(atan2)
extern "C" double asin(double x);
#pragma intrinsic(asin)

struct SOrientationBasis_6F4B20E0
{
    float m[9];
    void DecomposeOrientationBasis(float* outA, float* outB, float* outC);
};

void SOrientationBasis_6F4B20E0::DecomposeOrientationBasis(float* outA,
                                                             float* outB,
                                                             float* outC)
{
    float* basis = m;
    float z = basis[2];   // +0x08

    if (!(1.0f <= z))
    {
        if (!(z <= -1.0f))
        {
            //  Normal case.
            *outA = (float)atan2((double)basis[1], (double)basis[0]);
            *outB = (float)asin((double)-basis[2]);
            *outC = (float)atan2((double)basis[5], (double)basis[8]);
            return;
        }

        //  Bottom gimbal lock (z <= -1).
        *outA = (float)atan2((double)basis[3], (double)basis[6]);
        *outB = g_unk6F953D20;
        *outC = 0.0f;
        return;
    }

    //  Top gimbal lock (z >= 1).
    *outA = (float)atan2((double)-basis[3], (double)-basis[6]);
    *outB = g_cameraBlendRate;
    *outC = 0.0f;
}
