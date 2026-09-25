//============================================================================
//  0x6F6D7590 - GetFileAttributesA, with a null path reported through
//  Storm's own last-error slot as ERROR_INVALID_PARAMETER and answered 0
//  (not INVALID_FILE_ATTRIBUTES - that difference is the shipped code's,
//  and OsFileIsDirectory below relies on it).
//============================================================================
#include "os.h"

DWORD __fastcall OsGetFileAttributes(const char* path)
{
    if (path == 0)
    {
        SErrSetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }

    return GetFileAttributesA(path);
}
