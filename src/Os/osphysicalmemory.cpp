//============================================================================
//  0x6F6CE070 - OsGetTotalPhysicalMemory (gamemain-batch-3).  A thin
//  GlobalMemoryStatus() forward that hands back only dwTotalPhys.
//============================================================================
#include <windows.h>

unsigned int OsGetTotalPhysicalMemory()
{
    MEMORYSTATUS status;
    GlobalMemoryStatus(&status);
    return status.dwTotalPhys;
}
