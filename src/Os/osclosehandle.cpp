//============================================================================
//  0x6F6D73D0 - a bare CloseHandle, no null check and no result.
//============================================================================
#include "os.h"

void __fastcall OsCloseHandle(HANDLE h)
{
    CloseHandle(h);
}
