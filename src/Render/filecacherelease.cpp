//============================================================================
//  0x6F4BC6B0 - hand a loaded file's bytes back.
//
//  Two owners are possible and the caller says which: a buffer the cache
//  copied out of a node came from the heap and goes back to Storm's
//  allocator, while one Storm's own loader produced goes back to Storm's own
//  loader.  The cache being switched off entirely (g_fileCacheEnabled == 0)
//  forces the second, because then nothing was ever copied.
//============================================================================
#include "filecache.h"

static const char kFileCacheFile[] = ".\\FileCache.cpp";

int __fastcall ReleaseLoadedFile(void* buffer, int fromLoader)
{
    if (g_fileCacheEnabled != 0 && fromLoader == 0)
    {
        SMemFree(buffer, kFileCacheFile, 0x2AA, 0);
        return 1;
    }

    return Storm_280(buffer);
}
