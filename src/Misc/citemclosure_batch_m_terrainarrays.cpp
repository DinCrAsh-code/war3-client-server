//============================================================================
//  CItem-vtable-closure batch M: two more TSGrowableArray<TSExplicitList<T>>
//  ComputeChunk/SetAlloc pairs, over node types this batch's own dumps never
//  name - same caution as tools/funcmap.py's own
//  SGrowTerrainHashBucketArrayThunk::SetAlloc entry (Misc/
//  citemclosure_depth11_thunks.cpp): each pair's own per-element destructor
//  callee (sub_6F72C740 / sub_6F7AFA40) is not itself in this batch's scope,
//  so nothing here confirms it is the generic TSExplicitList<T>::~
//  TSExplicitList() shape rather than some other member - a real template
//  instantiation would be a guess at that, not evidence.  ComputeChunk is
//  type-independent (sizeof(TSExplicitList<T>) is 12 bytes regardless of T)
//  but the pairs are kept together with their own SetAlloc for the same
//  reason citemclosure_terrain_hashtable2.cpp's own pairs are: a table's
//  ComputeChunk and SetAlloc are always this same neighbouring-address
//  shape, and a lone ComputeChunk with no SetAlloc alongside it would read
//  as one table's growth policy with no table.
//
//  Same 0% instruction match by construction as every other redirect at
//  this depth - see each BEHAVIOUR entry in tools/funcmap.py.
//============================================================================

#define TARRAY_THUNK(sig, addr)        \
    __declspec(naked) sig              \
    {                                  \
        __asm { mov eax, addr }        \
        __asm { jmp eax }              \
    }

//----------------------------------------------------------------------------
//  0x6F726100 / 0x6F7308E0 - a first unconfirmed TSGrowableArray<
//  TSExplicitList<T> >'s ComputeChunk/SetAlloc pair (17 / 115 instructions,
//  the same generic shapes Containers/tsarray.inl defines).  Both `retn 4`.
//----------------------------------------------------------------------------
TARRAY_THUNK(void __stdcall TerrainArrayGroup1_ComputeChunk(int), 0x6F726100)
TARRAY_THUNK(void __stdcall TerrainArrayGroup1_SetAlloc(int), 0x6F7308E0)

//----------------------------------------------------------------------------
//  0x6F7AEF60 / 0x6F7B0670 - a second unconfirmed pair, the identical shape
//  again at a different address (so a distinct instantiation, not an alias
//  of the first - a sibling of the other one, not an invented shared
//  identity, same convention as citemclosure_terrain_gates.cpp's own
//  CreateTerrainRecordByFourCC1/2/3).  Both `retn 4`.
//----------------------------------------------------------------------------
TARRAY_THUNK(void __stdcall TerrainArrayGroup2_ComputeChunk(int), 0x6F7AEF60)
TARRAY_THUNK(void __stdcall TerrainArrayGroup2_SetAlloc(int), 0x6F7B0670)

#undef TARRAY_THUNK
