//============================================================================
//  0x6F6C59C0 - drop a trailing separator, then cut the last component off.
//
//  Both steps are skipped outright when the string is nothing but its own
//  root, which is what the `len > rootLength` guard says; the tail call into
//  OsPathStripFileName is inside that guard, not after it.
//============================================================================
#include "os.h"

void __fastcall OsPathStripTrailingName(char* path)
{
    unsigned int len = Storm_506(path);

    if (len <= (unsigned int)OsPathRootLength(path))
        return;

    char last = path[len - 1];
    if (last == '\\' || last == '/')
        path[len - 1] = 0;

    OsPathStripFileName(path);
}
