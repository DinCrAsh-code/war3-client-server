//============================================================================
//  0x6F6D7600 - DeleteFileA, same null-path shape as OsGetFileAttributes.
//============================================================================
#include "os.h"

BOOL __fastcall OsDeleteFile(const char* path)
{
    if (path == 0)
    {
        SErrSetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }

    return DeleteFileA(path);
}
