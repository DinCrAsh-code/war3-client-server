//============================================================================
//  0x6F00A8B0 - GetConfigColor: four consecutive config ints packed into one
//  colour dword, big-endian by index (`(c0<<24)|(c1<<16)|(c2<<8)|c3`).
//
//  The index really is scaled by four before the four reads (`add edi,edi`
//  twice), so a colour "index" names a group of four consecutive entries
//  under the same key rather than a single one.
//
//  Own translation unit: four real calls to GetConfigInt (configint.cpp).
//============================================================================
#include "packedcolor.h"

//  0x6F009F90 - configint.cpp.
int __fastcall GetConfigInt(const char* section, const char* key, int index);

unsigned int* __fastcall GetConfigColor(unsigned int* out, const char* section,
                                        const char* key, int index)
{
    int base = index * 4;
    int c0 = GetConfigInt(section, key, base);
    int c1 = GetConfigInt(section, key, base + 1);
    int c2 = GetConfigInt(section, key, base + 2);
    int c3 = GetConfigInt(section, key, base + 3);

    //  Each component truncated to a byte on the way in - the shipped code
    //  reads all four back out of their stack slots with `movzx`/`mov ah`
    //  byte loads rather than masking whole dwords.
    *out = ((unsigned int)(unsigned char)c0 << 24) |
           ((unsigned int)(unsigned char)c1 << 16) |
           ((unsigned int)(unsigned char)c2 << 8) |
           (unsigned int)(unsigned char)c3;
    return out;
}
