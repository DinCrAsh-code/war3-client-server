//============================================================================
//  More of the same deep NTempest/Storm terrain-rendering frontier
//  Misc/citemclosure_terrain_gates.cpp and citemclosure_terrain_gates2.cpp
//  already document (reached from CItem's vtable BFS, batch 8 of 8 - see
//  docs/targets/CItem-callee-batch8.md), landing in the identical
//  0x6F74xxxx-0x6F76xxxx neighbourhood those two files already cover for
//  "AuTerrainImage"/"AuTerrainGridMesh"-tagged arrays
//  (ReleaseTerrainImageArray at 0x6F7532A0, per-element destructor
//  sub_6F74AB00/DestroyTerrainImageElement at 0x6F74AB00 - Containers/
//  tools/funcmap.py already names it a THUNK for exactly this reason).
//  These eight are the generic-container plumbing *around* that same
//  "AuTerrainImage" array/hash-table pair: a TSGrowableArray<T>::SetAlloc
//  (0x6F759170, same "AuTerrainImage" tag) and its element copy-constructor
//  (0x6F7543D0, which is what feeds DestroyTerrainImageElement its matching
//  live objects), plus a TSHashTable<T,K>-shaped Initialize/SetCount/Rehash/
//  CheckGrow/NewNode/Ptr quintet (0x6F758670/0x6F760B30/0x6F762D40/
//  0x6F762F70/0x6F765000/0x6F7674A0) whose element type is never
//  established anywhere in this closure - the generic Containers/tshash.inl
//  shape confirms *which* template this is, not *which* instantiation, and
//  guessing the missing T/K here would be inventing a layout rather than
//  confirming one, exactly the situation Widget/soundplaytablething.cpp's
//  own SSoundPlayTableThunk::Ptr already documents for an unrelated
//  instantiation of the identical template.  None of it is item-positioning
//  or selection logic; all of it is exactly the "deep, non-interesting
//  chunk" CLAUDE.md's own "work outward... not into Storm's internals"
//  already says to stop at.  Same pattern as the two sibling files: a direct
//  redirect into the game's own code at its real address.  0% instruction
//  match by construction - see each BEHAVIOUR entry in tools/funcmap.py.
//
//  The ninth (0x6F741250) is a ~250-instruction FPU block computing a
//  terrain-normal-style plane fit (three cross products feeding
//  FVec3::Normalize, Math/CFloat.h's own 0x6F011550) reached only from this
//  same terrain frontier - large, self-contained, and not worth the
//  reconstruction budget relative to the rest of this batch; thunked for
//  the same "not worth chasing further" reason as the array/table plumbing
//  above, not because its layout is unclear.
//
//  Declared signatures below exist only to give thunk_abi_audit.py the
//  right `retn <n>` off each dump's own trailing `retn` line; nothing in
//  src/ calls any of these (grep says zero references outside this file).
//============================================================================

#define ADDR_THUNK(sig, addr)                     \
    __declspec(naked) sig                         \
    {                                             \
        __asm { mov eax, addr }                   \
        __asm { jmp eax }                         \
    }

//  0x6F758670 - `retn 4`, __thiscall(ecx=table, stack=key const char*): a
//  TSHashTable<T,K>::Ptr(const char*)-shaped lookup (mask at +0x24, buckets
//  at +0x1C) - hashes via Storm_590 then walks the bucket chain comparing
//  each candidate key with Storm_509.
struct STerrainHashTableThunk
{
    int __thiscall Ptr(int key);
    void __thiscall Initialize();
    void __thiscall SetCount(int newcount);
    void __thiscall Rehash(int newcount);
    int __thiscall CheckGrow(int index);
    int __thiscall NewNode(int hashval, int arg1, int arg2);
};
ADDR_THUNK(int __thiscall STerrainHashTableThunk::Ptr(int), 0x6F758670)

//  0x6F759170 - `retn 4`, __thiscall(ecx=array, stack=alloc): a
//  TSGrowableArray<T>::SetAlloc-shaped grow of an "AuTerrainImage..."-tagged
//  array (Storm_405/Storm_401/Storm_403 realloc-or-alloc-and-move, per
//  element via sub_6F7543D0/sub_6F74AB00 - Containers/tsarray.inl's own
//  SetAlloc).
struct STerrainImageArrayThunk
{
    void __thiscall SetAlloc(int alloc);
};
ADDR_THUNK(void __thiscall STerrainImageArrayThunk::SetAlloc(int),
           0x6F759170)

//  0x6F760B30 - `retn 4`, __thiscall(ecx=array, stack=count): the bucket
//  array's own TSGrowableArray<TSExplicitList<T>>::SetCount(unsigned int) -
//  grows/shrinks the bucket array, default-constructing each new bucket's
//  self-referencing sentinel node (Containers/tsarray.inl's own SetCount,
//  ComputeChunk callee at sub_6F73E8C0, SetAlloc callee at sub_6F759690 -
//  both out of this batch's scope).
ADDR_THUNK(void __thiscall STerrainHashTableThunk::SetCount(int),
           0x6F760B30)

//  0x6F762D40 - `retn 0`, __thiscall(ecx=table): TSHashTable<T,K>::
//  Initialize() - mask=3, buckets.SetCount(4) via 0x6F760B30 above, then
//  SetLinkOffset (sub_6F74F280) on each of the four (Containers/tshash.inl's
//  own Initialize).
ADDR_THUNK(void __thiscall STerrainHashTableThunk::Initialize(),
           0x6F762D40)

//  0x6F762F70 - `retn 4`, __thiscall(ecx=table, stack=newcount): TSHashTable
//  <T,K>::Rehash(unsigned int) - parks every node off the old bucket array,
//  regrows it via 0x6F760B30, then reinserts every parked node by
//  `hashval & mask` (Containers/tshash.inl's own Rehash).
ADDR_THUNK(void __thiscall STerrainHashTableThunk::Rehash(int), 0x6F762F70)

//  0x6F765000 - `retn 4`, __thiscall(ecx=table, stack=index): TSHashTable
//  <T,K>::CheckGrow(unsigned int) - decays m_probes, walks the bucket at
//  `index`, and calls 0x6F762F70 (Rehash(2*mask+2)) once the chain gets long
//  (Containers/tshash.inl's own CheckGrow).
ADDR_THUNK(int __thiscall STerrainHashTableThunk::CheckGrow(int), 0x6F765000)

//  0x6F7674A0 - `retn 0Ch` (3 stack dwords), __thiscall(ecx=table,
//  stack=hashval,arg1,arg2): TSHashTable<T,K>::NewNode(unsigned int, void*,
//  void*) - lazily Initialize()s, CheckGrow()s the target bucket, allocates
//  the node through the table's own vtable-slot-1 AllocNode (the
//  hashtablep.inl shape, not allochash.inl's - see Containers/hashtablep.inl
//  and Widget/soundplaytablething.cpp's header comment for the identical
//  "element/key type not established" situation), and links it at the head
//  of `m_list`.
ADDR_THUNK(int __thiscall STerrainHashTableThunk::NewNode(int, int, int),
           0x6F7674A0)

//  0x6F7543D0 - `retn 4`, __thiscall(ecx=dest, stack=src T*): the
//  "AuTerrainImage" element's own copy constructor - copies ~0x9C bytes of
//  plain fields, then Assign()s three sub-arrays via
//  TSGrowableArray<NTempest::C3Vector>::Assign (0x6F4F9640, EXACT),
//  TSGrowableArray<NTempest::C2Vector>::Assign (0x6F50FBC0, DIFFERS) and
//  TSGrowableArray<SUnresolvedField2>::Assign (0x6F4079C0, EXACT) - all
//  three already-reconstructed elsewhere in this tree - plus one more
//  (sub_6F34C260) whose own element type this call tree never names.
struct STerrainImageElementThunk
{
    void __thiscall CopyConstruct(int src);
};
ADDR_THUNK(void __thiscall STerrainImageElementThunk::CopyConstruct(int),
           0x6F7543D0)

//  0x6F741250 - `retn 14h` (5 stack dwords), plain `__stdcall` (no register
//  argument at any call site in this closure): a terrain-normal-style plane
//  fit - two edge vectors, their cross products, FVec3::Normalize
//  (0x6F011550) on the result, then a barycentric-weighted blend of the
//  triangle's own per-vertex attributes into the caller's output struct.
ADDR_THUNK(void __stdcall TerrainNormalPlaneFit(int, int, int, int, int),
           0x6F741250)
