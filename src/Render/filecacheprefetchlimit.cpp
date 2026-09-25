//============================================================================
//  0x6F4BF470/0x6F4C3F20 - the file-cache module's (filecache.h) enable
//  switch and prefetch-size sizing, reached out of GameMain's own init
//  sequence (Game/gamemaininitsequence.cpp).
//============================================================================
#include "filecache.h"

//  0x6F4BEC20 is ResetFileCache (Render/filecacheclear.cpp, gamemain-batch-2's
//  own real reconstruction of this address, takes no arguments and tail-
//  called with none, matching the dump's own `jmp ResetFileCache`).  This
//  file used to carry a second, naked redirect to the same address under
//  the name NotifyFileCacheEnabledChanged_6F4BEC20 - a duplicate of the
//  real body *and* a wrong signature (declared `unsigned int` and called
//  with the enabled flag, when the shipped tail jump passes nothing at
//  all).  Fixed by calling the real reconstruction directly.
void ResetFileCache();   // filecacheclear.cpp, 0x6F4BEC20

//----------------------------------------------------------------------------
//  0x6F4BF470 - change-guarded update-and-notify: store the new enabled
//  flag only if it differs from the cached one, then forward it on.
//----------------------------------------------------------------------------
void __fastcall SetFileCacheEnabled(unsigned int enabled)
{
    if ((unsigned int)g_fileCacheEnabled != enabled)
    {
        g_fileCacheEnabled = (int)enabled;
        ResetFileCache();
    }
}

//  0x6F4C3470 - out of this session's scope (gamemain-batch-2 owns it);
//  return value unused here.  Thunked.
__declspec(naked) int FileCachePrepareSizing_6F4C3470()
{
    __asm { mov eax, 0x6F4C3470 }
    __asm { jmp eax }
}

//  0x6F6CE070 - out of this session's scope (gamemain-batch-3 owns it);
//  a byte count (physical memory size, per this call site's own `shr 14h`
//  turning it into megabytes) shifted right 0x14 places here, not inside
//  the callee. Thunked.
__declspec(naked) unsigned int FileCacheQueryMemoryBytes_6F6CE070()
{
    __asm { mov eax, 0x6F6CE070 }
    __asm { jmp eax }
}

//----------------------------------------------------------------------------
//  0x6F4C3F20 - size the prefetch limit off available memory: at least
//  80 MB is assumed, `2*MB - 160` is the raw limit, and the result is
//  clamped to [8, 512].  The store-then-conditionally-overwrite shape
//  (store the raw value first, then two more stores on the two out-of-range
//  arms) is the shipped instruction order, not a source-level clamp() call.
//----------------------------------------------------------------------------
void InitializeFileCachePrefetchLimit()
{
    FileCachePrepareSizing_6F4C3470();

    unsigned int megabytes = FileCacheQueryMemoryBytes_6F6CE070() >> 0x14;
    if (megabytes <= 0x50)
        megabytes = 0x50;

    int limit = (int)(megabytes + megabytes - 0xA0);
    g_prefetchLimit = limit;
    if (limit <= 8)
        goto small;
    if (limit < 0x200)
        return;
    g_prefetchLimit = 0x200;
    return;

small:
    g_prefetchLimit = 8;
}
