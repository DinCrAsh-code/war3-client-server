//============================================================================
//  0x6F73A960 - reached from CItem's own vtable BFS (batch 8 of 8, see
//  docs/targets/CItem-callee-batch8.md).  Splits a 32-bit int (by pointer)
//  into a high/low pair of floats: the raw bit pattern is masked into two
//  disjoint 14-bit windows (one shifted down by 9 first), each re-tagged
//  with a fixed 2^16 exponent (0x47800000) so it reads as a plain float,
//  then narrowed to its true magnitude by subtracting the same double
//  constant from both - the classic "double-single" splitting trick for
//  extended-precision arithmetic on hardware with no true double.
//  `__fastcall(ecx=&value, stack=dest)`, `retn 4`.
//============================================================================

extern const double g_unkDblSplitBias;   // dbl_6F981980

void __fastcall SplitIntToFloatPair(const int* value, float* dest)
{
    int bits = *value;
    int hiBits = (bits & 0x7FC000) | 0x47800000;
    int loBits = (int)(((unsigned int)bits >> 9) & 0x7FC000) | 0x47800000;

    dest[0] = (float)((double)*(const float*)&hiBits - g_unkDblSplitBias);
    dest[1] = (float)((double)*(const float*)&loBits - g_unkDblSplitBias);
}
