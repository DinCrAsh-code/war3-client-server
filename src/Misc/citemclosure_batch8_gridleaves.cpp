//============================================================================
//  Five more addresses from the same CItem vtable BFS (batch 8 of 8 - see
//  docs/targets/CItem-callee-batch8.md), all reached through
//  Misc/citemclosure_batch8_gridwrappers.cpp's own thin dispatchers.  Each
//  one's own body is a genuine grid/terrain-record routine, but every one of
//  them calls at least one further callee this closure never reaches the
//  body of and never establishes the type of - a record layout
//  (sub_6F74BAE0/sub_6F750CB0/sub_6F74BB80), a blend/lookup pair
//  (sub_6F750AB0/sub_6F751060), or a key-list database whose own element
//  type is a further callee out of scope again (sub_6F4D20E0) - so, per the
//  same "guessing a layout is worse than not looking" reasoning
//  citemclosure_terrain_gates3.cpp's STerrainHashTableThunk documents, each
//  is a direct redirect rather than an invented reconstruction.  0%
//  instruction match by construction - see each BEHAVIOUR entry in
//  tools/funcmap.py.
//============================================================================

#define ADDR_THUNK(sig, addr)                     \
    __declspec(naked) sig                         \
    {                                             \
        __asm { mov eax, addr }                   \
        __asm { jmp eax }                         \
    }

//  0x6F746B20 - `retn 8`, __thiscall(ecx=this, stack=x,z): bounds-checks
//  (x,z) against `this+0xB4/0xB8`, computes the grid-cell index, and - when
//  the cell's flag byte at +0xC has bit 3 set - remaps to a "linked" cell
//  via sub_6F73EC10/sub_6F73F010 before scanning that cell's own record
//  range (a `[eax+58h]` count / `[eax+5Ch]` array pair) for one whose four
//  bounds (`[rec],[rec+4],[rec+8],[rec+0Ch]` as x0,y0,x1,y1) contain the
//  point.  Returns 0 (no match) or `eax` (the record's own +0x24 base,
//  reused below by 0x6F754FC0/0x6F760C20).
struct SGridRecordLookupThunk
{
    int __thiscall FindRecordAt(int x, int z);
};
ADDR_THUNK(int __thiscall SGridRecordLookupThunk::FindRecordAt(int, int),
           0x6F746B20)

//  0x6F750810 - `retn 8`, __thiscall(ecx=this, stack=x,z): probes 4 offset
//  variants of (x,z) against a per-cell tag byte read through
//  sub_6F74BAE0, returning 1 the moment one reads 'R' or 'S' ("Road"/
//  "Something" terrain-tag classification, consistent with the "RLAA"
//  4-letter tag 0x6F750D30 compares below).
struct SGridTagProbeThunk
{
    int __thiscall ProbeTagRS(int x, int z);
};
ADDR_THUNK(int __thiscall SGridTagProbeThunk::ProbeTagRS(int, int),
           0x6F750810)

//  0x6F750D30 - `retn 8`, __thiscall(ecx=this, stack=x,z): tries
//  0x6F750810 (mine, above) first, then sub_6F750CB0, then reads a 4-byte
//  tag via sub_6F74BB80 and compares it against the literal "RLAA" (offset
//  into a data table's own string constant, walked 4 bytes at a time up to
//  `dword_6F9815D4`).
struct SGridTagRlaaThunk
{
    int __thiscall ProbeTagRlaa(int x, int z);
};
ADDR_THUNK(int __thiscall SGridTagRlaaThunk::ProbeTagRlaa(int, int),
           0x6F750D30)

//  0x6F7523C0 - `retn 10h` (4 stack dwords), __thiscall(ecx=this, stack=
//  gridX,gridY,recPtr,flags - the order SStormSingletonA::
//  QueryBlendedRecordValue's own div/mod decode of a packed index actually
//  pushes, stormsingletonawavefront.cpp): blends a base height/tint (read
//  off `recPtr[0]`, masked/rescaled) with two conditional contributions
//  gated by `flags` bits 1/2/4 - a per-cell detail term via sub_6F750AB0 and
//  a per-region term via sub_6F751060 (`this+0x790`'s own +0x1C count) -
//  and returns the summed float.
struct SGridBlendThunk
{
    float __thiscall BlendCellValue(int gridX, int gridY, int recPtr, int flags);
};
ADDR_THUNK(float __thiscall SGridBlendThunk::BlendCellValue(int, int, int, int),
           0x6F7523C0)

//  0x6F71AB10 - `retn 0Ch` (3 stack dwords) with two more in `ecx`/`edx` -
//  a real `__fastcall`, not the plain `__stdcall` this file first guessed
//  (its own sole caller, sub_6F6F3520 in stormsingletonadispatch.cpp, loads
//  both registers right before the call): a `/GS`-cookied buffer builder -
//  queries a key count via sub_6F4D20E0, then walks 0..count formatting
//  each key's looked-up value (sub_6F4D2080/MiscDatabaseLookupAndReport,
//  already a THUNK in tools/funcmap.py for the identical "database lookup"
//  shape) into a comma-joined (Storm_503) output buffer.  Its own `/GS-`
//  cookie XOR/check is the unreproducible shape docs/msvc-vc8-idioms.md
//  already records for this fixed build (see
//  citemclosure_terrain_gates.cpp's EnsureTerrainSubsystemA/B).
ADDR_THUNK(void __fastcall BuildMiscDatabaseKeyList(int, int, int, int, int),
           0x6F71AB10)

//  0x6F760C20 - `retn 0Ch` (3 stack dwords), __thiscall(ecx=this, stack=
//  point,y,flags): a terrain-height pick at a world point - tries
//  SStormSingletonA::FindRecordAt (thunked above) then a geometry pick
//  through NodeListPick (0x6F75CE60, already DIFFERS in tools/funcmap.py)
//  over three stack-scratch floats this closure cannot recover the layout
//  of, falling back to QueryCellRecordIndex/QueryBlendedRecordValue (both
//  real SStormSingletonA members, Storm/stormsingletona*.cpp) when the pick
//  misses.
struct SGridHeightPickThunk
{
    float __thiscall PickHeightAt(int point, int y, int flags);
};
ADDR_THUNK(float __thiscall SGridHeightPickThunk::PickHeightAt(int, int, int),
           0x6F760C20)

//  0x6F762090 - `retn 4`, __thiscall(ecx=this, stack=point): the same
//  geometry-pick shape as PickHeightAt above (calls
//  SStormSingletonA::ComputeWavefrontQuadIndex - a real member,
//  stormsingletonawavefront.cpp - then PICKOWNER::Pick, 0x6F75E870, already
//  DIFFERS) over five more stack-scratch floats whose layout is likewise
//  not established here.
struct SGridSurfacePickThunk
{
    float __thiscall PickSurfaceAt(int point);
};
ADDR_THUNK(float __thiscall SGridSurfacePickThunk::PickSurfaceAt(int),
           0x6F762090)
