//============================================================================
//  Depth-10 BFS descendants of CItem's vtable slots 32/104
//  (asm/processed/CItem__Method_0x80_0x6F2B6130_calltree_asm.md), continuing
//  Misc/citemclosure_wedb_depth7.cpp's own frontier three levels deeper.
//  Every function below is confirmed unrelated to CItem/CSelectable by its
//  own evidence, same as that file and Misc/citemclosure_database_thunks.cpp:
//
//    - the 0x6F4Cxxxx cluster (sub_6F733470 through sub_6F4CAF80) is the
//      same CDatabaseBase/TSHashTable<CRowOrColumn,...> family
//      citemclosure_database_thunks.cpp already documents as "no
//      docs/targets/vtables/ entry for any of CDatabaseBase,
//      CDatabaseHashTable, CSLKDatabase or CDBDatabase" - sub_6F4CA7F0's own
//      own vtable stamp (`??_7?$TSHashTable@VCRowOrColumn@@VHASHKEY_CONSTSTR@@@@6B@`)
//      confirms the same "this repo's TSHashTable declares two virtuals,
//      the shipped tables carry four" trap Containers/hashtablectors.cpp's
//      own note explains for every other TSHashTable ctor/dtor in this
//      repo; sub_6F733470/sub_6F4C96B0/sub_6F4C95F0/sub_6F4CAF80 are the
//      same table family's own node-unlink/rehash helpers, walked from
//      inside that dtor and from sub_6F7344D0 (LookupWorldEditStringOrWarn's
//      own out-of-scope diagnostic path, tools/funcmap.py's
//      LookupWorldEditStringOrWarn entry) - a raw pointer-chase over the
//      bucket array with no vtable of its own to get wrong, but still this
//      same unreconstructed table's internals;
//    - sub_6F4C8E60/sub_6F00BFC0 are a byte-identical pair, each reached
//      only from an SEH/`__CxxFrameHandler3` unwind funclet belonging to
//      that same CDatabaseBase family (0x6F4C99F0/0x6F4CAEE0's own
//      teardown, neither in this batch) - `CHandleObject`'s own trivial,
//      no-member-cleanup base destructor tail, restamping
//      `??_7CHandleObject@@6B@` on unwind;
//    - sub_6F4CAC60 is a one-instruction `jmp sub_6F4CA7F0` alias into that
//      same destructor;
//    - the 0x6F7Axxxx/0x6F70xxxx/0x6F75xxxx/0x6F52xxxx cluster
//      (sub_6F7AEFC0 through sub_6F7079B0) sits in the same NTempest
//      terrain-rendering neighbourhood Misc/citemclosure_terrain_gates.cpp
//      and citemclosure_terrain_gates2.cpp already establish for this
//      closure - several pairs here are themselves byte-identical twins in
//      two different terrain subsystems (sub_6F7B07B0/sub_6F7079B0;
//      sub_6F7B1CB0 repeats sub_6F733470's own four-element-array-reset
//      shape one module over), the same "this build does not fold
//      identical helpers across translation units" duplication
//      citemclosure_wedb_depth7.cpp's own UnlinkAllWEDBEntryBuckets/
//      DestroyWEDBEntryBucketArray pair already documents.
//
//  Same convention as every file above: a direct `mov eax,ADDR / jmp eax`
//  redirect into the game's own code at its real address, so a build
//  hooking anything else in this target still calls the genuine original
//  rather than link-failing on an unresolved symbol.  0% instruction match
//  by construction - see each BEHAVIOUR entry in tools/funcmap.py.
//
//  Declared signatures below exist only to give thunk_abi_audit.py the
//  right `retn <n>` - the byte count read straight off each dump's own
//  trailing `retn` line - not to claim a real calling convention: nothing
//  in src/ calls any of these (grep says zero references outside this
//  file), so the declaration only has to compile and clean the right
//  number of stack bytes.  A one-method struct spells `this` in ecx for
//  every entry that reads `[ecx+...]` as its first instruction; `__stdcall`
//  with N plain int parameters is the simplest spelling that cleans exactly
//  4*N bytes for the two that do not (sub_6F4D2080, sub_6F528470).
//============================================================================

#define ADDR_THUNK(sig, addr)                     \
    __declspec(naked) sig                         \
    {                                             \
        __asm { mov eax, addr }                   \
        __asm { jmp eax }                         \
    }

//----------------------------------------------------------------------------
//  The CDatabaseBase/TSHashTable<CRowOrColumn,HASHKEY_CONSTSTR> family.
//----------------------------------------------------------------------------

//  0x6F733470 - `retn 0`.  This table's own node-array reset: four
//  0xC-byte slots at +0x1C, each either torn down through sub_6F72C740
//  (out of this batch's scope) when already live or freshly linked to a
//  self-referencing sentinel; called from sub_6F7344D0
//  (LookupWorldEditStringOrWarn's own diagnostic path) as the table's lazy
//  first-use init.
struct SRowOrColumnResetThunk { void Reset(); };
ADDR_THUNK(void SRowOrColumnResetThunk::Reset(), 0x6F733470)

//  0x6F734070 - `retn 4`.  The same table's CheckGrow: a chain-length probe
//  over the raw bucket array at +0x1C (mask at +0x24, probe count at
//  +0x10), doubling through sub_6F7336A0 (out of scope) past 13 probes -
//  Containers/tshash.inl's own CheckGrow, but with Tail()/Prev() inlined to
//  raw pointer arithmetic rather than left as calls, the way this table's
//  particular instantiation compiled.
struct SRowOrColumnCheckGrowThunk { int CheckGrow(int index); };
ADDR_THUNK(int SRowOrColumnCheckGrowThunk::CheckGrow(int index), 0x6F734070)

//  0x6F4C96B0 - `retn 0`.  Unlinks two adjacent TSLink-shaped fields
//  (+0x0C and +0x04) from whatever list each currently sits on - the same
//  inlined-Unlink shape sub_6F4C95F0/sub_6F4CA7F0 below share, over this
//  object's own two link fields rather than a bucket array.
struct SRowOrColumnUnlinkPairThunk { void UnlinkPair(); };
ADDR_THUNK(void SRowOrColumnUnlinkPairThunk::UnlinkPair(), 0x6F4C96B0)

//  0x6F4C95F0 - `retn 0`.  Drains every bucket of the raw array at
//  `this` (count at +8, data at +0), unlinking each live node in place -
//  the same TSExplicitList::UnlinkAll shape citemclosure_wedb_depth7.cpp's
//  own UnlinkAllWEDBEntryBuckets already reconstructs, over this table's
//  own (unreconstructed) node type.  Called from sub_6F4CA7F0's own
//  teardown.
struct SRowOrColumnUnlinkAllThunk { void UnlinkAll(); };
ADDR_THUNK(void SRowOrColumnUnlinkAllThunk::UnlinkAll(), 0x6F4C95F0)

//  0x6F4C8E60 / 0x6F00BFC0 - `retn 0` each, byte-identical two-instruction
//  bodies (`mov dword ptr [ecx], offset ??_7CHandleObject@@6B@` / `retn`).
//  See the file header - `CHandleObject`'s own trivial base destructor,
//  reached only from an unwind funclet.
struct SHandleObjectTrivialDtorThunk { void Destruct(); };
ADDR_THUNK(void SHandleObjectTrivialDtorThunk::Destruct(), 0x6F4C8E60)

struct SHandleObjectTrivialDtorThunkB { void Destruct(); };
ADDR_THUNK(void SHandleObjectTrivialDtorThunkB::Destruct(), 0x6F00BFC0)

//  0x6F4CAC60 - `retn 0`, one instruction: `jmp sub_6F4CA7F0`.
struct SRowOrColumnDtorAliasThunk { void Destruct(); };
ADDR_THUNK(void SRowOrColumnDtorAliasThunk::Destruct(), 0x6F4CAC60)

//  0x6F4CA7F0 - `retn 0`.  This table's own destructor: SEH-framed
//  (`push offset SEH_6F4CA7F0` / `large fs:0`), the same
//  __except_handler4-shaped frame docs/msvc-vc8-idioms.md's own note
//  already establishes this fixed /GS- /EHs-c- toolchain cannot reproduce
//  byte-for-byte from any source shape - stamps
//  `??_7?$TSHashTable@VCRowOrColumn@@VHASHKEY_CONSTSTR@@@@6B@`, then tears
//  down its own +0x14/+0x04 sub-objects through sub_6F4C9780/
//  sub_6F4CA240/sub_6F4C95F0 (all out of scope) before unlinking itself.
struct SRowOrColumnDtorThunk { void Destruct(); };
ADDR_THUNK(void SRowOrColumnDtorThunk::Destruct(), 0x6F4CA7F0)

//  0x6F4CAF80 - `retn 8`.  A hash-bucket insert-or-replace over the same
//  table's raw array (mask/probe fields at +0x24/+0x10, bucket data at
//  +0x1C for the first key and +0x04/+0x08 for a second, smaller chain) -
//  lazily calling sub_6F4CA900 to initialise on first use, then
//  sub_6F4CACA0 for the hash lookup, splicing a node out of one chain and
//  into another via the same inlined-Unlink shape as the rest of this
//  family.
struct SRowOrColumnInsertThunk { void* Insert(int key, int value); };
ADDR_THUNK(void* SRowOrColumnInsertThunk::Insert(int key, int value),
           0x6F4CAF80)

//----------------------------------------------------------------------------
//  0x6F4D2080 - `retn 10h`, no `this` (all four arguments on the stack,
//  the first loaded straight into `eax`): a one-shot lookup
//  (sub_6F4D1EB0) followed by a Storm_501 call carrying its own three
//  remaining arguments and a byte flag zeroed up front - out of this
//  batch's own scope (sub_6F4D1EB0 has no dump here).
//----------------------------------------------------------------------------
ADDR_THUNK(void __stdcall MiscDatabaseLookupAndReport(int, int, int, int),
           0x6F4D2080)

//----------------------------------------------------------------------------
//  The NTempest terrain-rendering neighbourhood.
//----------------------------------------------------------------------------

//  0x6F7B07B0 / 0x6F7079B0 - `retn 4` each, byte-identical 115-instruction
//  bodies (same `push ecx`/`push ebx`/`push ebp`/`push esi`/`mov ebx,ecx`/
//  `mov eax,[ebx+8]` opening) in two different terrain-neighbourhood
//  modules - the same "this build does not fold identical helpers across
//  translation units" duplication the file header names.
struct STerrainHelperThunkA { int Method(int arg0); };
ADDR_THUNK(int STerrainHelperThunkA::Method(int arg0), 0x6F7B07B0)

struct STerrainHelperThunkB { int Method(int arg0); };
ADDR_THUNK(int STerrainHelperThunkB::Method(int arg0), 0x6F7079B0)

//  0x6F7AFB20 - `retn 0`.  Drains a raw bucket array (count at +8, data at
//  +0) the same shape as SRowOrColumnUnlinkAllThunk above, over a terrain
//  table's own (unreconstructed) node type.
struct STerrainUnlinkAllThunk { void UnlinkAll(); };
ADDR_THUNK(void STerrainUnlinkAllThunk::UnlinkAll(), 0x6F7AFB20)

//  0x6F7B1240 - `retn 4`.
struct STerrainHelperThunkC { int Method(int arg0); };
ADDR_THUNK(int STerrainHelperThunkC::Method(int arg0), 0x6F7B1240)

//  0x6F7B2170 - `retn 4`.  152 instructions, its own local `sub esp,14h`
//  frame (five DWORD locals) rather than an SEH one - not reproduced this
//  session, out of this batch's own reach without a dump of its several
//  unnamed callees.
struct STerrainHelperThunkD { int Method(int arg0); };
ADDR_THUNK(int STerrainHelperThunkD::Method(int arg0), 0x6F7B2170)

//  0x6F7B1CB0 - `retn 0`.  The same four-element-array-reset shape as
//  sub_6F733470 above (`push 4` / `lea ecx,[edi+14h]` opening), one
//  terrain-side instantiation of the same generic "reset this table's own
//  small fixed node array" pattern.
struct STerrainResetThunk { void Reset(); };
ADDR_THUNK(void STerrainResetThunk::Reset(), 0x6F7B1CB0)

//  0x6F7B2490 - `retn 4`.  The same CheckGrow shape as sub_6F734070 above
//  (`cmp [ecx+24h], 1FFFh` opening), a terrain table's own instantiation of
//  the same generic hash growth check.
struct STerrainCheckGrowThunk { int CheckGrow(int index); };
ADDR_THUNK(int STerrainCheckGrowThunk::CheckGrow(int index), 0x6F7B2490)

//  0x6F753700 - `retn 0`.  Tears down a +0x6C sub-object through
//  sub_6F74FCB0 (out of scope) then conditionally releases a +0x64 pointer.
struct STerrainDtorThunk { void Destruct(); };
ADDR_THUNK(void STerrainDtorThunk::Destruct(), 0x6F753700)

//----------------------------------------------------------------------------
//  0x6F528470 - `retn 8`, no `this` (both arguments on the stack): stamps
//  a `1` through its first argument, then walks a second argument's own
//  small array field - out of this batch's own scope without a dump of the
//  helpers that array walk reaches.
//----------------------------------------------------------------------------
ADDR_THUNK(void __stdcall TerrainStampAndWalk(int, int), 0x6F528470)

//  0x6F754280 - `retn 4`.  118 instructions, xor-zeroes an accumulator
//  register up front and walks a caller-supplied index - out of this
//  batch's own reach without a dump of its unnamed callees.
struct STerrainHelperThunkE { int Method(int arg0); };
ADDR_THUNK(int STerrainHelperThunkE::Method(int arg0), 0x6F754280)
