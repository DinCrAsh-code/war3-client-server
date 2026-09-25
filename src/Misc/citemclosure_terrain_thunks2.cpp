//============================================================================
//  More of the same NTempest/Storm terrain-rendering frontier
//  Misc/citemclosure_terrain_gates.cpp's own header describes - confirmed
//  unrelated to CItem/CSelectable by its own evidence (a `CWeatherDB`
//  vtable stamp, a `WESTRING_WARNING_MISSINGSTRING`/`WorldEditStrings`
//  localisation lookup, an RTTI-tagged array walk, none of it item-
//  positioning or selection logic), left as redirects for the same reason
//  every entry in that file is: a direct `mov eax,ADDR`/`jmp eax` into the
//  game's own code so a build hooking anything else in this target still
//  calls the genuine original.  Declared signatures exist only to give
//  thunk_abi_audit.py the right `retn <n>`, read straight off each dump's
//  own trailing `retn` line.
//============================================================================

#define ADDR_THUNK(sig, addr)                     \
    __declspec(naked) sig                         \
    {                                             \
        __asm { mov eax, addr }                   \
        __asm { jmp eax }                         \
    }

//  0x6F784E70 - CWeatherDB's constructor: stamps `??_7CWeatherDB@@6B@`,
//  reads a "TerrainArt\\Weather" config table keyed on "effectID".
//  `retn 0`.  Left a redirect for the same reason
//  Misc/citemclosure_database_thunks.cpp's header gives: no
//  `docs/targets/vtables/` entry for CWeatherDB, so a real constructor here
//  would stamp a vtable of unknown length.
ADDR_THUNK(void* __fastcall ConstructCWeatherDB(void* self), 0x6F784E70)

//  0x6F735D60 - looks a "WESTRING_..." key up in the WorldEditStrings
//  table (sub_6F731BF0) and, on a miss, logs a one-shot "missing string"
//  warning through Storm's own diagnostic path.  `retn 0`,
//  __fastcall(ecx=self, edx=flag).
ADDR_THUNK(void __fastcall LookupWorldEditStringOrWarn(void* self, int flag),
           0x6F735D60)

//  0x6F736150 - formats a "%s: %s" missing-string warning (recursing into
//  the function above for the diagnostic text itself when the section
//  table has no entry either) and hands it to Storm's warning dialog.
//  `retn 8`, __fastcall(ecx=section, edx=key) plus two stack dwords
//  (`arg_0`/`arg_4` in the dump's own naming).
ADDR_THUNK(void __fastcall GetWorldEditStringOrWarn(const char* section,
                                                    const char* key,
                                                    int arg0, int arg4),
           0x6F736150)

//  0x6F789F20 - a `TSGrowableArray`/list-shaped SEH-framed destructor two
//  levels into this closure's own database/terrain hash families
//  (`sub_6F7899A0`/`sub_6F74ED80`, both out of this batch's scope) whose
//  exception path shares tail code with a second, unnamed function at
//  0x6F751EA0 - the same `__except_handler4`-shaped frame
//  docs/msvc-vc8-idioms.md's own note says this toolchain cannot
//  reproduce.  `retn 0`.
ADDR_THUNK(void __fastcall TerrainRecordArrayDtorSEH(void* self), 0x6F789F20)

//  0x6F75F270 - a `TSGrowableArray<T>::SetCount`-shaped resize over a
//  148-byte (`0x94`) element whose constructor/destructor
//  (`sub_6F74A780`/`sub_6F74AA40`) are real out-of-line bodies rather than
//  the trivial no-ops citemclosure_terrain_hashtrio.cpp's own
//  `TSExplicitList` elements get, so this is not the same generic
//  instantiation - both callees are out of this batch's scope.  `retn 4`.
//  MSVC will not accept __thiscall on a free function, so - as in
//  Misc/citemclosure_database_thunks.cpp - this is a one-method struct's
//  member.
struct STerrainRecordArrayThunk { void SetCount(unsigned int count); };
ADDR_THUNK(void STerrainRecordArrayThunk::SetCount(unsigned int count),
           0x6F75F270)

//  0x6F6F9A60 - linear scan of a 0x41-byte-stride array at `this+0x1D4`
//  for a matching leading byte, bounded by a count from `sub_6F6F9A20`
//  (out of this batch's scope); returns the matching index or -1.
//  `retn 4`, __fastcall(ecx=self, edx unused going in) plus one stack
//  dword (the byte value read from the low 8 bits of it).
ADDR_THUNK(int __fastcall FindByteInStrideArray(void* self, int unused,
                                                int needle),
           0x6F6F9A60)

//  0x6F7B2920 - `TSHashTable<T,K>::NewNode`-shaped (Initialize-if-empty,
//  CheckGrow, then a virtual `AllocNode`-style dispatch through `this`'s
//  own vtable slot 0 with the bucket pointer and both extra arguments) for
//  the same terrain table citemclosure_terrain_hashtable2.cpp's Initialize/
//  CheckGrow/Ptr trio reconstructs - left a redirect rather than folded
//  into that file because the virtual dispatch's real signature is exactly
//  as unresolved here as it is for
//  Misc/citemclosure_database_thunks.cpp's `WEDBEntryHashTable_Clear`.
//  `retn 0Ch`.
struct STerrainHashTable2Thunk
{
    void* NewNode(unsigned int hashval, void* arg1, void* arg2);
    void* FindOrCreate(int a, int b, int c, int d);
};
ADDR_THUNK(void* STerrainHashTable2Thunk::NewNode(unsigned int hashval,
                                                   void* arg1, void* arg2),
           0x6F7B2920)

//  0x6F7B2B00 - a find-or-create over the same table: hashes/compares
//  through `sub_6F7B0A50` and, on a miss, calls `sub_6F7B27E0` (this
//  file's own `STerrainHashTable2Thunk::NewNode`'s likely sibling, out of
//  this batch's scope) to build the new node.  `retn 10h`.
ADDR_THUNK(void* STerrainHashTable2Thunk::FindOrCreate(int a, int b, int c,
                                                        int d),
           0x6F7B2B00)
