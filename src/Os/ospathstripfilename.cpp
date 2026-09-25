//============================================================================
//  0x6F6C5980 - cut the last path component off `path` in place.
//
//  The separator kept is the *later* of the last '\' and the last '/', and
//  the cut never eats into the root: a separator that lies before the root's
//  own end truncates at the root end instead, so "c:\file" becomes "c:\"
//  and not "c:".
//============================================================================
#include "os.h"

void __fastcall OsPathStripFileName(char* path)
{
    char* sep = Storm_570(path, '\\');
    char* slash = Storm_570(path, '/');

    if (sep <= slash)
        sep = slash;

    if (sep == 0)
        return;

    char* rootEnd = path + OsPathRootLength(path);

    if (sep < rootEnd)
        *rootEnd = 0;
    else
        sep[1] = 0;
}
