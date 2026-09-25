//============================================================================
//  0x6F6CDBF0 - GetCpuFeatureFlags: the CPU feature word, computed once and
//  cached in dword_6FAD1A90.
//
//  Five bits are read out of CPUID and one is a vendor check:
//    bit 0  - feature bit 4 of the standard feature word;
//    bit 1  - bit 23 (MMX);
//    bit 2  - bit 25 (SSE);
//    bit 3  - bit 31 of the extended word (3DNow!);
//    bit 4  - bit 26 (SSE2), *and only on a "GenuineIntel" part*;
//  and bit 31 is set unconditionally so that a zero cache means "not computed
//  yet" rather than "no features".
//
//  Not frame code; it is here because CSimpleTop's own module is what reaches
//  it in this call tree.  Its score is capped by the /GS stack cookie the
//  shipped function carries around its CPUID buffer, which this build's fixed
//  `/GS-` does not emit.
//============================================================================
#include "framethunks.h"

//  dword_6FAD1A90 / dword_6F974C30.
extern unsigned int g_cpuFeatureFlags;
extern const unsigned int g_cpuVendorIntel[3];

unsigned int GetCpuFeatureFlags()
{
    unsigned int cached = g_cpuFeatureFlags;
    if (cached)
        return cached;

    unsigned int vendor[3];
    unsigned int features;
    unsigned int extended;
    unsigned int flags = 0;

    if (CpuIdFeatures(vendor, &features, &extended))
    {
        if (features & 0x10)
            flags = 1;
        if (features & 0x800000)
            flags |= 2;
        if (features & 0x2000000)
            flags |= 4;
        if (extended & 0x80000000)
            flags |= 8;

        if (vendor[0] == g_cpuVendorIntel[0] &&
            vendor[1] == g_cpuVendorIntel[1] &&
            vendor[2] == g_cpuVendorIntel[2] &&
            (features & 0x4000000))
            flags |= 0x10;
    }

    flags |= 0x80000000;
    g_cpuFeatureFlags = flags;
    return flags;
}
