//============================================================================
//  gamemain-batch-5's own not-reconstructed callees - redirects into the
//  real, unhooked shipped bodies at their real addresses.  Same shape and
//  same rules as Misc/spritehandlethunks.cpp and Misc/unreconstructed_
//  thunks.cpp: NOT reconstructions, and the declared argument bytes have
//  to agree with each one's real `retn <n>` (tools/thunk_abi_audit.py).
//
//  Each entry below says in one line why it stayed a redirect rather than
//  being written out - the ~TSHashTable/Clear pair's vtable blocker, the
//  event-scheduler priority-heap cluster's cross-function invariants this
//  batch's own dump does not fully cover, the two big field-init `CGxDevice*`
//  constructors that stamp a vtable this batch did not audit for
//  completeness, and a handful of self-contained-but-large leaves (a 112-
//  instruction UTF-8 decoder, a 127-instruction gamma-ramp window setup)
//  that were out of this batch's budget.
//============================================================================

#define ADDR_THUNK(sig, addr)                     \
    __declspec(naked) sig                         \
    {                                             \
        __asm { mov eax, addr }                   \
        __asm { jmp eax }                         \
    }

//----------------------------------------------------------------------------
//  TSHashTable<PrefString,HASHKEY_NONE>/TSHashTable<SPreferenceEntry,int>'s
//  own ~TSHashTable, and the TAllocatorHashTable<UncachableNode,...>/
//  TSHashTable_P<PrefetchListFile,...> "clear every bucket, and either
//  physically unlink each node (arg!=0) or hand it to vtable slot 0
//  (arg==0)" member both templates carry at this same field layout.  All
//  four stamp/read a vtable this repo's TSHashTable/TAllocatorHashTable
//  declare only two of the shipped four slots for (AllocNode is declared,
//  never defined - Containers/hashtablectors.cpp's own note on why the
//  three ConstructPreference*/ConstructNativeToken constructors stay
//  redirects for exactly this reason).  Defining any of these four would
//  force the vtable's own COMDAT to be emitted in this TU, which needs a
//  real address for the slot(s) this repo never gives AllocNode a body
//  for - the identical blocker, one level further down the same four
//  functions' own call graph.
//----------------------------------------------------------------------------
//  0x6F004CD0 - `retn 0`, thiscall.  SPreferenceCache::~SPreferenceCache.
struct SPreferenceCacheDtorThunk { void Destroy(); };
ADDR_THUNK(void SPreferenceCacheDtorThunk::Destroy(), 0x6F004CD0)
//  0x6F0049D0 - `retn 0`, thiscall.  CPreferencesWar3's own ~TSHashTable
//  half (see Config/preferencehashtable.cpp for everything else of both
//  tables that *is* reconstructed).
struct PrefStringTableDtorThunk { void Destroy(); };
ADDR_THUNK(void PrefStringTableDtorThunk::Destroy(), 0x6F0049D0)
//  0x6F4BDEF0 - `retn 4`, thiscall + one stack bool.
//  TAllocatorHashTable<UncachableNode,HASHKEY_STRI,32>::Clear.
struct UncachableTableClearThunk { void Clear(int freeNodes); };
ADDR_THUNK(void UncachableTableClearThunk::Clear(int), 0x6F4BDEF0)
//  0x6F4BDFF0 - `retn 4`, thiscall + one stack bool.
//  TSHashTable_P<PrefetchListFile,HASHKEY_STRI_P>::Clear.
struct PrefetchListTableClearThunk { void Clear(int freeNodes); };
ADDR_THUNK(void PrefetchListTableClearThunk::Clear(int), 0x6F4BDFF0)

//----------------------------------------------------------------------------
//  Storm's own UTF-8 -> UTF-16 decoder: a 112-instruction, six-way jump
//  table over the UTF-8 lead-byte length class, with the classic surrogate-
//  pair math (`-0x2800`/`-0x2400` truncated to 16 bits) past U+FFFF.
//  __fastcall(ecx=outputBase, edx=outputIndex), `retn 0x10` for four stack
//  arguments (input, inputLen, and two out-pointers).  Self-contained and
//  correct-by-construction from the lookup tables alone, but out of this
//  batch's budget to transcribe the jump table and surrogate arithmetic
//  byte-for-byte.
//----------------------------------------------------------------------------
//  0x6F4C8160 - `retn 10h`.
struct Utf8DecodeThunk { void Decode(const char*, int, void*, void*); };
ADDR_THUNK(void __fastcall Utf8DecodeThunkFn(void*, int, const char*, int, void*, void*),
           0x6F4C8160)

//----------------------------------------------------------------------------
//  The display-mode-state cluster (sub_6F525F90's own closure): the 0/1/2
//  device init/shutdown state machine that owns dword_6FA88720/
//  dword_6FA88724 (sub_6F526CA0, thiscall, `retn 0`), and a 200-
//  instruction SEH-framed mode list dedup/insertion-sort (sub_6F008F10,
//  thiscall, `retn 0`) - neither of their own callees (sub_6F52CAB0/
//  sub_6F52A1B0/sub_6F52AE00/sub_6F0087C0/etc.) are in this batch's dump,
//  so nothing here could be checked against a real callee signature.  A
//  third member of this cluster, sub_6F529E40's own mode ranking/
//  enumeration table walk, was independently assigned to both this batch
//  and gamemain-batch-3, which reconstructed it for real as
//  EnumerateDefaultDisplayResolution - dropped from here in its favour.
//----------------------------------------------------------------------------
struct DisplayDeviceStateMachineThunk { void Step(); };
ADDR_THUNK(void DisplayDeviceStateMachineThunk::Step(), 0x6F526CA0)
struct DisplayModeListInsertThunk { void Insert(); };
ADDR_THUNK(void DisplayModeListInsertThunk::Insert(), 0x6F008F10)

//----------------------------------------------------------------------------
//  CGxDeviceOpenGl::CGxDeviceOpenGl and CGxDeviceD3d::CGxDeviceD3d - two
//  large field-init constructors that each stamp their own class's vtable.
//  CLAUDE.md's ctor_vtable_audit.py rule: a reconstructed constructor may
//  stamp a vtable only for a class this batch has confirmed *finished* in
//  vtable_classes.json, which neither class was audited for here - so both
//  stay redirects into the real, unhooked constructor rather than risk
//  installing a short table.  Both `retn 0`, thiscall.
//----------------------------------------------------------------------------
struct CGxDeviceOpenGlCtorThunk { void Construct(); };
ADDR_THUNK(void CGxDeviceOpenGlCtorThunk::Construct(), 0x6F52AFA0)
struct CGxDeviceD3dCtorThunk { void Construct(); };
ADDR_THUNK(void CGxDeviceD3dCtorThunk::Construct(), 0x6F529F20)

//----------------------------------------------------------------------------
//  A small string/buffer leaf whose one callee (sub_6F555630) is not in
//  this batch's dump.  __fastcall(ecx=buffer, edx=length) + one stack arg
//  (destSize), `retn 4`; returns 1 when the callee's answer lands inside
//  destSize (and null-terminates the buffer there) or 0 otherwise.
//  Spelled under its own raw address-name and exact int/void*/int/
//  unsigned-int signature rather than a distinct C++ name: Render/
//  fontfileversionarray.cpp (gamemain-batch-4) already forward-declares
//  and calls it as `sub_6F553A70` directly, expecting this batch to
//  supply the definition - a mismatched name or return type here would
//  compile clean on both sides and only fail at the real link
//  (docs/notes/verifier-gate-link-failures.md, cause 1).
//----------------------------------------------------------------------------
ADDR_THUNK(int __fastcall sub_6F553A70(void*, int, unsigned int), 0x6F553A70)

//----------------------------------------------------------------------------
//  Two SEH-framed constructors this batch could not name with confidence:
//  a critical-section-guarded object at 0x6F62F1D0 (thiscall, `retn 0`)
//  whose own class this dump gives no further context for, and an array-
//  of-400-Concurrency::ReaderWriterLock member (`eh vector constructor
//  iterator`) at 0x6F6E8350 (thiscall, `retn 0`) - Concurrency Runtime
//  internals CLAUDE.md's own "work outward, not into Storm's/the CRT's
//  internals" guidance already stops at.
//----------------------------------------------------------------------------
struct CritSecGuardedCtorThunk { void Construct(); };
ADDR_THUNK(void CritSecGuardedCtorThunk::Construct(), 0x6F62F1D0)
struct ReaderWriterLockArrayCtorThunk { void Construct(); };
ADDR_THUNK(void ReaderWriterLockArrayCtorThunk::Construct(), 0x6F6E8350)

//----------------------------------------------------------------------------
//  The event-scheduler / handle-table priority-heap cluster (sub_6F62FC50's
//  own closure): sift-up/sift-down and growable-array plumbing over an
//  indirect vtable-slot-0 comparison callback, none of whose surrounding
//  callers/callees (dword_6FACEB5C/60/68, sub_6F050DF0, sub_6F6321D0,
//  sub_6F00D230/sub_6F00D290) are in this batch's dump either, so the
//  field layout could not be confirmed independently of guessing at the
//  whole cluster together - out of this batch's budget as one unit rather
//  than reconstructed piecemeal against unconfirmed offsets.
//----------------------------------------------------------------------------
//  0x6F62E880/0x6F62EA20 are thiscall plus one *stack* argument (`retn 4`),
//  not fastcall - `mov eax,[esp+4+arg_0]` reads the stack slot ahead of
//  `mov edi/esi,ecx` picking up `this`.
struct EventHeapEraseThunk { void Erase(unsigned int); };
ADDR_THUNK(void EventHeapEraseThunk::Erase(unsigned int), 0x6F62E880)
struct EventHeapReindexThunk { void Reindex(unsigned int); };
ADDR_THUNK(void EventHeapReindexThunk::Reindex(unsigned int), 0x6F62EAC0)
struct EventHeapInsertThunk { void Insert(unsigned int); };
ADDR_THUNK(void EventHeapInsertThunk::Insert(unsigned int), 0x6F62EA20)
struct EventHeapGrowThunk { void Grow(unsigned int); };
ADDR_THUNK(void EventHeapGrowThunk::Grow(unsigned int), 0x6F632E30)
struct EventHeapDispatchThunk { void Dispatch(); };
ADDR_THUNK(void __fastcall EventHeapDispatchThunkFn(void*), 0x6F632240)
struct EventHeapNodeInitThunk { void Init(); };
ADDR_THUNK(void EventHeapNodeInitThunk::Init(), 0x6F62ECF0)
struct EventHeapBucketAllocThunk { void Alloc(unsigned int); };
ADDR_THUNK(void EventHeapBucketAllocThunk::Alloc(unsigned int), 0x6F62D320)
struct EventHeapBucketChunkThunk { void Chunk(unsigned int); };
ADDR_THUNK(void EventHeapBucketChunkThunk::Chunk(unsigned int), 0x6F62D2C0)
struct EventHeapRemoveThunk { void Remove(unsigned int); };
ADDR_THUNK(void EventHeapRemoveThunk::Remove(unsigned int), 0x6F62F540)

//----------------------------------------------------------------------------
//  The "BlizPlay" cinematic-window bring-up/message-pump/teardown cluster
//  (sub_6F52C4xx's own closure).  0x6F52C760 (fastcall(ecx=hInstance,
//  edx=?), `retn 0`) is a 127-instruction RegisterClassA/CreateWindowExA/
//  gamma-ramp setup out of this batch's budget on size alone; 0x6F52BAF0
//  (no args, `retn 0`) and 0x6F52BB50 (no args, `retn 0`) are its message
//  pump and CoUninitialize/ShowCursor teardown, both small but calling a
//  third function (sub_6F52B930, a 133-instruction ChangeDisplaySettingsA/
//  Sleep display-mode switch) that is not in this batch's dump either, so
//  redirecting the small two without the big one they share a callee with
//  would leave the callee itself unresolved.
//----------------------------------------------------------------------------
struct BlizPlayWindowSetupThunk { void Setup(); };
ADDR_THUNK(void __fastcall BlizPlayWindowSetupThunkFn(void*, void*), 0x6F52C760)
struct BlizPlayMessagePumpThunk { void Pump(); };
ADDR_THUNK(void BlizPlayMessagePumpThunk::Pump(), 0x6F52BAF0)
struct BlizPlayTeardownThunk { void Teardown(); };
ADDR_THUNK(void BlizPlayTeardownThunk::Teardown(), 0x6F52BB50)

#undef ADDR_THUNK
