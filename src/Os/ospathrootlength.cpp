//============================================================================
//  0x6F6C57D0 - how many leading characters of `path` are its root: 1 for a
//  bare "/", 2 for "c:", 3 for "c:\", the whole "\\server\share\" for a UNC
//  path, 0 for anything relative.
//
//  The UNC arm is transcribed rather than tidied.  Two forward searches for
//  '\' are written as a counted loop with the null test *inside* it, because
//  that is the shape the shipped stream has (`test eax,eax` at the top of
//  each of the two iterations, `add eax,1` unconditionally after the search)
//  - and the `+1` on a search that found nothing is the shipped behaviour
//  too, not a transcription slip.
//============================================================================
#include "os.h"

int __fastcall OsPathRootLength(const char* path)
{
    unsigned int len = Storm_506(path);

    if (len > 0 && path[0] == '/')
        return 1;

    if (len < 2)
        return 0;

    //  Named, not compared twice: the shipped code loads path[1] once into
    //  al and tests it against ':' and then '\'.
    char second = path[1];

    if (second == ':')
        return (path[2] == '\\') + 2;

    if (path[0] != '\\' || second != '\\')
        return 0;

    const char* p = path + 2;
    int remaining = 2;
    do
    {
        if (p != 0)
            p = Storm_571(p, '\\') + 1;
    }
    while (--remaining);

    if (p == 0)
        return len;

    return (int)(p - path);
}
