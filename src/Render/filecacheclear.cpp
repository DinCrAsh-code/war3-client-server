//============================================================================
//  0x6F4BEC20 - drop every cached/prefetched file and reset the module's own
//  byte/count counters, under the file cache's own lock (see filecache.h).
//  Tail-calls LeaveCritSec on the way out, matching the dump's own
//  `jmp LeaveCritSec` (no epilogue after it - the callee's own `retn`
//  becomes this function's).
//============================================================================
#include "filecache.h"

//  0x6F4BDDB0 - TAllocatorHashTable<PrefetchNode,HASHKEY_STRI,128>::UnlinkAll
//  (or its equivalent "drop every node" walk) over g_prefetchTable - outside
//  this batch's own 43; declared and thunked to its own real address so the
//  call site matches the dump.  The one stack argument (0) this call tree's
//  single call site pushes is never observed to be anything else.
//  `__fastcall` would put it in edx instead of on the stack - `retn 4`, not
//  `retn 0`, so it is a method on a minimal opaque receiver instead.
struct PrefetchTableClearer
{
    void UnlinkAll(int);
};

void ClearPrefetchTable(CPrefetchTable* table, int arg)
{
    ((PrefetchTableClearer*)table)->UnlinkAll(arg);
}

__declspec(naked) void PrefetchTableClearer::UnlinkAll(int)
{
    __asm { mov eax, 06F4BDDB0h }
    __asm { jmp eax }
}

void ResetFileCache()
{
    EnterCritSec(&g_fileCacheLock);

    ClearPrefetchTable(&g_prefetchTable, 0);
    g_prefetchCount = 0;
    g_prefetchBytes = 0;

    LeaveCritSec(&g_fileCacheLock);
}
