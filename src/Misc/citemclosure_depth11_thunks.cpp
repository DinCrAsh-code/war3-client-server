//============================================================================
//  BFS-depth-11 descendants of CItem's vtable slots 32/104 (same frontier
//  as Misc/citemclosure_terrain_gates.cpp and its siblings) confirmed
//  unrelated to CItem/CSelectable by their own evidence: a
//  "AvTsexplicitli_NN"-tagged bucket-array teardown, an ambiguous
//  vtable-slot-0 dispatch, an uncertain SetAlloc, and two NTempest
//  terrain-array releases each freeing a per-element sub-block plus a
//  second, whole-array field.  (0x6F4C9340/0x6F4C9410/0x6F4CA370, the three
//  addresses this batch's own worklist also names, turned out to already
//  be reconstructed under better evidence-based names -
//  Misc/slklineparser.cpp's own ParseSlkCellRecord/ParseSlkFormatRecord/
//  ParseSlkBoundsRecord - so nothing for them lives here.)  Same convention
//  as those files: a direct `mov eax,ADDR / jmp eax` redirect into the
//  game's own code at its real address, so a build hooking anything else in
//  this target still calls the genuine original rather than link-failing
//  on an unresolved symbol.  0% instruction match by construction.
//
//  Declared signatures exist only to give thunk_abi_audit.py the right
//  `retn <n>` - the byte count read straight off each dump's own trailing
//  `retn` line - not to claim a real calling convention: nothing in src/
//  calls any of these (grep says zero references outside this file).
//============================================================================

#define ADDR_THUNK(sig, addr)                     \
    __declspec(naked) sig                         \
    {                                             \
        __asm { mov eax, addr }                   \
        __asm { jmp eax }                         \
    }

//----------------------------------------------------------------------------
//  0x6F4C9780 - walks a `TSGrowableArray<TSExplicitList<T>>`-shaped bucket
//  array (Containers/tsarray.inl/tslist.inl's own generic shape, same
//  member offsets as citemclosure_terrainhash4.cpp's own table): when the
//  caller's one stack argument is zero, releases every node through this
//  object's own vtable slot 0 instead of just unlinking it - the same
//  "detach vs. destroy" fork Misc/citemclosure_database_thunks.cpp's own
//  SWEDBEntryHashTableThunk::Clear documents for its sibling table, and the
//  same reason this stays a redirect rather than a hand-written vtable
//  dispatch: nothing in this dump resolves slot 0's real signature.
//  `retn 4`.
//----------------------------------------------------------------------------
struct SWEDBFieldHashPurgeThunk { void Purge(int destroyNodes); };
ADDR_THUNK(void SWEDBFieldHashPurgeThunk::Purge(int destroyNodes), 0x6F4C9780)

//----------------------------------------------------------------------------
//  0x6F4CA240 - walks a table's bucket array (the inline TSLink::Unlink
//  shape once per bucket) then frees the bucket array's own backing block
//  through Storm_403, tagged "AvTsexplicitli_19" - same evidence shape as
//  Misc/citemclosure_terrain_gates.cpp's own ReleaseTerrainGridMeshLinkedNodes.
//  `retn 0`.
//----------------------------------------------------------------------------
__declspec(naked) void __cdecl DestroyWEDBFieldBucketArray()
{
    __asm { mov eax, 06F4CA240h }
    __asm { jmp eax }
}

//----------------------------------------------------------------------------
//  0x6F4CA460 - a one-instruction `jmp sub_6F4CA240` in the shipped image:
//  a second entry point for the same destructor body above, not a distinct
//  implementation - same convention as
//  Misc/citemclosure_wedbentryhash.cpp's own TSGrowableArrayDtorAlias_WEDBEntryHash.
//----------------------------------------------------------------------------
__declspec(naked) void DestroyWEDBFieldBucketArrayAlias()
{
    __asm { mov eax, 06F4CA460h }
    __asm { jmp eax }
}

//----------------------------------------------------------------------------
//  0x6F7B08F0 - Containers/citemclosure_terrain_hashtable2.cpp's own
//  STerrainHashRecord3 table's `TSGrowableArray<TSExplicitList<
//  STerrainHashRecord3>>::SetAlloc`: destroys every live bucket past the
//  new size, reallocates through Storm (in-place resize first, alloc/copy/
//  free fallback behind it - Storm/storm.h's own SetAlloc comment), then
//  default-constructs every newly-grown bucket.  Left a redirect rather
//  than the fifth real instantiation alongside that file's own UnlinkAll/
//  ComputeChunk/SetCount/Rehash: its own per-bucket destructor call
//  (`sub_6F7AFB20`) is not itself in this batch's dump, so nothing here
//  confirms it is UnlinkAll rather than some other member, and getting that
//  wrong would silently mis-shape the reconstruction with no score penalty
//  to catch it.  `retn 4`.
//----------------------------------------------------------------------------
struct SGrowTerrainHashBucketArrayThunk { void SetAlloc(unsigned int newAlloc); };
ADDR_THUNK(void SGrowTerrainHashBucketArrayThunk::SetAlloc(unsigned int newAlloc),
           0x6F7B08F0)

//----------------------------------------------------------------------------
//  0x6F74FC50 - per-element loop over a `count`-at-+4/`data`-at-+8,
//  0x18-byte-stride array releasing a sub-block at each element's own +8
//  through Storm_403 (tagged "AvC2VectorNtem..." - an embedded
//  TSGrowableArray<NTempest::C2Vector>, same evidence shape
//  Misc/citemclosure_assign_c2vector.cpp's own Assign instantiation
//  confirms the tag for), then - if a second field at +0 is over 1 - frees
//  a second, whole-array block tagged "AuCliffTexture...".  `retn 0`.
//----------------------------------------------------------------------------
__declspec(naked) void __cdecl ReleaseCliffTextureVectorArray()
{
    __asm { mov eax, 06F74FC50h }
    __asm { jmp eax }
}

//----------------------------------------------------------------------------
//  0x6F74FCB0 - the identical loop shape as ReleaseCliffTextureVectorArray,
//  over the same "aG_4"-tagged 2-byte element
//  Misc/citemclosure_assign_field2.cpp's own Assign instantiation reaches,
//  then a second whole-array free tagged "AuCliffPrim...".  `retn 0`.
//----------------------------------------------------------------------------
__declspec(naked) void __cdecl ReleaseCliffPrimFieldArray()
{
    __asm { mov eax, 06F74FCB0h }
    __asm { jmp eax }
}
