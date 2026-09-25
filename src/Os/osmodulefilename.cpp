//============================================================================
//  0x6F6C5860 - GetModuleFileNameA for the process image, no null checks.
//============================================================================
#include "os.h"

DWORD __fastcall OsGetModuleFileName(char* buffer, DWORD size)
{
    return GetModuleFileNameA(0, buffer, size);
}
