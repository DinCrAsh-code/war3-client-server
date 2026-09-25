//============================================================================
//  gamemain-batch-3: fifteen functions whose real dump was read this
//  session but which pull in enough further un-reconstructed depth (a
//  thread-worker loop, a hand-rolled TSHashTable-family destructor with no
//  matching generic template written yet, SEH __try/__finally frames this
//  toolchain cannot reproduce - docs/msvc-vc8-idioms.md's own "An
//  `__except_handler4`-shaped frame..." section, config-file/Storm-ordinal
//  chains, or a 280-instruction spatial search) that reconstructing them
//  for real was judged out of this session's budget.  Each is a genuine
//  engine-internals leaf reached by GameMain's own closure, not gameplay
//  logic a player would notice missing - kept as naked redirects into the
//  real, unhooked shipped body per CLAUDE.md, with every argument count
//  read straight off its own `asm/` dump (`retn <n>` plus the register
//  arguments each entry's own comment names) so thunk_abi_audit.py has
//  something real to check.
//============================================================================

#define ADDR_THUNK(sig, addr)          \
    __declspec(naked) sig              \
    {                                  \
        __asm { mov eax, addr }        \
        __asm { jmp eax }              \
    }

//  0x6F526F10 - retn 0Ch.  ecx unread; edx and three stack dwords used.
//  Camera/display-mode plumbing (sub_6F526990/sub_6F525F40/sub_6F52C940/
//  sub_6F526E50/sub_6F6CE230, plus Storm ordinal 289) with no dump-backed
//  reconstruction of that chain in this session.
ADDR_THUNK(void __fastcall GameMain_sub_6F526F10(void*, unsigned int,
                                                  unsigned int, unsigned int,
                                                  unsigned int),
           0x6F526F10)

//  0x6F630200 - retn 4.  One stack dword, no register argument read.  The
//  engine's own worker-thread body (TLS, GetTickCount pacing, a dozen
//  further callees under 0x6F62Cxxx-0x6F632xxx with no dump in this
//  session's batch).
ADDR_THUNK(unsigned int __stdcall GameMain_sub_6F630200(unsigned int),
           0x6F630200)

//  0x6F631210 - retn 0, no arguments (a tail `call sub_6F6C6A80` /
//  `jmp ResetPumpAccumulators` pair; sub_6F6C6A80 itself calls
//  ImmAssociateContextEx, an IME API this session did not chase).
ADDR_THUNK(void __cdecl GameMain_sub_6F631210(), 0x6F631210)

//  0x6F009190 - retn 18h.  `this` in ecx, a second value in edx, six stack
//  dwords.  A ~280-instruction nearest-in-radius spatial search over a
//  fixed 4-slot cluster plus a variable-length overflow array - real
//  gameplay-shaped logic, but too large to drive to EXACT this session;
//  flagged for a follow-up pass rather than guessed at.
ADDR_THUNK(int __fastcall GameMain_sub_6F009190(void*, unsigned int,
                                                 unsigned int, unsigned int,
                                                 unsigned int, unsigned int,
                                                 unsigned int, unsigned int),
           0x6F009190)

//  0x6F003C90 / 0x6F003B90 - retn 4 each, `this` in ecx only (no edx), one
//  stack bool.  Both look like a generic TSHashTable<T,K>-family
//  "destroy every bucket" method (SetLinkOffset/UnlinkAll-shaped,
//  Containers/tshash.inl's own neighbourhood) reused across two different
//  table instantiations the same way preferencehashtable.cpp's
//  Rehash/Initialize/CheckGrow trio already is - but no generic body for
//  *this* particular member exists in Containers/ yet, and deriving +
//  verifying one was out of budget.
//
//  `this`-only-plus-stack is `__thiscall`'s own shape, which this
//  compiler rejects on a free function (docs/msvc-vc8-idioms.md, "`
//  __thiscall` on a free function: rejected by this compiler") - so each
//  is a one-method placeholder struct instead, the same fix that entry
//  documents.
struct GameMainHashBucketDestroyerB3
{
    void Method_6F003C90(unsigned int);
    void Method_6F003B90(unsigned int);
};
ADDR_THUNK(void GameMainHashBucketDestroyerB3::Method_6F003C90(unsigned int),
           0x6F003C90)
ADDR_THUNK(void GameMainHashBucketDestroyerB3::Method_6F003B90(unsigned int),
           0x6F003B90)

//  0x6F0053B0 - retn 0, `this` in ecx.  A real __try/__finally frame in the
//  dump's own unified-EH shape (a per-function SEH_<addr> trampoline tail-
//  jumping into __CxxFrameHandler3) - the exact frame
//  docs/msvc-vc8-idioms.md's "An `__except_handler4`-shaped frame this
//  toolchain cannot reproduce" section already explains this repo's
//  bundled `cl.exe` cannot emit under any `/GS`/`/EHsc` combination it
//  accepts, so a from-scratch attempt here would cost real time to land at
//  the same low, frame-explained score that section's own two examples
//  already do.
ADDR_THUNK(void __fastcall GameMain_sub_6F0053B0(void*), 0x6F0053B0)

//  0x6F4BEA40 - retn 0, no arguments.  A module-shutdown routine touching
//  five further globals through three more un-reconstructed callees
//  (sub_6F4BDDB0/sub_6F4BDEF0/sub_6F4BDFF0) plus two more
//  ReleaseAllBlocks-shaped calls on other CDataAllocator instances.
ADDR_THUNK(void __cdecl GameMain_sub_6F4BEA40(), 0x6F4BEA40)

//  0x6F525F90 - retn 0.  A cursor-clip helper (ClipCursor/GetWindowRect/
//  GetCursorPos/SetCursorPos) gated behind two further un-reconstructed
//  callees (sub_6F525F00, sub_6F525F40) that pick the target window.
ADDR_THUNK(void __fastcall GameMain_sub_6F525F90(unsigned int), 0x6F525F90)

//  0x6F004330 - retn 8, `this` in ecx only (no edx), two stack dwords (the
//  second unread, matching this codebase's other unused-tail-argument
//  shapes).  An AGILE_TYPE_DATA-shaped masked-bucket lookup
//  (docs/msvc-vc8-idioms.md, "A table indexed by byte offset, not by
//  element" neighbourhood) this session did not trace to a concrete
//  instantiation.  Same __thiscall-on-a-free-function trap as
//  GameMainHashBucketDestroyerB3 above, same placeholder-struct fix.
//
//  0x6F005500 - retn 0Ch, `this` in ecx only, three stack dwords.  A
//  TSHashTable-family lookup/insert-shaped method over the same table
//  family as Method_6F003C90/Method_6F003B90 above, same reason kept as a
//  thunk.
struct GameMainHashLookupB3
{
    void* Method_6F004330(unsigned int, unsigned int);
    void* Method_6F005500(unsigned int, unsigned int, unsigned int);
};
ADDR_THUNK(void* GameMainHashLookupB3::Method_6F004330(unsigned int,
                                                        unsigned int),
           0x6F004330)
ADDR_THUNK(void* GameMainHashLookupB3::Method_6F005500(unsigned int,
                                                        unsigned int,
                                                        unsigned int),
           0x6F005500)

//  0x6F002F10 - retn 4, one stack dword, no register argument.  A
//  per-device config-file resolution path/width query (Storm ordinals
//  422/423/501/503 on a ~4KB stack buffer) this session did not chase.
ADDR_THUNK(int __stdcall GameMain_sub_6F002F10(unsigned int), 0x6F002F10)

//  0x6F6BF900 - retn 4, one stack dword, no register argument.  A
//  ~95-instruction leaf reached from the movie/path-lookup neighbourhood
//  (sub_6F6C2683/sub_6F6C2684 in the same dump) this session did not
//  trace further.
ADDR_THUNK(void __stdcall GameMain_sub_6F6BF900(unsigned int), 0x6F6BF900)

//  0x6F52A0C0 / 0x6F52B1C0 - retn 0 each, no arguments.  Each is the
//  SMemAlloc-then-placement-construct wrapper for one GX backend
//  (CGxDeviceD3d / CGxDeviceOpenGl respectively), under exactly the SEH
//  frame docs/msvc-vc8-idioms.md's "An `__except_handler4`-shaped frame
//  this toolchain cannot reproduce" section documents - same reason as
//  sub_6F0053B0 above, kept as thunks rather than landed at a known-low,
//  frame-explained score this session had no time left to verify.
ADDR_THUNK(void* __cdecl GameMain_sub_6F52A0C0(), 0x6F52A0C0)
ADDR_THUNK(void* __cdecl GameMain_sub_6F52B1C0(), 0x6F52B1C0)
