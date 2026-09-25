//============================================================================
//  CAbilityButton's Save/Load overrides (slots 14/15, +0x38/+0x3C,
//  overriding CAgentWar3::Save/Load - agentwar3.h).  See abilitybutton.h.
//
//  Both call the base version first (CAgentWar3::Save/Load), then persist
//  the button's own four fields through three version-gated groups.
//
//  Save's own three sites never check `until` at all - unlike Load, a
//  writer only ever needs "is the stream new enough to carry this field",
//  never "is it already too new" - and never call out to a separate
//  gated-write helper at all: the shipped disassembly inlines the whole
//  "read the gate's own `since`/`target`, compare, conditionally write"
//  sequence directly into Save's own body at each of its three sites, so
//  WriteGatedDword/WriteGatedDwordPair below are `static` (no real address
//  of their own - they exist only so this file does not repeat the same
//  five lines three times) and expected to fold back into Save through
//  ordinary same-TU inlining.
//
//  Load's own three sites are the opposite shape: each is a real
//  `call sub_...` to one of two out-of-line generic readers
//  (ReadGatedDword/ReadGatedDwordPair, abilitybutton_versiongate.cpp) -
//  kept in a separate translation unit from this file for exactly that
//  reason, the same way destructable_versiongate.cpp's own
//  LoadFieldWithVersionGate is separate from destructable_save.cpp: without
//  whole-program optimisation, MSVC never inlines across a TU boundary, so
//  a same-TU definition next to its only caller would fold away the real,
//  separate `call` the shipped Load actually makes.
//
//  ConfigureIconGate/ConfigureFieldsGate (0x6F038B60/0x6F038BD0, also in
//  abilitybutton_versiongate.cpp) are real out-of-line addresses both Save
//  and Load call into - the same lazily-allocated file-static SVersionGate,
//  reconfigured on every use, destructable_versiongate.cpp's own
//  ConfigureFloat5CGate documents, with two distinct statics
//  (dword_6FAB6610, shared with unit_load.cpp's own g_dwordAltGate;
//  dword_6FAB6620, used only from here) rather than one.
//
//  Field grouping, read straight off the dump: m_defaultIconId (+0x6C)
//  gates and writes alone through WriteDwordAlt/ReadDwordAlt; m_field70/74
//  and m_field78/7C each gate and write as an adjacent pair through
//  WriteDwordAlt2/ReadDwordAlt3 - two consecutive dwords read out of (and
//  into) the *same* gate's `m_target`/`m_target+4`, confirmed by both
//  0x6F042C90 (Save) and 0x6F042D50 (Load) reusing dword_6FAB6620's own
//  configure function once per pair rather than once per field.
//============================================================================
#include "abilitybutton.h"
#include "cdatastore.h"
#include "cdatastorescratch.h"
#include "versiongate.h"

//  abilitybutton_versiongate.cpp.
SVersionGate* __fastcall ConfigureIconGate(int since, int until, void* target);
SVersionGate* __fastcall ConfigureFieldsGate(int since, int until, void* target);
CDataStore* __fastcall ReadGatedDword(CDataStore* store, SVersionGate* gate);
CDataStore* __fastcall ReadGatedDwordPair(CDataStore* store, SVersionGate* gate);

//  No address of its own - see header comment above.
static void WriteGatedDword(CDataStoreScratch* store, SVersionGate* gate)
{
    if ((unsigned int)((CDataStore*)store)->GetVersion() >= (unsigned int)gate->m_since)
    {
        unsigned int* target = (unsigned int*)gate->m_target;
        if (target != 0)
            store->WriteDwordAlt(*target);
    }
}

//  Same shape, for the two adjacent-dword groups.  No address of its own.
static void WriteGatedDwordPair(CDataStoreScratch* store, SVersionGate* gate)
{
    if ((unsigned int)((CDataStore*)store)->GetVersion() >= (unsigned int)gate->m_since)
    {
        unsigned int* target = (unsigned int*)gate->m_target;
        if (target != 0)
        {
            store->WriteDwordAlt2(target[0]);
            store->WriteDwordAlt2(target[1]);
        }
    }
}

//----------------------------------------------------------------------------
//  0x6F042C90 - slot 14, Save.
//----------------------------------------------------------------------------
void CAbilityButton::Save(CDataStoreScratch* store)
{
    CAgentWar3::Save(store);

    WriteGatedDword(store, ConfigureIconGate(0x1771, -1, &m_defaultIconId));
    WriteGatedDwordPair(store, ConfigureFieldsGate(0x1771, -1, &m_field70));
    WriteGatedDwordPair(store, ConfigureFieldsGate(0x1771, -1, &m_field78));
}

//----------------------------------------------------------------------------
//  0x6F042D50 - slot 15, Load.
//----------------------------------------------------------------------------
void CAbilityButton::Load(CDataStore* store)
{
    CAgentWar3::Load(store);

    ReadGatedDword(store, ConfigureIconGate(0x1771, -1, &m_defaultIconId));
    ReadGatedDwordPair(store, ConfigureFieldsGate(0x1771, -1, &m_field70));
    ReadGatedDwordPair(store, ConfigureFieldsGate(0x1771, -1, &m_field78));
}
