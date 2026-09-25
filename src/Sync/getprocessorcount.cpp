//============================================================================
//  0x6F6CDCB0 - GetSystemInfo()->dwNumberOfProcessors, defaulted to 1.  The
//  dump zeroes nine consecutive dwords covering the whole 0x24-byte
//  SYSTEM_INFO before the call - the compiler's unrolled form of a
//  zero-initialiser, not nine hand-written stores.
//============================================================================
#include "evtsched.h"
#include <string.h>

unsigned int GetProcessorCount()
{
    SYSTEM_INFO si;
    memset(&si, 0, sizeof(si));
    GetSystemInfo(&si);
    return si.dwNumberOfProcessors ? si.dwNumberOfProcessors : 1;
}
