//============================================================================
//  BFS-depth-9 descendants of CItem's vtable slots 32/104
//  (asm/processed/CItem__Method_0x80_0x6F2B6130_calltree_asm.md), continuing
//  the same frontier as Misc/citemclosure_terrain_gates.cpp,
//  citemclosure_terrain_gates2.cpp, citemclosure_wedb_depth7.cpp and
//  citemclosure_terrain_hashtable2.cpp/hashtrio.cpp.  Same convention as
//  those: a direct `mov eax,ADDR / jmp eax` redirect into the game's own
//  code at its real address, so a build hooking anything else in this
//  target still calls the genuine original rather than link-failing on an
//  unresolved symbol.  0% instruction match by construction - see each
//  BEHAVIOUR entry in tools/funcmap.py.
//
//  Declared signatures below exist only to give thunk_abi_audit.py the
//  right `retn <n>` - the byte count read straight off each dump's own
//  trailing `retn` line - not to claim a real calling convention: nothing
//  in src/ calls any of these (grep says zero references outside this
//  file), so the declaration only has to compile and clean the right
//  number of stack bytes.  `__stdcall` with N plain int parameters is the
//  simplest spelling that cleans exactly 4*N bytes regardless of what the
//  shipped body's real arguments turn out to be; `__cdecl` with none for
//  `retn 0`.
//============================================================================

//----------------------------------------------------------------------------
//  A World Editor Database row/entry cluster distinct from
//  citemclosure_wedbentryhash.cpp's own WEDBEntryHash table - vtable-
//  confirmed class identity, but every one of these four bodies carries the
//  full `push -1 / push offset SEH_<addr> / mov eax,fs:0` unified-EH frame
//  docs/msvc-vc8-idioms.md's own "An `__except_handler4`-shaped frame this
//  toolchain cannot reproduce" section already explains this repo's fixed
//  `/GS- /EHs-c-` `cl.exe` cannot emit (a later CRT patch level than the
//  RTM copy in `vs8/`), so a source-level reconstruction would still not
//  match the frame regardless of how well the body itself is understood.
//----------------------------------------------------------------------------

//  0x6F4C99F0 - a constructor: stamps `??_7CRowOrColumn@@6B@`
//  (const CRowOrColumn::`vftable'), zeroes six fields, calls the shared
//  logger 0x6F4C40C0 tagged "HDATABASE", and stores its two stack arguments
//  at +0x20/+0x24.  `retn 8`.
__declspec(naked) void __stdcall ConstructCRowOrColumn(int, int)
{
    __asm { mov eax, 06F4C99F0h }
    __asm { jmp eax }
}

//  0x6F4CAB30 - parses an SLK sheet's row array: checks the ten-byte header
//  "ID;PWXL;N;E" via Storm_508 (the standard SLK signature), then loops
//  0x6F4C85E0/0x6F4CA570 pulling one row at a time.  Its own scope-table
//  tail (0x6F4C94D0) releases the row array afterward through Storm_403,
//  tagged "AUSLKROW" - the SLK row array's own type descriptor, and the
//  strongest evidence this is CRowOrColumn's own SLK loader.  `retn 4`.
__declspec(naked) void __stdcall ParseSLKRowArray(int)
{
    __asm { mov eax, 06F4CAB30h }
    __asm { jmp eax }
}

//  0x6F4CAEE0 - a constructor: stamps `??_7CDatabaseBase@@6B@`
//  (const CDatabaseBase::`vftable'), runs two field constructors
//  (0x6F4CA7F0, already-established shape - a row/column sub-object at
//  +0x10 and +0x38) and then re-stamps `??_7CHandleObject@@6B@` - the
//  construct-as-base-then-upgrade-vtable idiom this closure's earlier
//  batches already documented for other classes in this hierarchy.
//  `retn 0`.
__declspec(naked) void __cdecl ConstructCDatabaseBase()
{
    __asm { mov eax, 06F4CAEE0h }
    __asm { jmp eax }
}

//  0x6F4CB0B0 - populates a CDatabaseBase's row and entry tables: walks a
//  caller-supplied source through two vtable slots (0xC/0x14), and for each
//  string returned starting with 'S', resolves it through the shared
//  allocator (0x6F4C1AB0, tagged "HDBFIELD" for the first table, "HDBENTRY"
//  for the second), constructing a fresh CRowOrColumn (0x6F4C99F0 above)
//  when the allocator hands back a new slot, and linking each result into
//  the table at +0x10 or +0x38 (0x6F4CAF80).  147 instructions.  `retn 0`.
__declspec(naked) void __cdecl PopulateCDatabaseBaseRowAndEntryTables()
{
    __asm { mov eax, 06F4CB0B0h }
    __asm { jmp eax }
}

//  0x6F4CA5D0 - a `TSHashTable<CRowOrColumn,HASHKEY_CONSTSTR>` constructor:
//  stamps `??_7?$TSHashTable@VCRowOrColumn@@VHASHKEY_CONSTSTR@@@@6B@` and
//  initialises the table's storage - the same "why a redirect and not a
//  reconstruction" reasoning citemclosure_wedb_depth7.cpp's own
//  ConstructWEDBEntryTable already gives: this repo's TSHashTable declares
//  two virtuals it never defines, so making the template concrete would
//  stamp a vtable of this build's own making over the shipped four-slot
//  one.  No SEH frame - a plain constructor - but kept a thunk for that
//  same vtable-stamping reason regardless.  `retn 0`.
__declspec(naked) void __cdecl ConstructCRowOrColumnHashTable()
{
    __asm { mov eax, 06F4CA5D0h }
    __asm { jmp eax }
}

//----------------------------------------------------------------------------
//  A second, unnamed HASHKEY_CONSTSTR-shaped string table in the
//  0x6F731xxx-0x6F734xxx neighbourhood - a `Ptr(const char*)`-shaped lookup
//  and its `New()`-shaped insert-or-update sibling, over a table whose
//  owning class nothing reachable from this closure names (no vtable
//  stamp, no Storm allocation tag anywhere in either body).  Same shape as
//  Containers/tshashstr.inl's own `Ptr`/`New`, just an address
//  neighbourhood this closure's dumps never connect to a caller.
//----------------------------------------------------------------------------

//  0x6F731BF0 - `Ptr(const char* key)`-shaped lookup (Storm_590 hash,
//  Storm_509 bounded compare).  `retn 4`.
__declspec(naked) void __stdcall LookupGenericStringKeyedRecord(int)
{
    __asm { mov eax, 06F731BF0h }
    __asm { jmp eax }
}

//  0x6F7344D0 - the mutating counterpart: probes the same table, calls a
//  vtable slot through the table object on a miss, then splices the result
//  into the table-wide list - the same InsertOrUpdate shape
//  citemclosure_wedb_depth7.cpp's own InsertOrUpdateTerrainCell already
//  names for an unrelated table.  `retn 0Ch`.
__declspec(naked) void __stdcall InsertOrUpdateGenericStringKeyedRecord(int, int, int)
{
    __asm { mov eax, 06F7344D0h }
    __asm { jmp eax }
}

//----------------------------------------------------------------------------
//  A fourth terrain-cell hash table in the 0x6F7AFxxx-0x6F7B2xxx
//  neighbourhood, distinct from citemclosure_terrain_hashtable2.cpp's own
//  STerrainHashRecord3 table (that one's Initialize/CheckGrow/Ptr already
//  sit at 0x6F7B1D10/0x6F7B2530/0x6F7B2750, and this table's own
//  Initialize/CheckGrow below are new, different addresses) - the table
//  citemclosure_wedb_depth7.cpp's own FindTerrainCellByCoord (0x6F7B0AC0)
//  and InsertOrUpdateTerrainCell (0x6F7B2880) already reach through
//  "unk_6FADBEF4".  Four more members of it: Initialize, CheckGrow, and two
//  more Find/InsertOrUpdate-shaped bodies alongside those already-named
//  ones - "a sibling of the other one" the same way
//  citemclosure_wedb_depth7.cpp's own InitializeWEDBCacheTable1/2/3 record
//  it, not an invented distinguishing fact.
//----------------------------------------------------------------------------

//  0x6F7B2110 - Initialize(): four buckets, mask 3, matching
//  Containers/tshash.inl's own Initialize() exactly (`push 4` / SetCount(4)
//  on the bucket array at +0x14, `mov [this+0x24],3`, then every bucket
//  told its link offset).  `retn 0`.
__declspec(naked) void __cdecl InitializeTerrainCellHashTable()
{
    __asm { mov eax, 06F7B2110h }
    __asm { jmp eax }
}

//  0x6F7B25D0 - CheckGrow(unsigned int index): byte-identical shape to the
//  already-established 0x6F7B2530 (STerrainHashRecord3's own CheckGrow) but
//  calling a different Rehash (0x6F7B2170, out of this batch's scope) - the
//  two are provably distinct tables rather than the same function twice.
//  `retn 4`.
__declspec(naked) void __stdcall CheckGrowTerrainCellHashTable(int)
{
    __asm { mov eax, 06F7B25D0h }
    __asm { jmp eax }
}

//  0x6F7B0A50 - a second Find-by-coordinate-pair lookup over the same
//  table FindTerrainCellByCoord (0x6F7B0AC0) already covers, matching two
//  dwords at +0x18/+0x1C of the candidate the same way.  `retn 8`.
__declspec(naked) void __stdcall FindTerrainCellByCoord2(int, int)
{
    __asm { mov eax, 06F7B0A50h }
    __asm { jmp eax }
}

//  0x6F7B27E0 - a second InsertOrUpdate over the same table
//  InsertOrUpdateTerrainCell (0x6F7B2880) already covers - same probe/grow/
//  splice shape, different call site.  `retn 0Ch`.
__declspec(naked) void __stdcall InsertOrUpdateTerrainCell2(int, int, int)
{
    __asm { mov eax, 06F7B27E0h }
    __asm { jmp eax }
}

//  0x6F7AFAB0 - unlinks every node out of an array-of-buckets belonging to
//  this same terrain-cell table (the SetCount-shrink/SetAlloc-move helper
//  this neighbourhood's own SetCount/SetAlloc bodies call per vacated
//  bucket), draining each bucket's own list in place.  `retn 0`.
__declspec(naked) void __cdecl UnlinkAllTerrainCellBucketArray()
{
    __asm { mov eax, 06F7AFAB0h }
    __asm { jmp eax }
}

//----------------------------------------------------------------------------
//  Two more terrain internals, address neighbourhoods this closure's own
//  dumps do not connect to any named class or Storm allocation tag.
//----------------------------------------------------------------------------

//  0x6F74ED80 - drains a single TSExplicitList-shaped bucket in place (the
//  inlined TSLink::Unlink loop this whole closure's dumps are full of),
//  reloading the node count off `[this+8]` every pass - the same shape as
//  UnlinkAllTerrainCellBucketArray above, in an unrelated 0x6F74Exxx
//  neighbourhood with no caller reachable from this dump to tie it to a
//  named table.  `retn 0`.
__declspec(naked) void __cdecl PurgeTerrainListBucket()
{
    __asm { mov eax, 06F74ED80h }
    __asm { jmp eax }
}

//  0x6F71A610 - checks whether a caller-supplied key already carries the
//  "WESTRING_" prefix (Storm_508, bounded compare, 9 characters); if not,
//  formats a "%s%s"-shaped path through Storm_501 and forwards to the
//  resolver sub_6F736150 - an "ensure this key is a localisation string
//  key, adding the prefix if it is missing" gate.  Has its own /GS-cookie
//  prologue (plain stack-buffer form, no SEH frame) this repo's fixed
//  `/GS-` build cannot reproduce.  `retn 0`.
__declspec(naked) void __cdecl EnsureLocalizedStringKeyPrefix()
{
    __asm { mov eax, 06F71A610h }
    __asm { jmp eax }
}

//  0x6F71AA40 - looks a key up in a global registry (dword_6FADB0A4,
//  sub_6F4D2080) and, on a miss, resolves it through
//  EnsureLocalizedStringKeyPrefix above before returning the registry's
//  answer - a "look up or register, prefixing the key first" wrapper over
//  the two.  `retn 0Ch`.
__declspec(naked) void __stdcall ResolveOrRegisterLocalizedStringKey(int, int, int)
{
    __asm { mov eax, 06F71AA40h }
    __asm { jmp eax }
}

//  0x6F7899A0 - walks a caller object's own linked field (+0x1C),
//  releasing each entry through sub_6F753700 and then removing it from a
//  global registry (unk_6FADB524, sub_6F4C1B50) before continuing from the
//  head again - a "release and unregister everything" purge loop.
//  `retn 0`.
__declspec(naked) void __cdecl PurgeTerrainRecordRegistry()
{
    __asm { mov eax, 06F7899A0h }
    __asm { jmp eax }
}

//  0x6F7590A0 - resizes a "AuTerrainGridMesh"-tagged array of 0x94-byte
//  elements: releases every element from the shrunk-past tail
//  (sub_6F74AA40) through Storm_405 (resize in place) or Storm_401 (fresh
//  alloc, moving survivors one at a time through sub_6F754280/
//  sub_6F74AA40) or Storm_403 (release outright for a target size of
//  zero) - the same SetAlloc-shaped resize
//  citemclosure_terrain_gates.cpp's own ReleaseTerrainGridMeshArray
//  neighbours.  `retn 4`.
__declspec(naked) void __stdcall ResizeTerrainGridMeshArray(int)
{
    __asm { mov eax, 06F7590A0h }
    __asm { jmp eax }
}
