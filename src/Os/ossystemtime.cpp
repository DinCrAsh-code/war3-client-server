//============================================================================
//  Two adjacent, unrelated-but-neighbouring Win32 system-time wrappers.
//  Grouped in one TU per CLAUDE.md's address-neighbourhood rule (neither
//  calls the other, and 0x6F6C4F40/0x6F6C5060 are three dwords apart).
//============================================================================
#include <windows.h>

//----------------------------------------------------------------------------
//  0x6F6C4F40 - `push ecx / call GetSystemTime / retn`.
//----------------------------------------------------------------------------
void __fastcall OsGetSystemTime(LPSYSTEMTIME lpSystemTime)
{
    GetSystemTime(lpSystemTime);
}

//----------------------------------------------------------------------------
//  0x6F6C5060 - SystemTimeToFileTime, guarded: a null system-time pointer
//  *or* a null file-time pointer raises Storm's own argument-validation
//  assert (SErrSetLastError(0x57) = ERROR_INVALID_PARAMETER) instead of
//  calling through with one.
//----------------------------------------------------------------------------
void __stdcall SErrSetLastError(unsigned int code);

void __fastcall OsSystemTimeToFileTime(LPSYSTEMTIME lpSystemTime, LPFILETIME lpFileTime)
{
    if (lpFileTime == 0)
    {
        SErrSetLastError(0x57);
        return;
    }
    if (lpSystemTime == 0)
    {
        SErrSetLastError(0x57);
        return;
    }
    SystemTimeToFileTime(lpSystemTime, lpFileTime);
}
