//============================================================================
//  0x6F6D7DE0 - "is this an existing path that is not a directory".
//
//  The shipped test is `attributes != -1 && !(attributes & 0x10)`; an empty
//  string is rejected before the query, and a null one is rejected here
//  rather than being left to OsGetFileAttributes' own null arm.
//============================================================================
#include "os.h"

int __fastcall OsFileExists(const char* path)
{
    //  One nested chain with a single `return 0` at the end, not four early
    //  returns: the shipped stream shares one `xor eax,eax` / `retn` between
    //  all four rejections and reaches `mov eax,1` only by falling through.
    //  Written as early returns, MSVC if-converts the last test into
    //  `not`/`shr`/`and` and the branch disappears.
    if (path != 0 && path[0] != 0)
    {
        DWORD attributes = OsGetFileAttributes(path);

        if (attributes != INVALID_FILE_ATTRIBUTES &&
            !(attributes & FILE_ATTRIBUTE_DIRECTORY))
            return 1;
    }

    return 0;
}
