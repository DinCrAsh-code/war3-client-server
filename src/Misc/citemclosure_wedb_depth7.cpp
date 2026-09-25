//============================================================================
//  Depth-7 BFS descendants of CItem's vtable slots 32/104
//  (asm/processed/CItem__Method_0x80_0x6F2B6130_calltree_asm.md), continuing
//  Misc/citemclosure_terrain_gates.cpp's own frontier one level deeper.
//  Every one of these is confirmed unrelated to CItem/CSelectable by its own
//  evidence: a `WEDBEntryHash`/HASHKEY_STRI vtable stamp
//  (`??_7?$TSHashTable@UWEDBEntryHash@@VHASHKEY_STRI@@@@6B@`), Storm_401
//  allocation tags "HDBFIELD"/"HDATABASE", a ".slk" load path (World
//  Editor's generic named-database engine - not CObjectDataTable's own
//  0x6F25xxxx machinery, Game/objectdatatable.h, which is a different
//  instantiation of the same idea), or NTempest terrain type-descriptor
//  strings ("AucliffModel", "AuTerrainGeoset", "AuTerrainWater") and FPU
//  vertex-colour blend math - none of it item-positioning or selection
//  logic. Same convention as that file: a direct `mov eax,ADDR / jmp eax`
//  redirect into the game's own code at its real address, so a build
//  hooking anything else in this target still calls the genuine original
//  rather than link-failing on an unresolved symbol.  0% instruction match
//  by construction - see each BEHAVIOUR entry in tools/funcmap.py.
//
//  Declared signatures below exist only to give thunk_abi_audit.py the right
//  `retn <n>` - the byte count read straight off each dump's own trailing
//  `retn` line - not to claim a real calling convention: nothing in src/
//  calls any of these (grep says zero references outside this file), so the
//  declaration only has to compile and clean the right number of stack
//  bytes.  `__stdcall` with N plain int parameters is the simplest spelling
//  that cleans exactly 4*N bytes regardless of what the shipped body's real
//  arguments turn out to be; `__cdecl` with none for `retn 0`.
//============================================================================

//----------------------------------------------------------------------------
//  0x6F6F55D0 - a WEDB field array's bucket-capacity clamp: for an input
//  under 8 rounds up to the next power of two (or 1), for 8 and over stamps
//  a fixed cap of 8 into the object at +0x0C and returns it.  `retn 4`.
//----------------------------------------------------------------------------
__declspec(naked) void __stdcall ClampWEDBFieldBucketCapacity(int)
{
    __asm { mov eax, 06F6F55D0h }
    __asm { jmp eax }
}

//----------------------------------------------------------------------------
//  0x6F6F5630 - reallocates a WEDB field record array through Storm_401/405
//  (grow) or Storm_403 (release), all three tagged with the type descriptor
//  string "AUWEDBFIELD..." off this same object.  `retn 4`.
//----------------------------------------------------------------------------
__declspec(naked) void __stdcall ResizeWEDBFieldArray(int)
{
    __asm { mov eax, 06F6F5630h }
    __asm { jmp eax }
}

//----------------------------------------------------------------------------
//  0x6F6F8D80 - looks a field record up (ClampWEDBFieldBucketCapacity's
//  sibling helper, 0x6F6F8D50 - Misc/wedatabaserecordat.cpp's
//  CWEDatabase::RecordAt) and, when it comes back unset, logs
//  "WESTRING_WARNING_UNKNOWNDBFIELD" through a formatted Storm_578 call.
//  Has its own /GS-cookie prologue this repo's fixed `/GS-` build cannot
//  reproduce.  `retn 8`.
//----------------------------------------------------------------------------
__declspec(naked) void __stdcall LookupWEDBFieldOrWarn(int, int)
{
    __asm { mov eax, 06F6F8D80h }
    __asm { jmp eax }
}

//----------------------------------------------------------------------------
//  0x6F703800 - TSExplicitList<WEDBEntryHash>::UnlinkAll with TSLink::Unlink
//  inlined into the loop instead of called out to (see tslist.inl for the
//  called-out shape at 0x6F46B560) - drains every bucket of the table
//  0x6F70D070/0x6F70DAD0 construct.  `retn 0`.
//----------------------------------------------------------------------------
__declspec(naked) void __cdecl UnlinkAllWEDBEntryBuckets()
{
    __asm { mov eax, 06F703800h }
    __asm { jmp eax }
}

//----------------------------------------------------------------------------
//  0x6F7055F0 - calls UnlinkAllWEDBEntryBuckets on the table's all-nodes
//  list first, then either walks every bucket releasing each node through a
//  vtable slot-0 call (arg == 0) or unlinks every node directly without
//  releasing it (arg != 0) - the "delete nodes too, or just detach them"
//  fork a TSHashTable-shaped teardown takes.  `retn 4`.
//----------------------------------------------------------------------------
__declspec(naked) void __stdcall PurgeWEDBEntryBuckets(int)
{
    __asm { mov eax, 06F7055F0h }
    __asm { jmp eax }
}

//----------------------------------------------------------------------------
//  0x6F707B70 - walks every WEDBEntryHash bucket unlinking its nodes (same
//  inlined-Unlink loop as UnlinkAllWEDBEntryBuckets, over [this+8]/[this+4]
//  instead of the all-nodes list), then frees the bucket array itself
//  through Storm_403, tagged "AvTsexplicitli_129" (a TSExplicitList<T>
//  instance - same evidence shape as
//  citemclosure_terrain_gates.cpp's own ReleaseTerrainGridMeshLinkedNodes).
//  `retn 0`.  0x6F70B140 is a one-instruction `jmp` alias, same shape as
//  that file's own *Alias siblings.
//----------------------------------------------------------------------------
__declspec(naked) void __cdecl DestroyWEDBEntryBucketArray()
{
    __asm { mov eax, 06F707B70h }
    __asm { jmp eax }
}

__declspec(naked) void __cdecl DestroyWEDBEntryBucketArrayAlias()
{
    __asm { jmp DestroyWEDBEntryBucketArray }
}

//----------------------------------------------------------------------------
//  0x6F4CA130 - releases a WEDB field's index object (0x6F4C9D40) and, when
//  there was none, falls back to another release path (0x6F4C9C00) tagged
//  "HDBFIELD" through the shared logger (0x6F4C40C0).  `retn 0`.
//----------------------------------------------------------------------------
__declspec(naked) void __cdecl ReleaseWEDBFieldIndexOrWarn()
{
    __asm { mov eax, 06F4CA130h }
    __asm { jmp eax }
}

//----------------------------------------------------------------------------
//  0x6F4CB790 - loads a World Editor Database from a ".slk" (or a caller-
//  supplied) sheet: builds the path with Storm_501, parses it (0x6F4C1550),
//  then allocates and fills either an "HDATABASE"-tagged record via
//  Storm_401 + 0x6F4CB210, or a second shape via 0x6F4CB5C0 - all inside its
//  own SEH-framed, /GS-cookied prologue this repo's fixed `/GS-` build
//  cannot reproduce.  112 instructions.  `retn 0`.
//----------------------------------------------------------------------------
__declspec(naked) void __cdecl LoadWorldEditorDatabase()
{
    __asm { mov eax, 06F4CB790h }
    __asm { jmp eax }
}

//----------------------------------------------------------------------------
//  0x6F70D070 - stamps `??_7?$TSHashTable@UWEDBEntryHash@@VHASHKEY_STRI@@@@6B@`
//  and initialises the table's storage exactly like
//  Containers/hashtablectors.cpp's own four redirected constructors -
//  same "why a redirect and not a reconstruction" reasoning applies
//  (this repo's TSHashTable declares two virtuals it never defines, so
//  making the template concrete would stamp a vtable of this build's own
//  making over the shipped four-slot one).  `retn 0`.
//----------------------------------------------------------------------------
__declspec(naked) void __cdecl ConstructWEDBEntryTable()
{
    __asm { mov eax, 06F70D070h }
    __asm { jmp eax }
}

//----------------------------------------------------------------------------
//  0x6F70DAD0 - an enclosing object's own constructor: stamps the same
//  WEDBEntryHash vtable inline, then calls three more construction helpers
//  (0x6F705520, 0x6F707AF0, 0x6F7037B0) - SEH-framed for the same reason as
//  ConstructWEDBEntryTable.  Not the real CWEDatabase - a sibling depth-8
//  batch's own ConstructWEDatabase (0x6F70DEE0) carries the confirmed
//  `??_7CWEDatabase@@6B@` stamp, so this is a distinct object that also
//  embeds a WEDBEntryHash table.  `retn 0`.
//----------------------------------------------------------------------------
__declspec(naked) void __cdecl ConstructWEDBRecordStore()
{
    __asm { mov eax, 06F70DAD0h }
    __asm { jmp eax }
}

//----------------------------------------------------------------------------
//  0x6F706920 - releases several TRefCnt-shaped fields cached on a World
//  Editor Database object (calling the already-reconstructed
//  ReleaseForward_6F4C8870, Agent/refcnt.h, three times over different
//  field families) and walks two more record arrays releasing an entry
//  each through 0x6F4C9D30.  `retn 0`.
//----------------------------------------------------------------------------
__declspec(naked) void __cdecl ReleaseWEDatabaseCachedFields()
{
    __asm { mov eax, 06F706920h }
    __asm { jmp eax }
}

//----------------------------------------------------------------------------
//  0x6F6F9AD0 - resolves a WEDB field entry key, optionally re-keying it
//  through 0x6F6F1D70 first, then forwards to 0x6F6F9A60.  `retn 8`.
//----------------------------------------------------------------------------
__declspec(naked) void __stdcall ResolveWEDBFieldEntryKey(int, int)
{
    __asm { mov eax, 06F6F9AD0h }
    __asm { jmp eax }
}

//----------------------------------------------------------------------------
//  0x6F4D2150 - walks a WEDB field-array-of-arrays calling a caller-supplied
//  callback per entry, following a nested-array chain the same shape
//  ClampWEDBFieldBucketCapacity's neighbourhood uses.  `retn 8`.
//----------------------------------------------------------------------------
__declspec(naked) void __stdcall ForEachWEDBFieldEntry(int, int)
{
    __asm { mov eax, 06F4D2150h }
    __asm { jmp eax }
}

//----------------------------------------------------------------------------
//  0x6F7AF440 - unlinks a single node from each of two adjacent
//  TSExplicitList-shaped members (+0x04 and +0x0C) of a caller-supplied
//  object, the inlined TSLink::Unlink shape again but run at most once per
//  list rather than drained in a loop.  Genuinely `__stdcall` (no `ecx` read
//  anywhere in the body).  `retn 4`.
//----------------------------------------------------------------------------
__declspec(naked) void __stdcall ReleaseTerrainNodeListPair(int)
{
    __asm { mov eax, 06F7AF440h }
    __asm { jmp eax }
}

//----------------------------------------------------------------------------
//  0x6F7B0AC0 - a hash lookup over a table with `m_mask` at +0x24 and
//  `m_buckets.m_data` at +0x1C, matching a coordinate pair (two dwords at
//  +0x18/+0x1C of the candidate) against a caller-supplied key struct.
//  `retn 8`.
//----------------------------------------------------------------------------
__declspec(naked) void __stdcall FindTerrainCellByCoord(int, int)
{
    __asm { mov eax, 06F7B0AC0h }
    __asm { jmp eax }
}

//----------------------------------------------------------------------------
//  0x6F7B2880 - the mutating counterpart to FindTerrainCellByCoord: probes
//  the same table, indirects through a vtable slot when the first probe
//  misses, then splices a node into the table-wide list.  `retn 0Ch`.
//----------------------------------------------------------------------------
__declspec(naked) void __stdcall InsertOrUpdateTerrainCell(int, int, int)
{
    __asm { mov eax, 06F7B2880h }
    __asm { jmp eax }
}

//----------------------------------------------------------------------------
//  0x6F7B2B90 - hashes a string key (Storm_590) through a lookup
//  (0x6F7B2750) and, on a miss, allocates and links a new entry
//  (0x6F7B2920) before forwarding to 0x6F7B2B00 - an intern-or-lookup
//  pattern over the same "unk_6FADBEF4" table both callees share.  `retn 0Ch`.
//----------------------------------------------------------------------------
__declspec(naked) void __stdcall InternTerrainCellString(int, int, int)
{
    __asm { mov eax, 06F7B2B90h }
    __asm { jmp eax }
}

//----------------------------------------------------------------------------
//  0x6F7475D0 / 0x6F7477A0 - a per-scanline vertex-colour blend: for each
//  step between two endpoints, divides an integer delta by the step count,
//  rounds through the FPU (`fnstcw`/`or 0C00h`/`fldcw`, round-toward-zero)
//  and writes one interpolated byte per channel.  0x6F7475D0 writes the
//  four bytes as one packed dword per step (`[edi+edx*4]`); 0x6F7477A0
//  writes them as a packed word (`[ecx+edx*2-2]`) with an extra clamp
//  (`cmp ?,0F7h` / `add ?,8`) per channel first.  Both `retn 14h`.
//----------------------------------------------------------------------------
__declspec(naked) void __stdcall ComputeTerrainBlendWeightsPacked32(
        int, int, int, int, int)
{
    __asm { mov eax, 06F7475D0h }
    __asm { jmp eax }
}

__declspec(naked) void __stdcall ComputeTerrainBlendWeightsPacked16(
        int, int, int, int, int)
{
    __asm { mov eax, 06F7477A0h }
    __asm { jmp eax }
}

//----------------------------------------------------------------------------
//  0x6F7447E0 - releases every element of a cliff-model array through
//  0x6F789F20, then frees the array's own backing block, tagged
//  "AucliffModel...".  `retn 0`.
//----------------------------------------------------------------------------
__declspec(naked) void __cdecl ReleaseCliffModelArray()
{
    __asm { mov eax, 06F7447E0h }
    __asm { jmp eax }
}

//----------------------------------------------------------------------------
//  0x6F74FD10 - same release-loop shape as ReleaseCliffModelArray, over a
//  terrain-geoset array; the backing block is tagged "AuTerrainGeoset...".
//  `retn 0`.
//----------------------------------------------------------------------------
__declspec(naked) void __cdecl ReleaseTerrainGeosetArray()
{
    __asm { mov eax, 06F74FD10h }
    __asm { jmp eax }
}

//----------------------------------------------------------------------------
//  0x6F7537F0 - same release-loop shape again, over a terrain-water array;
//  the backing block is tagged "AuTerrainWater...".  `retn 0`.
//----------------------------------------------------------------------------
__declspec(naked) void __cdecl ReleaseTerrainWaterArray()
{
    __asm { mov eax, 06F7537F0h }
    __asm { jmp eax }
}

//----------------------------------------------------------------------------
//  0x6F73B350 - releases one terrain tile record: forwards a field of a
//  caller-supplied object plus three zero arguments to 0x6F526A10.
//  `retn 4`.
//----------------------------------------------------------------------------
__declspec(naked) void __stdcall ReleaseTerrainTileRecord(int)
{
    __asm { mov eax, 06F73B350h }
    __asm { jmp eax }
}

//----------------------------------------------------------------------------
//  0x6F762030 - a terrain texture-tile index lookup: when one input field
//  is set, forwards to 0x6F75F270 first, then always computes a tile offset
//  out of three more fields on `this` and the caller's own record.
//  `retn 8`.
//----------------------------------------------------------------------------
__declspec(naked) void __stdcall LookupTerrainTileIndex(int, int)
{
    __asm { mov eax, 06F762030h }
    __asm { jmp eax }
}

//----------------------------------------------------------------------------
//  0x6F71C1D0 - a third SEH-framed lazy-singleton-init gate, byte-identical
//  in shape to citemclosure_terrain_gates.cpp's own EnsureTerrainSubsystemA/
//  B (0x6F71C260/0x6F71C380) and Jass/jassstormterraingates.cpp's
//  StormTerrainSubsystemInitGate - same unreproducible cookie-XORed,
//  frame-pointer-omitted SEH frame.  Its own path string is
//  "e:\\Drive1\\temp\\buildwar3x\\War3\\Sou...".  `retn 0`.
//----------------------------------------------------------------------------
__declspec(naked) void __cdecl EnsureTerrainSubsystemC()
{
    __asm { mov eax, 06F71C1D0h }
    __asm { jmp eax }
}

//----------------------------------------------------------------------------
//  0x6F7857E0 - applies a vertex blend through a caller-object's own
//  callback field (+0x22EC): when set, stages six FPU values (three
//  midpoint/delta pairs derived from the caller's arguments plus a
//  constant, dbl_6F875120) and calls the callback with them.  `retn 18h`.
//----------------------------------------------------------------------------
__declspec(naked) void __stdcall ApplyTerrainMaterialVertexBlend(
        int, int, int, int, int, int)
{
    __asm { mov eax, 06F7857E0h }
    __asm { jmp eax }
}

//----------------------------------------------------------------------------
//  0x6F710950 - GetOrCreateWEDBRecord's own record allocator: probes a
//  table (same `m_mask`-at-+0x24 shape as FindTerrainCellByCoord's table),
//  falls back to 0x6F70F140/0x6F7103F0 on a miss, then calls a vtable slot
//  through the table object and splices the result into the table-wide
//  list.  `retn 0Ch`.
//----------------------------------------------------------------------------
__declspec(naked) void __stdcall GetOrCreateWEDBRecord(int, int, int)
{
    __asm { mov eax, 06F710950h }
    __asm { jmp eax }
}

//----------------------------------------------------------------------------
//  0x6F711520 - formats a "%s_%d"-keyed name (UnpackFourCCToBuffer's own
//  buffer plus a caller-supplied index), looks up an existing WEDB record
//  by it (ClampWEDBFieldBucketCapacity's sibling helper
//  ResolveWEDBFieldEntryKey-shaped lookup at 0x6F706B40 -
//  Misc/wedbentrylookup.cpp) and, on a miss, creates one via
//  CWEDatabase::RecordAt (0x6F6F8D50) + 0x6F4CA0F0 +
//  GetOrCreateWEDBRecord.  Has its own /GS-cookie prologue.  `retn 8`.
//----------------------------------------------------------------------------
__declspec(naked) void __stdcall AcquireNamedWEDBRecord(int, int)
{
    __asm { mov eax, 06F711520h }
    __asm { jmp eax }
}

//----------------------------------------------------------------------------
//  0x6F760340 / 0x6F7629E0, 0x6F7603A0 / 0x6F762A80, 0x6F760400 / 0x6F762B20
//  - three parallel TSHashTable-shaped caches, each one an
//  Initialize()/insert-with-grow pair over its own 4-bucket array at
//  [this+0x14] with `m_mask` at [this+0x24] - the exact layout
//  Storm/storm.h's own TSHashTable<T,K> declares (Containers/tshash.inl's
//  Initialize() is this same body).  Not written as three instantiations of
//  that template because the three node types' own identity is not
//  established by anything in this call tree - each Initialize's own
//  UnlinkAll callee (0x6F74B1E0/0x6F74B250/0x6F74B2C0) and each insert's own
//  grow callee (0x6F7604D0/0x6F7606A0/0x6F760870) are distinct, undumped
//  addresses this closure never names, so the numbered suffix records "a
//  sibling of the other two", not an invented identity - same convention as
//  citemclosure_terrain_gates.cpp's own CreateTerrainRecordByFourCC1/2/3.
//  The three Initialize()s are `retn 0`; the three insert-with-grows are
//  `retn 4`.
//----------------------------------------------------------------------------
__declspec(naked) void __cdecl InitializeWEDBCacheTable1()
{
    __asm { mov eax, 06F760340h }
    __asm { jmp eax }
}

__declspec(naked) void __stdcall FindOrGrowWEDBCacheTable1(int)
{
    __asm { mov eax, 06F7629E0h }
    __asm { jmp eax }
}

__declspec(naked) void __cdecl InitializeWEDBCacheTable2()
{
    __asm { mov eax, 06F7603A0h }
    __asm { jmp eax }
}

__declspec(naked) void __stdcall FindOrGrowWEDBCacheTable2(int)
{
    __asm { mov eax, 06F762A80h }
    __asm { jmp eax }
}

__declspec(naked) void __cdecl InitializeWEDBCacheTable3()
{
    __asm { mov eax, 06F760400h }
    __asm { jmp eax }
}

__declspec(naked) void __stdcall FindOrGrowWEDBCacheTable3(int)
{
    __asm { mov eax, 06F762B20h }
    __asm { jmp eax }
}
