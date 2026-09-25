//============================================================================
//  Four real, out-of-line addresses CAbilityButton::Save/Load
//  (abilitybutton_saveload.cpp) call into - kept in their own translation
//  unit, the same reason destructable_versiongate.cpp's own
//  ConfigureFloat5CGate/LoadFieldWithVersionGate pair is: without whole-
//  program optimisation, MSVC never inlines across a TU boundary, so a
//  same-TU definition next to its only caller would have let /Ob2 fold
//  these away into Save/Load's own bodies - which is not what the shipped
//  disassembly does (each is a real `call sub_...`, not inlined code).
//
//  0x6F038B60/0x6F038BD0 configure one of two lazily-allocated
//  `SVersionGate` statics (dword_6FAB6610/dword_6FAB6620) - the same
//  "ensure-then-overwrite-all-three-fields-through-the-global" shape
//  destructable_versiongate.cpp's own ConfigureFloat5CGate already
//  documents, just for two distinct statics instead of one.
//  dword_6FAB6610 is the same real static unit_load.cpp's own
//  g_dwordAltGate already mirrors from its own translation unit - each TU
//  that touches a real global like this keeps its own private shadow
//  rather than a shared extern, the established convention for a static
//  this codebase cannot declare as one real `extern` without dragging a
//  cross-file initialisation order into it.
//
//  0x6F02C840/0x6F02C8A0 are the generic gated-read counterparts Load
//  drives: same [since, until] window logic as
//  destructable_versiongate.cpp's own LoadFieldWithVersionGate, just for a
//  plain dword (through ReadDwordAlt) or an adjacent dword pair (through
//  ReadDwordAlt3, twice) instead of a single CFloat through ReadFloat.
//============================================================================
#include "cdatastore.h"
#include "cdatastorescratch.h"
#include "versiongate.h"
#include "storm.h"   // placement new

//  Storm.dll ordinal 401.
void* __stdcall SMemAlloc(unsigned int size, const char* logfilename,
                          int logline, unsigned int flags);

//  Truncated identically in the dump (aEDrive1TempBui_92) to several other,
//  textually different strings in this module's neighbourhood - see
//  destructable_versiongate.cpp's own note on the same shape.
static const char kGateAllocTag[] =
    "e:\\drive1\\temp\\buildwar3x\\war3\\source\\engine\\saveversion.h";

//----------------------------------------------------------------------------
//  0x6F038B60 - configures dword_6FAB6610.
//----------------------------------------------------------------------------
static SVersionGate* g_iconGate = 0;   // dword_6FAB6610

SVersionGate* __fastcall ConfigureIconGate(int since, int until, void* target)
{
    if (g_iconGate == 0)
    {
        void* raw = SMemAlloc(12, kGateAllocTag, 0x9D, 0);
        if (raw != 0)
            g_iconGate = new (raw) SVersionGate();
        else
            g_iconGate = 0;
    }

    //  Through the global itself rather than a cached local for each of the
    //  three stores - the shipped code re-reads dword_6FAB6610 before every
    //  one, the same shape destructable_versiongate.cpp's own
    //  ConfigureFloat5CGate already documents (and is itself still short of
    //  1.000 on this exact idiom - a pre-existing gap, not one this session
    //  introduces).
    g_iconGate->m_since = since;
    g_iconGate->m_until = until;
    g_iconGate->m_target = target;
    return g_iconGate;
}

//----------------------------------------------------------------------------
//  0x6F038BD0 - configures dword_6FAB6620, reused for both field pairs.
//----------------------------------------------------------------------------
static SVersionGate* g_fieldsGate = 0;   // dword_6FAB6620

SVersionGate* __fastcall ConfigureFieldsGate(int since, int until, void* target)
{
    if (g_fieldsGate == 0)
    {
        void* raw = SMemAlloc(12, kGateAllocTag, 0x9D, 0);
        if (raw != 0)
            g_fieldsGate = new (raw) SVersionGate();
        else
            g_fieldsGate = 0;
    }

    g_fieldsGate->m_since = since;
    g_fieldsGate->m_until = until;
    g_fieldsGate->m_target = target;
    return g_fieldsGate;
}

//----------------------------------------------------------------------------
//  0x6F02C840 - the generic gated single-dword reader Load's own +0x6C
//  site drives.
//----------------------------------------------------------------------------
CDataStore* __fastcall ReadGatedDword(CDataStore* store, SVersionGate* gate)
{
    int afterSince = ((unsigned int)store->GetVersion() >= (unsigned int)gate->m_since);
    int beforeUntil = ((unsigned int)store->GetVersion() <= (unsigned int)gate->m_until);

    if (afterSince && beforeUntil)
    {
        unsigned int* target = (unsigned int*)gate->m_target;
        if (target != 0)
        {
            store->ReadDwordAlt(target);
            return store;
        }
    }

    if (beforeUntil)
        return store;

    unsigned int scratch;
    store->ReadDwordAlt(&scratch);
    return store;
}

//----------------------------------------------------------------------------
//  0x6F02C8A0 - the generic gated dword-*pair* reader, for the two field
//  groups.  Same window logic; reads/discards two adjacent dwords instead
//  of one.
//----------------------------------------------------------------------------
CDataStore* __fastcall ReadGatedDwordPair(CDataStore* store, SVersionGate* gate)
{
    int afterSince = ((unsigned int)store->GetVersion() >= (unsigned int)gate->m_since);
    int beforeUntil = ((unsigned int)store->GetVersion() <= (unsigned int)gate->m_until);

    if (afterSince && beforeUntil)
    {
        unsigned int* target = (unsigned int*)gate->m_target;
        if (target != 0)
        {
            store->ReadDwordAlt3(&target[0]);
            store->ReadDwordAlt3(&target[1]);
            return store;
        }
    }

    if (beforeUntil)
        return store;

    unsigned int scratch[2] = { 0, 0 };
    store->ReadDwordAlt3(&scratch[0]);
    store->ReadDwordAlt3(&scratch[1]);
    return store;
}
