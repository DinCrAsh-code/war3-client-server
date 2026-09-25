//============================================================================
//  0x6F72B0B0 - GetDefaultConfigColor: four consecutive GetDefaultConfigInt
//  lookups (baseIndex, +1, +2, +3), each truncated to a byte and packed
//  big-endian into one dword (`(r<<24)|(g<<16)|(b<<8)|a`) - the same
//  "read N consecutive indexed values, pack/store" shape
//  GetDefaultConfigVec3 (gamedefaultconfigvec3.cpp) already uses for three
//  floats, here for four bytes instead.  Reached (like the whole
//  GetDefaultConfig* family) only through Storm's own singleton bootstrap,
//  not through any item-positioning code - see
//  docs/notes/jass-setitemposition-storm-gates.md.
//============================================================================
#include "gamedefaultconfig.h"

int* __fastcall GetDefaultConfigColor(int* out, const char* key,
                                       const char* section,
                                       unsigned int baseIndex)
{
    unsigned char r = (unsigned char)GetDefaultConfigInt(key, section, baseIndex);
    unsigned char g = (unsigned char)GetDefaultConfigInt(key, section, baseIndex + 1);
    unsigned char b = (unsigned char)GetDefaultConfigInt(key, section, baseIndex + 2);
    unsigned char a = (unsigned char)GetDefaultConfigInt(key, section, baseIndex + 3);
    *out = (r << 24) | (g << 16) | (b << 8) | a;
    return out;
}
