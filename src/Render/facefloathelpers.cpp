//============================================================================
//  Two small float helpers off FACECACHEENTRY's font-size path
//  (SetFaceCacheEntrySize/RecomputeFaceCacheEntryMetrics, thunked in
//  Render/facecacheentry_thunks.cpp).
//============================================================================
#include "text.h"
#include "stormsingletona.h"

//  dword_6FADF154 is g_unk6FADF154 (Misc/trivialaccessors_07.cpp), typed
//  `void*` there for a pointer-returning accessor; this closure reads the
//  same four bytes as a plain int (`fild dword_6FADF154`).  Redeclaring it
//  with a different type here would split one funcmap.DATA global into two
//  mangled names (tools/link_check.py's own "SPLIT DATA GLOBAL" check), so
//  it keeps the `void*` spelling and is reinterpreted through a pointer
//  cast instead.
extern void* g_unk6FADF154;

//  flt_6F92F230 is g_cameraBlendRate (GameUI/cameramiscleaves.cpp) - the
//  same shared float-literal-pool constant, read here for an unrelated
//  purpose.
extern const float g_cameraBlendRate;

//  0x6F7B8B10 - `retn 4`.  No `this`: a plain function taking one stack
//  float, picking between 1.0 and one shared float-literal constant by its
//  sign - the classic `fcomp`/`fnstsw`/`test ah,0x41` "operand <= 0.0"
//  test, entered with 0.0 already on the FPU stack.
float __stdcall SelectUnitOrBiasConstant(float value)
{
    return (0.0f <= value) ? 1.0f : g_cameraBlendRate;
}

//  0x6F7B8B70 - `retn 4`, __fastcall(ecx flag, float value).  When the flag
//  is nonzero, the value passes straight through unchanged.  Otherwise it
//  is scaled by g_unk6FADF154 (itself converted from a possibly-negative
//  int through the standard unsigned-int-to-double bias idiom), fed through
//  SelectUnitOrBiasConstant above, scaled again by
//  g_unkDblQueryCellBias (dbl_6F875120, stormsingletona.h) and added back
//  to the original value, then rounded to the nearest int and converted
//  back to float - i.e. "round `value` to the nearest multiple of
//  1/g_unk6FADF154, in float, unless the caller says not to."
float __fastcall ScaleFontMetric(int passThrough, float value)
{
    if (passThrough)
        return value;

    double scale = (double)*(int*)&g_unk6FADF154;
    double scaled = scale * value;
    float bias = SelectUnitOrBiasConstant((float)scaled);
    double rounded = bias * g_unkDblQueryCellBias + value;
    return (float)(int)rounded;
}
