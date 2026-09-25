//============================================================================
//  0x6F6C5A00 - given a path, copy into `dest` (at most `size` bytes,
//  Storm_501-truncated, always NUL-terminated) whatever comes after the
//  *last* path separator of either kind - or the whole path if it has
//  neither.
//
//  Storm_572 is called twice, once per separator character, and the two
//  results (each "pointer to the last occurrence, or null") are combined by
//  a raw pointer comparison rather than a second scan: whichever call found
//  its separator further into the string has the larger address, so
//  `sep = (p1 <= p2) ? p2 : p1` picks "the last separator overall" without
//  caring which character it was.  Both null (neither found) still compares
//  true and falls through to the "whole path" arm below, which is why the
//  0-vs-0 case does not need its own test.
//
//  Reached from CNetData::LoadReplayHeader (netdata_loadreplayheader.cpp)
//  building a MAX_PATH-sized companion path for the replay file's own
//  first-open decompression pass.
//============================================================================
#include "os.h"    // Storm_572, Storm_501

const char* __fastcall Storm_572(const char* s, char ch);

void __fastcall ExtractPathTail(const char* src, char* dest, unsigned int size)
{
    *dest = 0;

    const char* lastBackslash = Storm_572(src, '\\');
    const char* lastSlash = Storm_572(src, '/');
    const char* sep = (lastBackslash <= lastSlash) ? lastSlash : lastBackslash;

    if (sep != 0)
        Storm_501(dest, sep + 1, size);
    else
        Storm_501(dest, src, size);
}
