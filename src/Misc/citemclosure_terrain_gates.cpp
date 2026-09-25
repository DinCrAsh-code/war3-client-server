//============================================================================
//  A frontier of deep NTempest/Storm terrain-rendering internals, reached
//  only by the BFS walk out of CItem's vtable slots 32/104
//  (CItem::ClearPendingAndNotify / CSelectable::RefreshAttachmentPlacement -
//  asm/processed/CItem__Method_0x80_0x6F2B6130_calltree_asm.md) wandering
//  into an unrelated engine subsystem at depth 5, the same way
//  Jass/jassstormterraingates.cpp's own frontier did out of
//  ConstructStormSingletonA.  Every one of these is confirmed unrelated to
//  CItem/CSelectable by its own evidence: NTempest-namespaced vtables
//  (CTerrainDB, CDynTable<TerrainLayerPriority>), terrain/config type
//  descriptor and key strings ("gridhl", "Terrain", "terstats",
//  "FogOfWar"/"FoggedTerrain", "AUOBJECTCHANCE", "AuTerrainGridMesh",
//  "AuTerrainImage", "AuTerrainModel", "AuCliffDoodad", "AuWaterLevelInfo",
//  "AuDoodadPathing", ...), or an SEH-framed lazy-singleton-init shape
//  identical to jassstormterraingates.cpp's own StormTerrainSubsystemInitGate
//  - none of it item-positioning or selection logic, all of it exactly the
//  "deep, non-interesting chunk" CLAUDE.md's own "work outward... not into
//  Storm's internals" already says to stop at.  Same pattern as that file:
//  a direct `mov eax,ADDR / jmp eax` redirect into the game's own code at
//  its real address, so a build hooking anything else in this target still
//  calls the genuine original rather than link-failing on an unresolved
//  symbol.  0% instruction match by construction and not worth chasing
//  further - see each BEHAVIOUR entry in tools/funcmap.py.
//
//  Declared signatures below exist only to give thunk_abi_audit.py the
//  right `retn <n>` - the byte count read straight off each dump's own
//  trailing `retn` line - not to claim a real calling convention: nothing
//  in src/ calls any of these (grep says zero references outside this
//  file), so the declaration only has to compile and clean the right
//  number of stack bytes.  `__stdcall` with N plain int parameters is the
//  simplest spelling that cleans exactly 4*N bytes regardless of what the
//  shipped body's real arguments turn out to be.
//============================================================================

//----------------------------------------------------------------------------
//  0x6F713380 - stamps `??_7CTerrainDB@@6B@` in its own first instructions
//  (SEH-framed, 219 lines) - CTerrainDB's own constructor.  `retn 0`.
//----------------------------------------------------------------------------
__declspec(naked) void __cdecl ConstructTerrainDB()
{
    __asm
    {
        mov eax, 06F713380h
        jmp eax
    }
}

//----------------------------------------------------------------------------
//  0x6F746460 - grows a CMemBlock-backed buffer (calls CMemBlock::SetSize,
//  0x6F4B39C0, Pathfinding/cellbuffer.cpp) and inserts bytes via an
//  unreached callee (sub_6F73E9F0).  `retn 8`.
//----------------------------------------------------------------------------
__declspec(naked) void __stdcall GrowAndInsertTerrainBytes(int, int)
{
    __asm
    {
        mov eax, 06F746460h
        jmp eax
    }
}

//----------------------------------------------------------------------------
//  0x6F746510 - stamps
//  `??_7?$CDynTable@VTerrainLayerPriority@@@NTempest@@6B@` - the
//  NTempest::CDynTable<TerrainLayerPriority> constructor, calling
//  sub_6F4B38B0.  `retn 10h`.
//----------------------------------------------------------------------------
__declspec(naked) void __stdcall ConstructTerrainLayerPriorityDynTable(
        int, int, int, int)
{
    __asm
    {
        mov eax, 06F746510h
        jmp eax
    }
}

//----------------------------------------------------------------------------
//  0x6F7400C0 - reads terrain-stats config under "Terrain"/"terstats"
//  (calls RegisterConsoleConfigVariable below, or sub_6F7B1380).  `retn 4`.
//----------------------------------------------------------------------------
__declspec(naked) void __stdcall ReadTerrainStatConfig(int)
{
    __asm
    {
        mov eax, 06F7400C0h
        jmp eax
    }
}

//----------------------------------------------------------------------------
//  0x6F74C280 - registers a "gridhl" console variable (calls
//  RegisterConsoleConfigVariable below).  `retn 0`.
//----------------------------------------------------------------------------
__declspec(naked) void __cdecl RegisterGridHalfLifeConfigVar()
{
    __asm
    {
        mov eax, 06F74C280h
        jmp eax
    }
}

//----------------------------------------------------------------------------
//  0x6F7B2D30 - general console-variable registration: calls the Storm
//  ordinal import Storm_580 and sub_6F7B2BE0, itself a ".\\CONSOLE.CPP"
//  Storm allocation-tagged record constructor - genuine engine console
//  plumbing, not game logic.  `retn 14h`.
//----------------------------------------------------------------------------
__declspec(naked) void __stdcall RegisterConsoleConfigVariable(
        int, int, int, int, int)
{
    __asm
    {
        mov eax, 06F7B2D30h
        jmp eax
    }
}

//----------------------------------------------------------------------------
//  0x6F72A330 - TSGrowableArray<T>::SetAlloc-shaped grow of an
//  "AUOBJECTCHANCE"-tagged array (an 8-byte-element table read by object
//  randomisation config, the same key jassstormterraingates.cpp's own
//  StormTerrainAutoObjectConfigGate note already names).  `retn 4`.
//----------------------------------------------------------------------------
__declspec(naked) void __stdcall GrowAutoObjectChanceArray(int)
{
    __asm
    {
        mov eax, 06F72A330h
        jmp eax
    }
}

//----------------------------------------------------------------------------
//  0x6F74F660 - releases a heightmap-related array; the type descriptor
//  string for the array's own backing block reads "AuHeightMap...".
//  `retn 0`.
//----------------------------------------------------------------------------
__declspec(naked) void __cdecl ReleaseHeightmapHelperArray()
{
    __asm
    {
        mov eax, 06F74F660h
        jmp eax
    }
}

//----------------------------------------------------------------------------
//  0x6F74C480 - reads five "FogOfWar"/"FoggedTerrain"/"BoundaryTerrain"/
//  "FoggedBoundaryTerrain"/"FoggedObject"/"BoundaryObject" config keys via
//  sub_6F72B0B0 (113 lines).  `retn 0`.
//----------------------------------------------------------------------------
__declspec(naked) void __cdecl RegisterFogOfWarConfigVars()
{
    __asm
    {
        mov eax, 06F74C480h
        jmp eax
    }
}

//----------------------------------------------------------------------------
//  0x6F753100 - releases a "AuTerrainGridMesh..."-tagged terrain array,
//  per-element destructor sub_6F74AA40.  `retn 0`.
//----------------------------------------------------------------------------
__declspec(naked) void __cdecl ReleaseTerrainGridMeshArray()
{
    __asm
    {
        mov eax, 06F753100h
        jmp eax
    }
}

//----------------------------------------------------------------------------
//  0x6F753230 - releases an array whose own backing block is tagged
//  "AuCliffDoodad...", per-element cleanup at +0x18 tagged
//  "AvCIMVector...".  `retn 0`.
//----------------------------------------------------------------------------
__declspec(naked) void __cdecl ReleaseCliffDoodadArray()
{
    __asm
    {
        mov eax, 06F753230h
        jmp eax
    }
}

//----------------------------------------------------------------------------
//  0x6F7532A0 - releases a "AuTerrainImage..."-tagged terrain array,
//  per-element destructor sub_6F74AB00.  `retn 0`.
//----------------------------------------------------------------------------
__declspec(naked) void __cdecl ReleaseTerrainImageArray()
{
    __asm
    {
        mov eax, 06F7532A0h
        jmp eax
    }
}

//----------------------------------------------------------------------------
//  0x6F753300 - releases a "PAuHTexture..."-element array and a
//  "AuWaterLevelInfo..."-tagged block.  `retn 0`.
//----------------------------------------------------------------------------
__declspec(naked) void __cdecl ReleaseWaterAndTextureArray()
{
    __asm
    {
        mov eax, 06F753300h
        jmp eax
    }
}

//----------------------------------------------------------------------------
//  0x6F75F220 - releases a "AuTerrainModel..."-tagged terrain array,
//  per-element destructor sub_6F758890.  `retn 0`.
//----------------------------------------------------------------------------
__declspec(naked) void __cdecl ReleaseTerrainModelArray()
{
    __asm
    {
        mov eax, 06F75F220h
        jmp eax
    }
}

//----------------------------------------------------------------------------
//  0x6F752220 - walks a terrain grid-mesh array, calling
//  ReleaseTerrainGridMeshLinkedNodes (below) per element and unlinking a
//  wavefront-style list; conditionally dispatches through a vtable slot 0
//  call.  `retn 4`.
//----------------------------------------------------------------------------
__declspec(naked) void __stdcall ReleaseTerrainGridMeshWavefrontLists(int)
{
    __asm
    {
        mov eax, 06F752220h
        jmp eax
    }
}

//----------------------------------------------------------------------------
//  0x6F75AF20 - per-element loop calling ReleaseTerrainGridMeshLinkedNodes's
//  own callee (0x6F74F230, Containers/terrainlistunlinkall.cpp) then
//  releasing the backing block, tagged "AvTsexplicitli..." (a
//  TSExplicitList<T> instance).  `retn 0`.  0x6F75ED30 is a one-instruction
//  `jmp` alias IDA gives its own name - see Agent/handleobject.cpp's
//  GetHandleOrZeroAlias for the identical shape.
//----------------------------------------------------------------------------
__declspec(naked) void __cdecl ReleaseTerrainGridMeshLinkedNodes()
{
    __asm
    {
        mov eax, 06F75AF20h
        jmp eax
    }
}

__declspec(naked) void __cdecl ReleaseTerrainGridMeshLinkedNodesAlias()
{
    __asm { jmp ReleaseTerrainGridMeshLinkedNodes }
}

//----------------------------------------------------------------------------
//  0x6F7522F0 - same shape as ReleaseTerrainGridMeshWavefrontLists, over the
//  terrain-image array instead.  `retn 4`.
//----------------------------------------------------------------------------
__declspec(naked) void __stdcall ReleaseTerrainImageWavefrontLists(int)
{
    __asm
    {
        mov eax, 06F7522F0h
        jmp eax
    }
}

//----------------------------------------------------------------------------
//  0x6F75AFA0 - same shape as ReleaseTerrainGridMeshLinkedNodes, tagged
//  "AvTsexplicitli_63" instead of "_64".  `retn 0`.  0x6F75ED50 is its
//  one-instruction `jmp` alias, same shape as above.
//----------------------------------------------------------------------------
__declspec(naked) void __cdecl ReleaseTerrainImageLinkedNodes()
{
    __asm
    {
        mov eax, 06F75AFA0h
        jmp eax
    }
}

__declspec(naked) void __cdecl ReleaseTerrainImageLinkedNodesAlias()
{
    __asm { jmp ReleaseTerrainImageLinkedNodes }
}

//----------------------------------------------------------------------------
//  0x6F742910 - loops over a count at [this+0x100], calling sub_6F73B0D0
//  (release-if-set through [elem+0x64], 0x6F4C40A0) per 0x94-byte element
//  at [this+0x104]+i*0x94.  `retn 0`.
//----------------------------------------------------------------------------
__declspec(naked) void __cdecl ReleaseTerrainGridMeshRefArray()
{
    __asm
    {
        mov eax, 06F742910h
        jmp eax
    }
}

//----------------------------------------------------------------------------
//  0x6F74D7C0 - clears three doodad-pathing array/backing-block triples,
//  one tagged "AuDoodadPathing...".  `retn 0`.
//----------------------------------------------------------------------------
__declspec(naked) void __cdecl ClearDoodadPathingArrays()
{
    __asm
    {
        mov eax, 06F74D7C0h
        jmp eax
    }
}

//----------------------------------------------------------------------------
//  0x6F763CF0 - same loop shape as ReleaseTerrainGridMeshRefArray, calling
//  sub_6F763BD0 - a 100+ line NTempest field clearer touching
//  "AvC3VectorNtem"/"AvC2VectorNtem"-tagged sub-vectors - per element.
//  `retn 0`.
//----------------------------------------------------------------------------
__declspec(naked) void __cdecl ReleaseTerrainVectorFieldArray()
{
    __asm
    {
        mov eax, 06F763CF0h
        jmp eax
    }
}

//----------------------------------------------------------------------------
//  0x6F785C10 - walks a terrain-layer array refreshing a per-element flag
//  through sub_6F785B60 when a bitmask test at +0x28 passes.  `retn 4`.
//----------------------------------------------------------------------------
__declspec(naked) void __stdcall RefreshTerrainLayerFlags(int)
{
    __asm
    {
        mov eax, 06F785C10h
        jmp eax
    }
}

//----------------------------------------------------------------------------
//  0x6F711720 - formats an "INDEX_%04d"-keyed lookup (sub_6F711630, 78
//  lines) and either looks up an existing record (sub_6F4C8820 -
//  Agent/queryfield.cpp's QueryTaggedField) or releases it
//  (sub_6F6F1A00/sub_6F6F19E0 - Agent/vslot4_refcnt_forwards.cpp's
//  ReleaseIfNotSuppressed and a sibling this call tree does not reach the
//  body of).  `retn 4`.
//----------------------------------------------------------------------------
__declspec(naked) void __stdcall LookupOrReleaseIndexedRecord(int)
{
    __asm
    {
        mov eax, 06F711720h
        jmp eax
    }
}

//----------------------------------------------------------------------------
//  0x6F71C260 / 0x6F71C380 - two more SEH-framed lazy-singleton-init gates,
//  byte-identical in shape to Jass/jassstormterraingates.cpp's own
//  StormTerrainSubsystemInitGate (0x6F71C2F0, right between these two
//  numerically) - the cookie-XORed, frame-pointer-omitted SEH frame
//  docs/msvc-vc8-idioms.md records as unreproducible at this repo's fixed
//  `/GS- /EHs-c-`.  Both `retn 0`.
//----------------------------------------------------------------------------
__declspec(naked) void __cdecl EnsureTerrainSubsystemA()
{
    __asm
    {
        mov eax, 06F71C260h
        jmp eax
    }
}

__declspec(naked) void __cdecl EnsureTerrainSubsystemB()
{
    __asm
    {
        mov eax, 06F71C380h
        jmp eax
    }
}

//----------------------------------------------------------------------------
//  0x6F767B80 / 0x6F767CE0 / 0x6F767E20 - three siblings that hash a tag
//  through FourCC__Hash (AGILE_TYPE_ID::Hash - tools/funcmap.py), allocate a
//  fixed-size record and copy several config-style string fields into it via
//  sub_6F712FD0/sub_6F6F0C90/.../sub_6F711CD0/sub_6F6F1E10 and siblings -
//  engine record/cache construction, not CItem state.  All three `retn 0`.
//----------------------------------------------------------------------------
__declspec(naked) void __cdecl CreateTerrainRecordByFourCC1()
{
    __asm
    {
        mov eax, 06F767B80h
        jmp eax
    }
}

__declspec(naked) void __cdecl CreateTerrainRecordByFourCC2()
{
    __asm
    {
        mov eax, 06F767CE0h
        jmp eax
    }
}

__declspec(naked) void __cdecl CreateTerrainRecordByFourCC3()
{
    __asm
    {
        mov eax, 06F767E20h
        jmp eax
    }
}
