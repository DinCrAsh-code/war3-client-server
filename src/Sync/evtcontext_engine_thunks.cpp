//============================================================================
//  gamemain-batch-4's own thunk bucket: 29 functions from the GameMain
//  depth-5 dump left as naked `mov eax,<addr>` / `jmp eax` redirects into
//  the real, unhooked shipped code, rather than reconstructed - genuinely
//  deep, non-gameplay engine internals, the same class CLAUDE.md's own
//  examples (`ConstructStormSingletonA`, the Storm/Terrain gateway
//  frontier) already call out as not worth chasing:
//
//    * 0x6F685AD0/A90/CA0/C00 - a small bignum/checksum-table library
//      (byte-substitution table generation over a fixed 0x971AF0 table,
//      schoolbook multiply-accumulate rows, and two mod-exponentiation
//      style bit-extraction loops keyed by fixed constants 0x1D0/0x1Dh).
//      No call site in this dump reaches gameplay state through them.
//    * 0x6F4BDDB0 - a hash-table's full-release Clear(): a virtual free
//      call per surviving bucket entry through slot 0, and the same
//      TSLink<T>::Unlink() shape storm.h's own template already
//      reconstructs, over a class this dump never names.
//    * 0x6F62Fxxx/0x6F630xxx/0x6F632xxx - `EvtContext`
//      (`??_7EvtContext@@6B@`) and its "AuEvtThread"-tagged pool: an
//      SEH-wrapped, vtable-based thread/timer-priority scheduler
//      (TSTimerPriority<ulong> base, per-slot CRITICAL_SECTIONs, its own
//      wait/signal/dispatch loop). Several of its members were traced far
//      enough to be confident of the shape (a hash-table style bucket
//      array at dword_6FACEB5C/8/60, Event::Set/Wait through the members
//      already reconstructed in Sync/event.cpp, the same TSLink unlink
//      idiom as 0x6F4BDDB0) but the constructor (0x6F62F610, 221
//      instructions, one `??_L@YGXPAXIHP6EX0@Z1@Z` eh-vector-constructor-
//      iterator call, two base vtables stamped) and the worker loop
//      (0x6F632610/0x6F62FC50/0x6F630000, all 150+ instructions) were not
//      worth guessing a class layout for in this session's remaining
//      budget - a wrong slot signature here is exactly the FATAL-class
//      crash `vtable_dispatch_audit.py`/`thunk_abi_audit.py` exist to
//      catch, and this session had no way to run either against a real
//      committed vtable for a class it never finished modelling.
//    * 0x6F6E7870/78C0/8060/81F0/8420 - `.\W32\OsCall.cpp`'s own
//      thread-registration bookkeeping (a second, module-private TLS
//      index and refcounted per-thread node list, `TlsAlloc`/
//      `GetCurrentThreadId`/`TlsSetValue` all reached only from inside
//      this cluster) - same "deep, non-interesting chunk" call as the
//      EvtContext cluster, and the reason `TlsAlloc`/`GetCurrentThreadId`
//      themselves have no reconstruction of their own this session: no
//      *reconstructed* code in this batch ever calls them, so leaving
//      them thunked here rather than exercised is honest about that.
//    * 0x6F526990/6F52C940/6F526E50 - the render-device singleton's own
//      vtable-slot tail dispatch and a Win32 window-class registration
//      routine (`GetModuleHandleA` + three unowned callees). The first is
//      a bare `jmp` through `g_renderDevice`'s own vtable+0x10 with no
//      committed vtable entry to check the ABI against
//      (`docs/targets/vtables/` has nothing for this class); the other
//      two are straightforward but call three siblings none of which are
//      in this session's batch or already reconstructed.
//
//  Every address here has a real ### heading in
//  asm/GameMain_0x6F009850_calltree_asm.md, so `--fail-on-ready` will not
//  flag any of these as a dump-less guess.
//============================================================================

//  Plain C++ linkage, not `extern "C"`: a decorated (mangled) symbol is
//  what makes MASM print the `; readable-name` comment PUBLIC/PROC lines
//  carry, which is the only thing `tools/extract_gen.py` reads a function's
//  name from (docs/msvc-vc8-idioms.md, "Declaring imports so the diff can
//  see them" - the same rule, applied to a definition instead of a call).
//  An `extern "C"` version of this macro compiled clean and linked fine but
//  left every one of these 30 thunks in verify.py's "mapped but not found
//  in build/*.asm" bucket, because a plain C symbol carries no such comment.
#define RAW_THUNK(name, addr)                     \
    __declspec(naked) void name()                 \
    {                                              \
        __asm { mov eax, addr }                   \
        __asm { jmp eax }                         \
    }

//  The bignum/checksum-table library.
RAW_THUNK(ChecksumTableTransform,   0x6F685AD0)
RAW_THUNK(BignumMultiplyAddRow,     0x6F685A90)
RAW_THUNK(BignumModExpStep,         0x6F685CA0)
RAW_THUNK(BignumReduceMod,          0x6F685C00)

//  The hash-table Clear().
RAW_THUNK(HashTable_ReleaseAll,     0x6F4BDDB0)

//  EvtContext and its AuEvtThread pool.
RAW_THUNK(EvtThreadPool_Acquire,    0x6F62FAE0)
RAW_THUNK(EvtContext_Construct,     0x6F62F610)
RAW_THUNK(EvtThreadPool_Reap,       0x6F62FE20)
RAW_THUNK(EvtThreadPool_ReleaseSlot, 0x6F62F4E0)
RAW_THUNK(EvtThreadPool_ElementAt,  0x6F62CF40)
RAW_THUNK(EvtContext_MarkFlagBit0,  0x6F62D6E0)
RAW_THUNK(EvtContext_Wait,          0x6F632E70)
RAW_THUNK(EvtThreadPool_SignalAll,  0x6F62CF80)
RAW_THUNK(EvtContext_SignalStop,    0x6F62D790)
RAW_THUNK(EvtContext_Run,           0x6F632610)
RAW_THUNK(EvtContext_ClearFlagBit2, 0x6F62D7D0)
RAW_THUNK(EvtThreadPool_UpdateStats, 0x6F62FF40)
RAW_THUNK(EvtContext_Stop,          0x6F62EDA0)
RAW_THUNK(EvtContext_SignalOrFallback, 0x6F632C30)
RAW_THUNK(EvtContext_Dispatch,      0x6F630000)
RAW_THUNK(EvtContext_ProcessQueue,  0x6F62FC50)

//  OsCall.cpp's own thread-registration bookkeeping.
RAW_THUNK(OsCallNode_Attach,        0x6F6E7870)
RAW_THUNK(OsCallNode_Detach,        0x6F6E78C0)
RAW_THUNK(OsCallNode_Release,       0x6F6E8060)
RAW_THUNK(OsCallContext_RegisterThread, 0x6F6E81F0)
RAW_THUNK(OsCallContext_Init,       0x6F6E8420)

//  The IME-disable window helper (a new Win32 import - imm32.dll - this
//  session did not want to add to link_check.py's SDK_IMPORT_LIBS and
//  both injection repos' own build_mix.py link lines without a live gate
//  to prove the addition against), the render-device vtable tail dispatch
//  (no committed vtable to check the ABI against) and the window-class
//  registration routine (three unowned callees).
RAW_THUNK(DisableWindowIme,         0x6F6C6A60)
RAW_THUNK(RenderDevice_Slot4Dispatch, 0x6F526990)
RAW_THUNK(GameWindow_RegisterClass, 0x6F52C940)
RAW_THUNK(RenderDevice_WaitReady,   0x6F526E50)
