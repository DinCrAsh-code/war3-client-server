//============================================================================
//  0x6F6D7620 - CreateDirectoryA for `path`, optionally creating every
//  missing parent first.
//
//  The parent walk works on a private copy: each '\' found past the root end
//  is punched to NUL, the prefix so far is created, and the '\' is put back
//  before the search moves on.  The final CreateDirectoryA is outside the
//  `wholeTree` test - it runs either way, and its result is this function's
//  own return value.
//============================================================================
#include "os.h"

BOOL __fastcall OsCreateDirectoryTree(const char* path, int wholeTree)
{
    char work[MAX_PATH];

    if (path == 0)
    {
        SErrSetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }

    if (wholeTree != 0)
    {
        Storm_501(work, path, MAX_PATH);
        OsPathStripTrailingName(work);

        char* sep = Storm_569(work + OsPathRootLength(work), '\\');
        while (sep != 0)
        {
            *sep = 0;
            CreateDirectoryA(work, 0);
            *sep = '\\';
            sep = Storm_569(sep + 1, '\\');
        }
    }

    return CreateDirectoryA(path, 0);
}
