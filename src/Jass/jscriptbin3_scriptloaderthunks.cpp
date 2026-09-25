//============================================================================
//  jscript-bin3's own worklist (a 36-address BFS closure rooted at the JASS
//  `.j` script loader) - the core common.j/blizzard.j loading family.  Every
//  one of these is a real, understood function (see the comment above each),
//  kept a self-redirect thunk rather than reconstructed this session: each
//  calls at least one genuinely unreconstructed callee (a distinct compiler
//  entry point, a checksum/registration helper, or a JASS-native-table
//  lookup whose owning type is not confidently pinned down from this
//  closure alone), so a from-scratch transcription would either fail to
//  link or silently guess at a callee's signature.  Left for a session that
//  can reach those callees too.
//============================================================================

#define SELF_REDIRECT(addr)          \
    {                                 \
        __asm { mov eax, addr }      \
        __asm { jmp eax }             \
    }

//----------------------------------------------------------------------------
//  0x6F3B1A20 - `retn 20h` (8 stack args, no register args).  Loads
//  common.j and blizzard.j (through sub_6F3B1970, called with esi pointed at
//  each literal in turn) and folds their two checksums (sub_6F39E5C0) with
//  the caller-supplied seed via xor/rol into a combined script checksum,
//  releasing every loaded buffer (ReleaseLoadedFile/sub_6F4BC6B0) on the way
//  out.  Read first for orientation per this round's own task description.
//----------------------------------------------------------------------------
__declspec(naked) void __stdcall ComputeScriptChecksums(
    void*, void*, void*, void*, void*, void*, void*, void*)
    SELF_REDIRECT(06F3B1A20h)

//----------------------------------------------------------------------------
//  0x6F3B1C30 - `retn 0` (__fastcall, ecx/edx both registers).  Calls
//  ComputeScriptChecksums above, then - only if that succeeded - hands
//  "common.j"/"blizzard.j" plus the three loaded buffers/checksums to
//  sub_6F458900 (the JASS compile-and-register entry every JASS-loading
//  root in this closure funnels through) with a fixed callback
//  (sub_6F3B18A0), releasing the buffers afterward either way.
//----------------------------------------------------------------------------
__declspec(naked) void __fastcall LoadAndCompileCommonScripts(void*, void*)
    SELF_REDIRECT(06F3B1C30h)

//----------------------------------------------------------------------------
//  0x6F737260 - `retn 4` (__fastcall ecx/edx, one stack arg).  The sibling
//  entry point sub_6F3B1C30 above front-loads: opens "common.j" (returning
//  early if that fails), then - gated on the stack flag - "common.ai" or
//  "blizzard.j", computes a checksum over the ecx-supplied buffer
//  (sub_6F72C1A0/Storm_501), and funnels all three into the same
//  sub_6F458900 compile entry with a different callback (sub_6F72D1F0).
//  One of the handful of functions this round's task description calls out
//  by address as directly referencing common.j/common.ai/blizzard.j
//  together.
//----------------------------------------------------------------------------
__declspec(naked) void __fastcall LoadCoreJassScripts(void*, void*, int)
    SELF_REDIRECT(06F737260h)

//----------------------------------------------------------------------------
//  0x6F39EC70 - `retn 0` (__fastcall: ecx = filename extension, e.g.
//  ".w3e"; edx = SHA1_CTX* this closure's caller owns).  Builds
//  "war3map<ext>" (Storm_578, "%s%s") into a stack buffer, loads that file
//  through the file cache (LoadCachedFile), SHA1-updates the context with
//  its bytes when the load succeeds (SHA1_CTX::Update), and releases the
//  buffer (ReleaseLoadedFile).  SHA1_CTX itself is private to
//  Net/netdata_loadreplayheader.cpp (no shared header), which is the actual
//  blocker to a from-scratch reconstruction here rather than any gap in
//  understanding the body.
//----------------------------------------------------------------------------
__declspec(naked) void __fastcall UpdateMapFileChecksum(const char*, void*)
    SELF_REDIRECT(06F39EC70h)

//----------------------------------------------------------------------------
//  0x6F39ED00 - `retn 0` (__fastcall: ecx = the SHA1_CTX* forwarded to
//  UpdateMapFileChecksum above; edx = "include optional files" flag).  Folds
//  every map component file's checksum into one running hash: always
//  .w3e/.wpm/.doo/.w3u, and - only when edx is set - .w3b/.w3d/.w3a/.w3q
//  too, tail-calling UpdateMapFileChecksum for the last of those instead of
//  a plain call.  Depends on the same function above.
//----------------------------------------------------------------------------
__declspec(naked) void __fastcall ComputeMapChecksumSet(void*, int)
    SELF_REDIRECT(06F39ED00h)

//----------------------------------------------------------------------------
//  0x6F39F5D0 - `retn 4` (__thiscall, one stack arg).  `this + 0x1Ch`
//  plus the stack argument and a fixed callback pointer (sub_6F42FC50) go
//  straight into sub_6F44B700, an unreconstructed registration/list-insert
//  helper this closure never reaches the body of.
//----------------------------------------------------------------------------
__declspec(naked) void RegisterAtField1C(void* self, void* arg)
    SELF_REDIRECT(06F39F5D0h)

//----------------------------------------------------------------------------
//  0x6F2E3840 - `retn 10h` (__thiscall, 4 stack args).  Stores two of its
//  four stack arguments into `this+0x38`/`this+0x3C`, calls a resolver
//  through `this+0x24`'s own vtable slot 0 (SOptionalHandleRefResolver::
//  ResolveChained-shaped, already reconstructed elsewhere in this closure)
//  with a fixed global argument, forwards `this+0x38` and a fixed global
//  (dword_6FAB65F4) into RegisterAtField1C above, then calls
//  sub_6F2E0B70 - unreconstructed, so the whole function stays a redirect.
//----------------------------------------------------------------------------
__declspec(naked) void InitAndRegisterField1C(void* self, void*, void*, void*)
    SELF_REDIRECT(06F2E3840h)

//----------------------------------------------------------------------------
//  0x6F44EA00 - `retn 0` (__thiscall, no stack args).  Reads the running
//  thread's own slot-5 thread-local block (GetThreadLocalSlot) and looks a
//  key up in the table at its own +0x18 through what funcmap.py already
//  names TAllocatorHashTable<JassNativeSlot,HASHKEY_STR,64>::Ptr
//  (sub_6F44CDA0) - but jassthreadstate.h's own layout puts a *different*
//  table type (TSHashTable<NativeFunc,HASHKEY_STR>, m_natives) at that same
//  offset, a naming/layout mismatch this session could not resolve from
//  this closure alone.  Left a redirect rather than risk compiling against
//  the wrong table type.
//----------------------------------------------------------------------------
__declspec(naked) void* __fastcall LookupNativeSlotForThisThread(unsigned int)
    SELF_REDIRECT(06F44EA00h)

#undef SELF_REDIRECT
