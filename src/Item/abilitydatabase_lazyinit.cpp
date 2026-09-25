//============================================================================
//  0x6F25EFE0 - LazyInitGameDataCacheField14: allocate and placement-
//  construct the CAbilityDatabase parked at the game-data cache's own
//  +0x14 slot (array index 5), then store and re-fetch it.  See
//  gamecontext.h's own declaration comment and
//  docs/notes/checksum-provider-registry.md for the slot's place in the
//  wider lazy-singleton array.
//
//  Own translation unit, not part of abilitygenericfieldrecord.cpp: address
//  neighbourhood (0x6F25EFE0) is far enough from that file's own
//  0x6F262xxx/0x6F25Bxxx/0x6F25F5xx cluster (CLAUDE.md's "one translation
//  unit per original module") that it shipped separately, and its own
//  callee (LazyInitGameDataCacheField40, 0x6F25AC40, an SEH-carrying
//  function ~0x4300 bytes further down) is further evidence the two lazy
//  fields' own init helpers are not part of that module either.
//
//  No SEH frame here (unlike LazyInitGameDataCacheField40 below it in the
//  dump): the object's own construction is trivial - three zero stores and
//  a vtable stamp, nothing that can throw - so MSVC inlines it flat with
//  no unwind story at all.  The null test and null-store-on-failure this
//  dump shows are MSVC's usual `new (p) T()` shape for a `p` that may be
//  null (docs/msvc-vc8-idioms.md, "Placement new") - written here as plain
//  field stores under an explicit `if`, not an actual placement-new of a
//  C++ CAbilityDatabase, because CAbilityDatabase declares no constructor
//  at all (see abilitydatabase.h's own note on why) - the shape the
//  compiler emits is identical either way for a POD with no base and no
//  virtual dispatch of its own.
//
//  The caller (GetAbilityDataLoader, abilitygenericfieldrecord.cpp) already
//  checks `context->m_pCache->m_field14 != 0` before ever calling here, so
//  this function itself allocates unconditionally on every call - it does
//  not re-check the slot, matching the dump exactly.
//
//  Safe to arm in mix_config.json as far as the vtable stamp goes - the
//  object this hands back carries the real, complete, shipped
//  `??_7CAbilityDatabase@@6B@` (0x6F92E0A8), not a half-reconstructed one
//  of our own, so GetAbilityDataLoader's own vtable[2] dispatch reaches the
//  real "Units\\AbilityData" loader exactly as it would unhooked.  Left
//  `false` anyway, per CLAUDE.md - nothing here arms a hook.
//
//  CAbilityDatabase now really derives from CWar3SlkDatabase (see
//  abilitydatabase.h's own note, 2026-09-13) but still declares no
//  constructor of its own - this function's literal vtable-pointer write
//  is unaffected either way, and stays the right call: this class's own
//  implicit constructor (were it ever instantiated) would stamp its own,
//  never-elsewhere-used `??_7CAbilityDatabase@@6B@`, not the base's, and
//  the shipped literal remains the genuinely correct address regardless.
//============================================================================
#include "abilitydatabase.h"
#include "gamecontext.h"

//  Storm.dll ordinal 401.  Same declaration as src/storm.h's, repeated
//  rather than pulling in that header's containers - see
//  docs/msvc-vc8-idioms.md, "Declaring imports so the diff can see them".
void* __stdcall SMemAlloc(unsigned int amount, const char* logfilename,
                          int logline, unsigned int flags);

//  Allocation tag: the header the type was declared in, per Blizzard's
//  allocation macros - same string this dump's own two lazy-init helpers
//  both push (0x6F8782C0, read with `ida_query bytes` rather than trusted
//  from IDA's own truncated "..." comment).
static const char* const kAbilityDataHeader =
    "e:\\drive1\\temp\\buildwar3x\\war3\\source\\unit\\../Data.h";

//  The shipped `??_7CAbilityDatabase@@6B@`.  Written as the literal address
//  rather than through a C++ constructor - see abilitydatabase.h's own note
//  on why: a real constructor for a class with virtuals always stamps
//  *some* vtable, and this class's own is not "finished" in
//  vtable_classes.json (2 of 3 slots have no dump), so tools/ctor_vtable_audit.py
//  is right to flag installing our own.  This is the shipped one instead -
//  genuinely correct, not merely score-equivalent.
#define ABILITY_DATABASE_VTABLE ((void*)0x6F92E0A8)

void* __fastcall LazyInitGameDataCacheField14(unsigned int index, unsigned int)
{
    CGameDataContext* context = (CGameDataContext*)GetThreadLocalSlot(kThreadLocalGameData);

    CAbilityDatabase* obj = (CAbilityDatabase*)SMemAlloc(
        sizeof(CAbilityDatabase), kAbilityDataHeader, 0x74, 0);

    //  `context->m_pCache` read as a flat `void**` and indexed by the
    //  caller's own runtime `index` (5 for this slot) - the same "shared
    //  lazy-singleton slot array" model GameDataContextChecksumProviderHash
    //  already documents (gamecontext.h), not the named CGameDataCache
    //  fields: those are fixed compile-time offsets a caller can spell
    //  directly, but this helper's own `index` argument is a runtime value,
    //  so the dump's own `[ecx+esi*4]` addressing is what a real array
    //  index compiles to, not a struct member access.
    //
    //  The store-then-refetch is written out in full in BOTH arms rather
    //  than once after the `if` - the dump duplicates that exact five-
    //  instruction tail into both branches (loc_6F25F02E is a full second
    //  copy, not a shared merge point), which is what this mirrors; a
    //  single shared tail after the `if` compiles to one merged copy
    //  instead and no longer matches.
    if (obj != 0)
    {
        obj->m_recordSet = 0;
        obj->m_haveChecksum = 0;
        obj->m_checksum = 0;
        *(void**)obj = ABILITY_DATABASE_VTABLE;
        ((void**)context->m_pCache)[index] = obj;
        return ((void**)context->m_pCache)[index];
    }
    else
    {
        ((void**)context->m_pCache)[index] = 0;
        return ((void**)context->m_pCache)[index];
    }
}

#undef ABILITY_DATABASE_VTABLE
