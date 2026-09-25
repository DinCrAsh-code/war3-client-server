//============================================================================
//  0x6F6C5B20 - a path with no root of its own is relative.
//
//  The whole body is `OsPathRootLength(path) == 0`; the shipped
//  `neg`/`sbb`/`add 1` is what MSVC emits for exactly that comparison
//  returned as an int, and `!x` here would be the same three instructions.
//============================================================================
#include "os.h"

int __fastcall OsPathIsRelative(const char* path)
{
    return OsPathRootLength(path) == 0;
}
