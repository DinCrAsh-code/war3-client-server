//============================================================================
//  CItem-vtable-closure batch M: a World Editor Database ".slk" sheet's own
//  row array - distinct from Misc/citemclosure_dbfieldhash.cpp's
//  SDatabaseFieldRecord hash table (different address neighbourhood,
//  different growth-chunk constants, no hashing at all here) and from
//  Misc/citemclosure_wedb_depth7.cpp's own WEDB field/entry machinery.
//  The evidence is the row array's own Storm_401 allocation tag,
//  "AUSLKROW..." (0x6F4C98D0's own `push offset aAuslkrow`), which reads as
//  the RTTI descriptor for a shipped `SLKRow` element type, 0x184 bytes -
//  a raw ".slk" spreadsheet row this batch's own BFS depth reaches through
//  WEDB's generic named-database loader, none of it CItem/CSelectable
//  positioning or selection logic.
//
//  Same convention as the other citemclosure_*.cpp files at this depth: a
//  direct `mov eax,ADDR / jmp eax` redirect into the game's own code at its
//  real address, so a build hooking anything else in this target still
//  calls the genuine original rather than link-failing on an unresolved
//  symbol.  0% instruction match by construction - see each BEHAVIOUR entry
//  in tools/funcmap.py.
//
//  `__stdcall` with N plain int parameters is the simplest spelling that
//  cleans exactly 4*N bytes regardless of what the shipped body's real
//  arguments turn out to be, matching every other file at this depth;
//  declared signatures exist only to give thunk_abi_audit.py the right
//  `retn <n>`.
//============================================================================

#define WEDBSLK_THUNK(sig, addr)       \
    __declspec(naked) sig              \
    {                                  \
        __asm { mov eax, addr }        \
        __asm { jmp eax }              \
    }

//----------------------------------------------------------------------------
//  0x6F4C8AE0 - the row array's own growth-chunk clamp: for an input under 8
//  rounds up to the next power of two (or 1), for 8 and over stamps a fixed
//  cap of 8 into the object at +0x0C and returns it - the same shape as
//  citemclosure_wedb_depth7.cpp's own ClampWEDBFieldBucketCapacity, but a
//  different table (a `retn 4` sibling, not a call site of it).  Called from
//  SetRowCount below when the object's own +0x0C chunk field is unset.
//----------------------------------------------------------------------------
WEDBSLK_THUNK(void __stdcall WEDBSLKSheet_ClampRowGrowChunk(int), 0x6F4C8AE0)

//----------------------------------------------------------------------------
//  0x6F4C98D0 - reallocate the row array to an exact element count through
//  Storm_401/403/405, tagged "AUSLKROW..." (0x184-byte rows), moving
//  survivors one at a time on a fresh-block fallback and memset-clearing
//  every newly grown row's own two intrusive sub-objects to their
//  just-constructed poison state.  Called from SetRowCount below with the
//  chunk-rounded count already computed by its caller, not by this member
//  itself.  `retn 4`.
//----------------------------------------------------------------------------
WEDBSLK_THUNK(void __stdcall WEDBSLKSheet_ReallocRowArray(int), 0x6F4C98D0)

//----------------------------------------------------------------------------
//  0x6F4CA170 - SetRowCount(unsigned int count, int clear): grows the row
//  array (rounding the target through
//  WEDBSLKSheet_ClampRowGrowChunk/WEDBSLKSheet_ReallocRowArray above,
//  TSGrowableArray<T>::SetCount's own grow shape) when `count` exceeds the
//  current row count, `memset`s the newly added rows when `clear` is set,
//  and always zero-fills every grown-in row's own two intrusive
//  sub-objects (the ValueName-tagged `rep stosd` loop) before storing the
//  new count.  `retn 8`.
//----------------------------------------------------------------------------
WEDBSLK_THUNK(void __stdcall WEDBSLKSheet_SetRowCount(int, int), 0x6F4CA170)

//----------------------------------------------------------------------------
//  0x6F4CA310 - shrink-or-grow entry point: when the requested row index is
//  at or past the current count, forwards to WEDBSLKSheet_SetRowCount above
//  (index, 0) to grow without clearing; otherwise, when a caller-supplied
//  "compact" flag is set, memmoves every row from `index` onward down by
//  one 0x184-byte slot and stores the shrunk count - a row deletion.
//  `retn 4`.
//----------------------------------------------------------------------------
WEDBSLK_THUNK(void __stdcall WEDBSLKSheet_DeleteOrGrowRow(int), 0x6F4CA310)

//----------------------------------------------------------------------------
//  0x6F4C8DC0 - a bounds-checked cell-text accessor: validates a row index
//  against +0x1C and a column index against +0x20 (each failure stores a
//  distinct negative status code into `*this` and answers the shared empty
//  string `ValueName`, 0x6F87529C - Frame/backdropgenerate.cpp already
//  reads that symbol the same way), then indexes the row array at
//  `row*0x61 + col` (0x61 = 97, this object's own column stride, not the
//  0x184-byte SLKRow stride SetRowCount above uses - a *second*, narrower
//  per-cell array this same object owns) and stores a quote-stripped,
//  NUL-terminated slice of the caller's own quoted-string argument into
//  that cell.  `retn 4`.
//----------------------------------------------------------------------------
WEDBSLK_THUNK(void __stdcall WEDBSLKSheet_SetClampedCellText(int), 0x6F4C8DC0)

#undef WEDBSLK_THUNK
