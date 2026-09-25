//============================================================================
//  CDataRecycler - the pool the CDataStore family checks its chunk buffers
//  back into.  The name is stamped: both constructors write
//  `??_7CDataRecycler@@6B@`, and the element the pool hands out carries the
//  RTTI descriptor ".?AUCHUNK@CDataStore..." in every free.
//
//  Only the five words the two constructors write are named; the rest of the
//  class (the two lock-free stacks and the chunk book-keeping past +0x18) is
//  reached by the functions this session did not reconstruct.
//============================================================================
#ifndef CDATARECYCLER_H
#define CDATARECYCLER_H

//: ??_7CDataRecycler@@6B@ - stamped, never dispatched through from this
//: build.  Local, for the reason src/Game/savegameglobals.cpp gives.
extern void* g_vftableCDataRecycler[1];

class CDataRecycler
{
public:
    //  0x6F4C6610 - retn 0, and *not* a constructor: it stamps the vtable
    //  and returns nothing at all, where MSVC's constructors hand `this`
    //  back in eax.  Two instructions, immediately after the constructor in
    //  the image, which is where a class with a vtable and no members to
    //  release puts its destructor.
    ~CDataRecycler();
    //  0x6F4C65C0 - retn 8.  Both bounds are clamped to at least one, and
    //  the live count is clamped to the limit.
    CDataRecycler(unsigned int count, int limit);

    //  0x6F65A970 - vtable slot 0 (agent-networking-wave3-2026-08-29). Same
    //  "scalar deleting destructor" shape as every other DeleteSelf this
    //  session reconstructed elsewhere (netprovider_deleteself.cpp and
    //  friends) - calls ~CDataRecycler() (already reconstructed above,
    //  directly - no naked base-dtor thunk needed since that body is
    //  already real C++), then frees `this` when bit 0 of flags is set.
    void* DeleteSelf(unsigned int flags);

    //  0x6F4C6820 - vtable slot 1 (agent-networking-wave4-2026-08-29;
    //  wave 3's own vtable dump already carried this address at
    //  method_index 1, between DeleteSelf and Alloc, but nothing had
    //  reconstructed it yet). Drains both lock-free stacks this class
    //  carries past its five named words: every node on the `m_field10`
    //  stack is handed back through this object's OWN vtable slot 4
    //  (offset 0x10, i.e. `Free`, dispatched indirectly rather than called
    //  directly - see cdatarecycler_slots.cpp's own note on why); every
    //  node on the `m_field14` stack is freed for real with a logged
    //  `SMemFree(ptr, "delete", -1, 0)` (`Storm_403`, matching `DeleteSelf`'s
    //  own literal). Named for what it does to the pool, not for a known
    //  caller - nothing in this call tree's own closure invokes it.
    void Purge();

    //  0x6F4C6890 - pop one node off the `m_field10` chunk stack and hand
    //  its two payload words back through the two out-parameters (`*outPtr
    //  = node->m_field4`+`*outSize = node->m_field8`, both zeroed on an
    //  empty stack), then return the popped node itself onto the
    //  `m_field14` free-node stack for reuse. The trailing two stack
    //  parameters are real (retn 0x10, four stack words) but never read on
    //  the one path this call tree exercises - the one call site
    //  (AcquireChunk, below) always passes 0/0.  Not `Alloc` (0x6F657520,
    //  the logged `SMemAlloc` wrapper, a different address) - this is the
    //  pool's own "reuse an already-checked-in chunk if one exists"
    //  fast path, called by AcquireChunk before it ever considers growing.
    void TryPopChunk(void** outPtr, unsigned int* outSize, int unused1, int unused2);

    //  0x6F67C540 - NetClient's own receive-chunk acquire helper: try
    //  TryPopChunk first; if the popped chunk's own recorded size is
    //  already >= 0x418 bytes, reuse it as-is, otherwise grow it to
    //  exactly 0x418 through this object's own vtable slot 3 (`Realloc`,
    //  dispatched indirectly for the same reason Purge dispatches Free
    //  indirectly - this call site genuinely goes through the vtable in
    //  the dump, not a direct call). Zeroes the first two header words of
    //  whatever chunk it hands back (the caller then fills in a timestamp
    //  and a size at +8/+0xC itself - see netclient_recvappend.cpp).
    //  Called with `ecx` pointed at this sub-object directly rather than
    //  through the vtable (`retn` with no operand - a fastcall taking only
    //  the one register argument, no stack cleanup needed); modelled as a
    //  free function taking a `CDataRecycler*` (netclient_chunkpool.cpp)
    //  rather than a genuine member purely to keep it out of this header,
    //  which otherwise only carries what dispatches through the class's
    //  own committed 5-slot vtable.

    //  0x6F657520 - vtable slot 2 (corrected from "slot 1" - wave 4's own
    //  `ida_query vtable` re-check of 0x6F9710A4 shows Purge, above, sits
    //  at method_index 1, ahead of this one). Logged SMemAlloc wrapper:
    //  `amount == 0` answers null without allocating; `fname == 0`
    //  substitutes a fixed fallback source location (0xF7). Echoes
    //  `amount` itself back through `outAmount` (not the allocated
    //  pointer) on every path, `amount == 0` included.
    void* Alloc(unsigned int amount, unsigned int* outAmount, const char* fname, int line);

    //  0x6F657570 - vtable slot 3 (corrected the same way as Alloc, above).
    //  Logged SMemReAlloc wrapper: `fname == 0` substitutes a fixed
    //  fallback source location (0x119). Same echo-`amount`-back-through-
    //  `outAmount` shape as Alloc. AcquireChunk (above) reaches this one
    //  through the vtable directly, not by name - see its own comment.
    void* Realloc(void* ptr, unsigned int amount, unsigned int* outAmount, const char* fname, int line);

    //  0x6F6575B0 - vtable slot 4 (corrected the same way as Alloc/Realloc,
    //  above). Logged SMemFree wrapper: `ptr == 0` is a silent no-op (no
    //  call at all); `fname == 0` substitutes a fixed fallback source
    //  location (0x108). Purge (above) reaches this one through the
    //  vtable directly, not by name - see its own comment.
    void Free(void* ptr, const char* fname, int line);

    //  0x6F4C68F0 - NetClient's own receive-chunk release helper
    //  (agent-networking-wave4-2026-08-29, netclient_chunkpool.cpp): the
    //  other half of AcquireChunk. Atomically decrement m_limit (used here
    //  as a live outstanding-chunk budget, not literally "the recycler's
    //  size limit" - the same word doing double duty, unrenamed since
    //  cdatarecyclerctor.cpp already established the name against the
    //  constructor's own two arguments); if that would go negative
    //  (budget exhausted), undo the decrement and hand the chunk straight
    //  back through vtable slot 4 (`Free`) instead of pooling it. If
    //  budget remains, thread the chunk onto the `m_field10` stack for
    //  AcquireChunk to reuse - allocating a fresh link node from the
    //  `m_field14` free-node pool first (SetupNodeChain, batch-refilling
    //  that pool from one `Storm_401`/SMemAlloc block when it runs dry).
    //  `retn 0x10` (four stack words - a pointer, a size, and two more the
    //  one real call site always passes as 0/0, same shape as
    //  TryPopChunk's own trailing pair).
    void ReleaseChunk(void* ptr, unsigned int size, const char* fname, int line);

    //  0x6F4C6740 - refill the free-link-node stack from one fresh
    //  SMemAlloc'd block; ReleaseChunk's own callee when that pool runs
    //  dry.  See its own definition (cdatarecycler_slots.cpp) for the
    //  bulk-chain-then-splice shape.
    void SetupNodeChain(void* block, void** poolHead);

    //  0x6F67D630 - NetClient's own receive-chunk release helper
    //  (netclient_chunkpool.cpp): unlink `node` from whatever intrusive
    //  list it is on, then hand it to ReleaseChunk with a fixed 0x418-byte
    //  size (the same constant AcquireChunk grows to).
    void ReleaseNode(void* node);

    //  0x6F67D680 - NetClient vtable slot 17's own release helper
    //  (networking wave 9, netclient_slot17_send.cpp): same shape as
    //  ReleaseNode above, just for the record's SECOND, differently-sized
    //  chunk pool (embedded at `+0x214`, 0x74-byte chunks rather than
    //  +0x1E8's 0x418-byte ones) - a distinct `TSLink<T>` instantiation and
    //  a distinct fixed size/tag pair, otherwise identical.
    void ReleaseNode74(void* node);

    void*        m_vtable;      // +0x00
    int          m_limit;       // +0x04 - doubles as ReleaseChunk's live budget counter
    unsigned int m_count;       // +0x08
    int          m_field0C;     // +0x0C
    void*        m_field10;     // +0x10 - chunk-stack head (AcquireChunk/ReleaseChunk)
    void*        m_field14;     // +0x14 - free-link-node-stack head (TryPopChunk/ReleaseChunk)
};

#endif
