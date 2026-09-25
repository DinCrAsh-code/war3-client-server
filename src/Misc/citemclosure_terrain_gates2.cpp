//============================================================================
//  agent-citem-batch-C: 36 more addresses at BFS depth 6 out of CItem's
//  vtable slots 32/104 (CItem::ClearPendingAndNotify /
//  CSelectable::RefreshAttachmentPlacement -
//  asm/processed/CItem__Method_0x80_0x6F2B6130_calltree_asm.md), one hop
//  deeper than citemclosure_terrain_gates.cpp's own frontier (several of
//  these addresses are the very callees that file's comments already named
//  as "unreached" from depth 5 - sub_6F73E9F0, sub_6F711630, sub_6F6F0C90,
//  sub_6F711CD0, and the sibling of sub_6F6F1A00).  Every one below is
//  confirmed unrelated to CItem/CSelectable by its own evidence: NTempest-
//  namespaced vtable stamps (CWEDatabase, CCliffDB, CWaterDB,
//  TSHashTable<TerrainTileHash,HASHKEY_DWORD>, NTempest::CMemBlock), Storm
//  allocation type-descriptor strings for terrain/UI-console records
//  ("Auwedbfile...", "Auwedbfield...", "Avc2vectorNtem...",
//  "Avc3vectorNtem...", "Avcimvectornte...", "aG_4", "aI_4"), the
//  ".\\CONSOLE.CPP" Storm allocation tag, or an SEH-framed
//  vtable-stamping/field-teardown shape identical to
//  citemclosure_terrain_gates.cpp's own CWEDatabase/CCliffDB/CTerrainDB
//  entries - none of it item-positioning or selection logic.  Same
//  self-redirect-thunk treatment for the same reason: 0% instruction match
//  by construction, and not worth chasing further.
//
//  Declared signatures below exist only to give thunk_abi_audit.py the
//  right `retn <n>` - the byte count read straight off each dump's own
//  trailing `retn` line - not to claim a real calling convention: nothing
//  in src/ calls any of these (grep says zero references outside this
//  file), so the declaration only has to compile and clean the right
//  number of stack bytes.  `__stdcall` with N plain int parameters is the
//  simplest spelling that cleans exactly 4*N bytes regardless of what the
//  shipped body's real arguments turn out to be, the same convention
//  citemclosure_terrain_gates.cpp already uses.
//============================================================================

//----------------------------------------------------------------------------
//  0x6F6F5770 - grows a Storm-backed array of 0x40-byte elements tagged
//  "AutileSetInfo..." and copies a caller-supplied element into every new
//  slot (a broadcast-fill grow, not a plain SetAlloc).  `retn 4`.
//----------------------------------------------------------------------------
__declspec(naked) void __stdcall GrowAndFillAutileSetInfoArray(int)
{
    __asm
    {
        mov eax, 06F6F5770h
        jmp eax
    }
}

//----------------------------------------------------------------------------
//  0x6F6FC010 - same "AutileSetInfo..."-tagged grow-and-fill shape as
//  GrowAndFillAutileSetInfoArray, over a different array (the source
//  element pointer arrives as a fourth stack argument here instead of being
//  fixed).  `retn 8`.
//----------------------------------------------------------------------------
__declspec(naked) void __stdcall GrowAndFillAutileSetInfoArrayVariant(int, int)
{
    __asm
    {
        mov eax, 06F6FC010h
        jmp eax
    }
}

//----------------------------------------------------------------------------
//  0x6F701000 - grows a 0x6C-byte-element ring buffer by one (via
//  sub_6F6F55D0/sub_6F6F5630, a modulus-wrapped index helper this call
//  tree does not otherwise reach) and constructs the new slot through
//  sub_6F6F8D80, then registers it with Storm_501.  Called from
//  ReadTerrainStatConfig-shaped CCliffDB/CWaterDB field bootstraps in this
//  session's own frontier (0x6F70F790, 0x6F70E250, below).  `retn 8`.
//----------------------------------------------------------------------------
__declspec(naked) void __stdcall RegisterTerrainFieldBinding(int, int)
{
    __asm
    {
        mov eax, 06F701000h
        jmp eax
    }
}

//----------------------------------------------------------------------------
//  0x6F73E9F0 - fills a sub-range `[start, min(start+count, this[+0x1C]))`
//  of an 8-byte-element array at `this+0x0C` by repeating one caller-
//  supplied 8-byte source value across every slot in range (the source
//  pointer is read once, outside the loop) - a generic bounded-fill helper
//  over an unidentified array class, not CItem/CSelectable state.
//  `retn 0Ch`.
//----------------------------------------------------------------------------
__declspec(naked) void __stdcall FillRepeated8ByteElementRange(int, int, int)
{
    __asm
    {
        mov eax, 06F73E9F0h
        jmp eax
    }
}

//----------------------------------------------------------------------------
//  0x6F70D290 - stamps
//  `??_7?$TSHashTable@UTerrainTileHash@@VHASHKEY_DWORD@@@@6B@` -
//  TSHashTable<TerrainTileHash,HASHKEY_DWORD>'s own constructor.  `retn 0`.
//----------------------------------------------------------------------------
__declspec(naked) void __cdecl ConstructTerrainTileHashTable()
{
    __asm
    {
        mov eax, 06F70D290h
        jmp eax
    }
}

//----------------------------------------------------------------------------
//  0x6F70DBB0 - SEH-framed destructor tearing down the same
//  TSHashTable<TerrainTileHash,HASHKEY_DWORD> (unlinks its own node list,
//  calls sub_6F703800).  `retn 0`.
//----------------------------------------------------------------------------
__declspec(naked) void __cdecl DestroyTerrainTileHashTable()
{
    __asm
    {
        mov eax, 06F70DBB0h
        jmp eax
    }
}

//----------------------------------------------------------------------------
//  0x6F70DEE0 - SEH-framed constructor stamping `??_7CWEDatabase@@6B@` and
//  registering two named field bindings via RegisterTerrainFieldBinding
//  (above) and sub_6F4CB790/sub_6F4CA130 - the World Editor database base
//  class every one of this session's own CCliffDB/CWaterDB entries builds
//  on top of.  `retn 8`.
//----------------------------------------------------------------------------
__declspec(naked) void __stdcall ConstructWEDatabase(int, int)
{
    __asm
    {
        mov eax, 06F70DEE0h
        jmp eax
    }
}

//----------------------------------------------------------------------------
//  0x6F70DFD0 - SEH-framed CWEDatabase teardown: releases two
//  "Auwedbfile.../Auwedbfield..."-tagged blocks and calls sub_6F706920/
//  sub_6F70DAD0.  `retn 0`.
//----------------------------------------------------------------------------
__declspec(naked) void __cdecl DestroyWEDatabase()
{
    __asm
    {
        mov eax, 06F70DFD0h
        jmp eax
    }
}

//----------------------------------------------------------------------------
//  0x6F711BB0 - releases an "aI_4"-tagged block, then for each of `eax`
//  entries (sub_6F6F1A20-derived count) looks one up by index
//  (LookupOrReleaseIndexedRecord, citemclosure_terrain_gates.cpp) and
//  appends it to a growable index array (sub_6F02C530/sub_6F02C490, this
//  session's own frontier of the Containers array-grow pair).  `retn 8`.
//----------------------------------------------------------------------------
__declspec(naked) void __stdcall RebuildIndexedRecordArray(int, int)
{
    __asm
    {
        mov eax, 06F711BB0h
        jmp eax
    }
}

//----------------------------------------------------------------------------
//  0x6F71AAC0 - if `g_pDefaultConfig` (dword_6FADB0A4, Config/
//  gamedefaultconfig.h) carries the caller-supplied section name at all
//  (CConfigFile::HasSection, 0x6F4D21A0, config.h), walks every entry in it
//  through a caller-supplied callback (sub_6F4D2150, a config-section
//  tree-walk this call tree does not otherwise reach).  `retn 4`.
//----------------------------------------------------------------------------
__declspec(naked) void __stdcall ForEachConfigEntryInSectionIfPresent(int)
{
    __asm
    {
        mov eax, 06F71AAC0h
        jmp eax
    }
}

//----------------------------------------------------------------------------
//  0x6F4B38B0 - a *polymorphic* `NTempest::CMemBlock` constructor: stamps
//  `??_7CMemBlock@NTempest@@6B@` and calls CMemBlock::Init
//  (Pathfinding/cellbuffer.cpp).  Same reasoning as that file's own
//  0x6F4B3770 (ReleaseCMemBlockBase) - this repo has never built a vtable
//  for NTempest::CMemBlock, so the stamp is reproduced as a literal `mov`
//  in a naked, self-redirecting body rather than invented C++.  `retn 10h`.
//----------------------------------------------------------------------------
__declspec(naked) void __stdcall ConstructPolymorphicCMemBlock(int, int, int, int)
{
    __asm
    {
        mov eax, 06F4B38B0h
        jmp eax
    }
}

//----------------------------------------------------------------------------
//  0x6F740000 - a raw 17-entry jump table keyed on a small integer that
//  arrives in `eax` (not on the stack or in ecx/edx - a calling convention
//  no MSVC declaration spells), returning one of seventeen internal jump
//  targets inside a much larger, unreconstructed state machine
//  (sub_6F73FEF0 and its own internal `loc_` labels), or 0 out of range.
//  Nothing in src/ calls this, so the declaration only has to give the
//  right `retn` (none - 0 stack bytes) rather than a real signature.
//----------------------------------------------------------------------------
__declspec(naked) void __cdecl GetStateMachineJumpTargetByIndex()
{
    __asm
    {
        mov eax, 06F740000h
        jmp eax
    }
}

//----------------------------------------------------------------------------
//  0x6F7B1380 - hashes a caller-supplied 64-bit value (via Storm_580 and
//  sub_6F7B0AC0/sub_6F7AF440) and looks the result up in a global console
//  registry (dword_6FADBECC) through its own vtable slot 0 - console-
//  variable lookup plumbing, not game logic.  `retn 0`.
//----------------------------------------------------------------------------
__declspec(naked) void __fastcall LookupConsoleVariableByHash(int, int)
{
    __asm
    {
        mov eax, 06F7B1380h
        jmp eax
    }
}

//----------------------------------------------------------------------------
//  0x6F7B2BE0 - constructs a console-variable record: hashes a name
//  (sub_6F7B2880), copies up to three "General"-defaulted string fields via
//  a ".\\CONSOLE.CPP"-tagged Storm allocation (sub_6F7B2B90) and stores a
//  numeric default - genuine engine console plumbing, the constructor
//  citemclosure_terrain_gates.cpp's own RegisterConsoleConfigVariable
//  already calls.  `retn 1Ch`.
//----------------------------------------------------------------------------
__declspec(naked) void __fastcall ConstructConsoleVariableRecord(
        int, int, int, int, int, int, int, int, int)
{
    __asm
    {
        mov eax, 06F7B2BE0h
        jmp eax
    }
}

//----------------------------------------------------------------------------
//  0x6F748250 - copies a caller-supplied byte range into one of two fixed
//  512/1024-slot terrain lookup tables (chosen by a flag at `this+0x21E4`)
//  via sub_6F7477A0/sub_6F7475D0, index scaled by `eax`.  `retn 10h`.
//----------------------------------------------------------------------------
__declspec(naked) void __stdcall WriteTerrainLookupTableEntry(int, int, int, int)
{
    __asm
    {
        mov eax, 06F748250h
        jmp eax
    }
}

//----------------------------------------------------------------------------
//  0x6F74AA40 - per-element destructor for the "AuTerrainGridMesh..."-
//  tagged terrain array ReleaseTerrainGridMeshArray
//  (citemclosure_terrain_gates.cpp) walks: releases seven "aG_4"/
//  "Avc2vectorNtem..."/"Avc3vectorNtem..."-tagged fields at fixed offsets.
//  `retn 0`.
//----------------------------------------------------------------------------
__declspec(naked) void __cdecl DestroyTerrainGridMeshElement()
{
    __asm
    {
        mov eax, 06F74AA40h
        jmp eax
    }
}

//----------------------------------------------------------------------------
//  0x6F74AB00 - same shape as DestroyTerrainGridMeshElement, for the
//  "AuTerrainImage..."-tagged array ReleaseTerrainImageArray
//  (citemclosure_terrain_gates.cpp) walks: five fields tagged "aG_4"/
//  "Avcimvectornte..."/"Avc2vectorNtem..."/"Avc3vectorNtem...".  `retn 0`.
//----------------------------------------------------------------------------
__declspec(naked) void __cdecl DestroyTerrainImageElement()
{
    __asm
    {
        mov eax, 06F74AB00h
        jmp eax
    }
}

//----------------------------------------------------------------------------
//  0x6F758890 - SEH-framed per-element destructor for the
//  "AuTerrainModel..."-tagged array ReleaseTerrainModelArray
//  (citemclosure_terrain_gates.cpp) walks: releases an "aG_4"/"aI_4"-tagged
//  field pair and tears down two sub-objects via sub_6F7537F0/sub_6F7447E0/
//  sub_6F74FD10.  `retn 0`.
//----------------------------------------------------------------------------
__declspec(naked) void __cdecl DestroyTerrainModelElement()
{
    __asm
    {
        mov eax, 06F758890h
        jmp eax
    }
}

//----------------------------------------------------------------------------
//  0x6F763BD0 - per-element field clearer ReleaseTerrainVectorFieldArray
//  (citemclosure_terrain_gates.cpp) calls: releases eight fields tagged
//  "Avc3vectorNtem..."/"Avc2vectorNtem..."/"aG_4" and zeroes the triples
//  behind each.  `retn 4`.
//----------------------------------------------------------------------------
__declspec(naked) void __stdcall ClearTerrainVectorFieldElement(int)
{
    __asm
    {
        mov eax, 06F763BD0h
        jmp eax
    }
}

//----------------------------------------------------------------------------
//  0x6F785B60 - SEH-framed per-element flag refresher
//  RefreshTerrainLayerFlags (citemclosure_terrain_gates.cpp) calls: formats
//  a scratch buffer via sub_6F71C1D0/sub_6F785220 and, unless the result is
//  empty or `"-"`, copies a nested field and calls sub_6F7857E0 through the
//  array's own `this[0]`.  `retn 8`.
//----------------------------------------------------------------------------
__declspec(naked) void __stdcall RefreshTerrainLayerFlagElement(int, int)
{
    __asm
    {
        mov eax, 06F785B60h
        jmp eax
    }
}

//----------------------------------------------------------------------------
//  0x6F711630 - SEH-framed "INDEX_%04d"-style lookup-key formatter
//  LookupOrReleaseIndexedRecord (citemclosure_terrain_gates.cpp) calls:
//  builds the key with Storm_578, looks it up via sub_6F706B40, and on a
//  miss creates a new tagged record through sub_6F4C9D20/Storm_590/
//  sub_6F710950, formatting an "e:\\Drive1\\temp\\buildwar3x\\Storm\\H..."
//  build-path literal into it via Storm_507 - a hard-coded build-machine
//  path, not anything CItem reads.  `retn 4`.
//----------------------------------------------------------------------------
__declspec(naked) void __stdcall FormatIndexLookupKey(int)
{
    __asm
    {
        mov eax, 06F711630h
        jmp eax
    }
}

//----------------------------------------------------------------------------
//  0x6F70F790 - SEH-framed CCliffDB constructor: calls ConstructWEDatabase
//  (above) for its base subobject, stamps `??_7CCliffDB@@6B@`, then walks a
//  fixed table of "cliffModelDir".."DestructableID"-range field-name
//  literals (off_6F97B190) through RegisterTerrainFieldBinding (above).
//  `retn 0`.
//----------------------------------------------------------------------------
__declspec(naked) void __cdecl ConstructCliffDB()
{
    __asm
    {
        mov eax, 06F70F790h
        jmp eax
    }
}

//----------------------------------------------------------------------------
//  0x6F6F0C90 - formats `"%s\\%s"` (Storm_578) out of a name looked up
//  through the object's own vtable slot 5 with a fixed selector of 0 -
//  path-building for a terrain sub-resource, reached only from this
//  session's own CCliffDB/CWaterDB-style constructors.  `retn 0Ch`.
//----------------------------------------------------------------------------
__declspec(naked) void __stdcall FormatTerrainSubpathSelector0(int, int, int)
{
    __asm
    {
        mov eax, 06F6F0C90h
        jmp eax
    }
}

//----------------------------------------------------------------------------
//  0x6F6F0D00 - same shape as FormatTerrainSubpathSelector0 with a fixed
//  selector of 1 instead of 0.  `retn 0Ch`.
//----------------------------------------------------------------------------
__declspec(naked) void __stdcall FormatTerrainSubpathSelector1(int, int, int)
{
    __asm
    {
        mov eax, 06F6F0D00h
        jmp eax
    }
}

//----------------------------------------------------------------------------
//  0x6F6F8660 - SEH-framed, self-recursive tag classifier: looks a name up
//  through the object's own vtable slot 5 with a selector derived from a
//  caller-supplied flag, and depending on the last character read
//  (`\0`/`'_'`/other) either packs the tag (PackFourBytesBigEndianSigned,
//  citemclosure_terrain_leaves.cpp), returns 0, or recurses once with the
//  flag flipped.  `retn 8`.
//----------------------------------------------------------------------------
__declspec(naked) void __stdcall ClassifyAndPackTerrainTag(int, int)
{
    __asm
    {
        mov eax, 06F6F8660h
        jmp eax
    }
}

//----------------------------------------------------------------------------
//  0x6F6F8E30 - SEH-framed: builds a lookup key (sub_6F6F19C0), resolves an
//  entry through it (sub_6F6F8D50) and releases the result through
//  sub_6F4CA120 - a lookup-then-release combinator over the same
//  unidentified terrain registry ClassifyAndPackTerrainTag reads.
//  `retn 8`.
//----------------------------------------------------------------------------
__declspec(naked) void __stdcall LookupAndReleaseTerrainEntry(int, int)
{
    __asm
    {
        mov eax, 06F6F8E30h
        jmp eax
    }
}

//----------------------------------------------------------------------------
//  0x6F712FD0 - formats a `".blp"`-suffixed `"%s\\%s%s"` path (Storm_578)
//  out of two names looked up through `this+0x190`'s own record fields at
//  +0x144/+0xD8, then releases the intermediate lookup object through
//  sub_6F6F1A00 (citemclosure_terrain_gates.cpp's ReleaseIfNotSuppressed
//  sibling).  `retn 0Ch`.
//----------------------------------------------------------------------------
__declspec(naked) void __stdcall FormatBlpPathFromFields0x144_0xD8(int, int, int)
{
    __asm
    {
        mov eax, 06F712FD0h
        jmp eax
    }
}

//----------------------------------------------------------------------------
//  0x6F711CD0 - same `".blp"`-suffixed `"%s\\%s%s"`-formatting shape as
//  FormatBlpPathFromFields0x144_0xD8, over fields at +0x6C/+0x00 instead.
//  `retn 0Ch`.
//----------------------------------------------------------------------------
__declspec(naked) void __stdcall FormatBlpPathFromFields0x6C_0x00(int, int, int)
{
    __asm
    {
        mov eax, 06F711CD0h
        jmp eax
    }
}

//----------------------------------------------------------------------------
//  0x6F764C50 / 0x6F764CF0 / 0x6F764D90 - three siblings of one
//  hash-table-insert shape: compute a bucket (sub_6F7629E0/sub_6F762A80/
//  sub_6F762B20 - a lazily-seeded hash, each initialised on first use by
//  sub_6F760340/sub_6F7603A0/sub_6F760400), call a caller-supplied
//  key-compare through the table's own vtable slot 7, and splice the
//  result into the table's doubly-linked bucket list.  Three distinct
//  hash/compare pairs, so three distinct tables rather than one template
//  instantiation this repo already has a name for.  All three `retn 0Ch`.
//----------------------------------------------------------------------------
__declspec(naked) void __stdcall InsertIntoTerrainHashTableA(int, int, int)
{
    __asm
    {
        mov eax, 06F764C50h
        jmp eax
    }
}

__declspec(naked) void __stdcall InsertIntoTerrainHashTableB(int, int, int)
{
    __asm
    {
        mov eax, 06F764CF0h
        jmp eax
    }
}

__declspec(naked) void __stdcall InsertIntoTerrainHashTableC(int, int, int)
{
    __asm
    {
        mov eax, 06F764D90h
        jmp eax
    }
}

//----------------------------------------------------------------------------
//  0x6F70E250 - SEH-framed CWaterDB constructor: calls ConstructWEDatabase
//  (above) for its base subobject, stamps `??_7CWaterDB@@6B@`, then walks
//  a fixed "height"-starting field-name table (off_6F97B4E0) through
//  RegisterTerrainFieldBinding (above) - the CCliffDB-shaped sibling
//  ConstructCliffDB above already documents.  `retn 0`.
//----------------------------------------------------------------------------
__declspec(naked) void __cdecl ConstructWaterDB()
{
    __asm
    {
        mov eax, 06F70E250h
        jmp eax
    }
}

//----------------------------------------------------------------------------
//  0x6F6FA0B0 / 0x6F6FA100 / 0x6F6FA150 - three siblings filling a caller-
//  supplied 3-float (Vec3) output by calling the object's own vtable slot 7
//  three times with consecutive field indices (9/10/11, 12/13/14, 15/16/17
//  respectively) - a generic named-field Vec3 accessor over an unidentified
//  field-index interface, not CItem/CSelectable state.  All three
//  `retn 8`.
//----------------------------------------------------------------------------
__declspec(naked) void __stdcall GetIndexedVec3Fields9_10_11(int, int)
{
    __asm
    {
        mov eax, 06F6FA0B0h
        jmp eax
    }
}

__declspec(naked) void __stdcall GetIndexedVec3Fields12_13_14(int, int)
{
    __asm
    {
        mov eax, 06F6FA100h
        jmp eax
    }
}

__declspec(naked) void __stdcall GetIndexedVec3Fields15_16_17(int, int)
{
    __asm
    {
        mov eax, 06F6FA150h
        jmp eax
    }
}

//----------------------------------------------------------------------------
//  0x6F6FA1A0 - SEH-framed: looks a name up through the object's own vtable
//  slot 5 (selector 0x15) and, on success, formats an `".mdl"`-suffixed
//  `"%s\\%s\\%s%1d%s"` model path (Storm_578) using an error-code string
//  chosen from a caller-supplied 0..2 range - model-path plumbing over the
//  same unidentified terrain registry as GetIndexedVec3Fields9_10_11.
//  `retn 14h`.
//----------------------------------------------------------------------------
__declspec(naked) void __stdcall FormatTerrainModelPathWithErrorCode(
        int, int, int, int, int)
{
    __asm
    {
        mov eax, 06F6FA1A0h
        jmp eax
    }
}
