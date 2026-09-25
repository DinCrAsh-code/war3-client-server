//============================================================================
//  0x6F26D6F0 - reset both of CUnit's own activity timestamps (the
//  +0x90/+0x94 reserved dwords, unnamed until now) to "two minutes ago":
//  GetTickCount() - 0x1D4C0 (120000ms).  Run once at the very end of
//  CUnit::Load (unit_load.cpp) so a just-loaded unit starts believing
//  whatever it tracks there is already stale.
//
//  `this` in ecx, no stack arguments, no return value used.
//============================================================================
#include "unit.h"

extern "C" unsigned int __stdcall GetTickCount();

void CUnit::ResetActivityTimestamps()
{
    unsigned int now = GetTickCount() - 0x1D4C0;
    *(unsigned int*)((char*)this + 0x90) = now;
    *(unsigned int*)((char*)this + 0x94) = now;
}
