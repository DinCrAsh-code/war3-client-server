//============================================================================
//  0x6F4C6560 - divide a float by a fixed double constant, in place on the
//  stack (store the quotient back to the argument slot, then reload it -
//  the round trip through memory forces single-precision rounding on the
//  way out, the same reason a plain C `(float)` cast on an FPU result costs
//  a real store/reload rather than folding away).  Nothing in this call
//  tree names the constant or what calls this; two addresses share it
//  (0x6F4C6560 and 0x6F4C64C0, the second not reconstructed here).
//============================================================================

extern double g_unkDbl6F93C218;   // dbl_6F93C218

float __fastcall DivideByConst_6F93C218(float value)
{
    float result = (float)(value / g_unkDbl6F93C218);
    return result;
}
