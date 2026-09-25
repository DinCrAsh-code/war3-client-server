//============================================================================
//  0x6F3B0B70 - CGameWar3::Load, vtable slot 5.  Save's (gamewar3save.cpp)
//  read-side counterpart, field for field, through CDataStore's own reader
//  family (cdatastore.h) instead of CDataStoreScratch's writers, and
//  version-gated near the tail.
//
//  Left as naked thunks, the same reasoning as Save's own: each of these
//  either walks a sub-object this batch was not asked to reconstruct
//  (the m_field20 hash table, the AllySlot/player-slot arrays' own
//  per-element readers, the RCString array at m_field3F0) or reaches into
//  an unrelated, already-huge subsystem (a GameUI singleton's own fields,
//  a legacy per-race lookup table). See gamewar3.h's own doc comment on
//  Load for the two real field discoveries this trace made (m_field338,
//  m_field32C).
//
//  0x6F038B60/0x6F02C840 were originally thunked here too, under the
//  names GameWar3_TableEntry1/GameWar3_ApplyTableEntry1, on the mistaken
//  assumption they were CGameWar3-private - a concurrent session found
//  they are a shared, generic version-gate helper pair also used by
//  CAbilityButton::Save/Load, and reconstructed them for real as
//  ConfigureIconGate/ReadGatedDword (Item/abilitybutton_versiongate.cpp).
//  This call site now calls that real, shared reconstruction directly.
//============================================================================
#include "gamewar3.h"
#include "cdatastore.h"
#include "versiongate.h"

extern SVersionGate* __fastcall ConfigureIconGate(int since, int until, void* target);
extern CDataStore* __fastcall ReadGatedDword(CDataStore* store, SVersionGate* gate);

extern "C" {

#define ADDR_THUNK(sig, addr)                     \
    __declspec(naked) sig                         \
    {                                              \
        __asm mov eax, addr                       \
        __asm jmp eax                              \
    }

//  0x6F3A65A0 - retn 0.  __fastcall(stream, &m_gameSlotValue).
ADDR_THUNK(void __fastcall GameWar3_DeserializeGameSlotRef(void*, void*), 0x6F3A65A0)

//  0x6F3B03D0 - retn 0.  __fastcall(stream, &field-inside-m_field20).
ADDR_THUNK(void __fastcall GameWar3_DeserializeIdStringTable(void*, void*), 0x6F3B03D0)

//  0x6F3A6610 - retn 0.  __fastcall(stream, &m_fieldA8) - reads the count
//  and the whole m_allySlots array in one call (unlike Save, which loops
//  the count itself and calls one element-writer per iteration).
ADDR_THUNK(void __fastcall GameWar3_DeserializeAllySlots(void*, void*), 0x6F3A6610)

//  0x6F3A6690 - retn 0.  __fastcall(stream, &m_field54) - same shape as
//  GameWar3_DeserializeAllySlots for m_playerSlots.
ADDR_THUNK(void __fastcall GameWar3_DeserializePlayerSlots(void*, void*), 0x6F3A6690)

//  0x6F3AE6A0 - retn 0.  __fastcall(stream, &m_field98) - Save's own
//  SerializeNestedRefTable, read-side; not reconstructed here because its
//  own per-row/per-element readers (sub_6F3AE370/sub_6F3A7980) are not in
//  this session's dumps.
ADDR_THUNK(void __fastcall GameWar3_DeserializeNestedRefTable(void*, void*), 0x6F3AE6A0)

//  0x6F3A2840 - retn 0.  __thiscall(this) - a legacy per-race string
//  lookup and dispatch (a five-way jump table into sub_6F31F510) whose own
//  return value is unused here.  Nothing to do with a checksum.
ADDR_THUNK(void __fastcall GameWar3_LegacyRaceSideEffect(void*), 0x6F3A2840)

//  0x6F038B60/0x6F02C840 were thunked here as an opaque "table entry"
//  pair; a concurrent session's own real reconstruction supersedes both -
//  see ConfigureIconGate/ReadGatedDword in Item/abilitybutton_versiongate.cpp
//  and Item/abilitybutton_saveload.cpp.

//  0x6F24FF50 - retn 4.  Same shape as the now-superseded pair above, a second,
//  independent global registry slot (dword_6FAB660C vs dword_6FAB6610).
ADDR_THUNK(void* __fastcall GameWar3_TableEntry2(unsigned int, unsigned int, void*), 0x6F24FF50)

//  0x6F24F9C0 - retn 0.  Same shape as GameWar3_ApplyTableEntry1.
ADDR_THUNK(void __fastcall GameWar3_ApplyTableEntry2(void*, void*), 0x6F24F9C0)

//  0x6F3A7A00 - retn 0.  __fastcall(stream, &m_field3F0) - Save's own
//  RCString-array writer, read-side; not reconstructed for the same
//  not-in-this-session's-dumps reason as GameWar3_DeserializeNestedRefTable.
ADDR_THUNK(void __fastcall GameWar3_DeserializeRCStringArray(void*, void*), 0x6F3A7A00)

//  0x6F3A6710 - retn 0.  __fastcall(stream, &m_field400) - the same
//  gameSlotValue-shaped read as GameWar3_DeserializeGameSlotRef, at a
//  second address (/OPT:ICF-less duplicate, same as the write side's own
//  sub_6F3A0950/0910/0990 triple).
ADDR_THUNK(void __fastcall GameWar3_DeserializeField400(void*, void*), 0x6F3A6710)

//  0x6F3A2350 - retn 0.  __fastcall(stream, &m_field2BC) - reads the count
//  and the whole m_field2C0 array in one call, the read-side mirror of
//  Save's own inline loop over the same range.
ADDR_THUNK(void __fastcall GameWar3_DeserializeField2C0Array(void*, void*), 0x6F3A2350)

}

//  0x6F300710 - GetGameUI(create, reset), already reconstructed
//  (GameUI/gameui.h).  Spelled *exactly* as that header declares it, same
//  reasoning as gamewar3save.cpp's own copy of this declaration.
struct SGameUI;
SGameUI* __fastcall GetGameUI(int create, int reset);

//  Three more struct-member naked thunks, each `this` in ecx plus one
//  stack argument - the same reasoning as gamewar3save.cpp's own
//  SGameUIFieldWriteReceiver: a free `__fastcall` function would put the
//  second argument in edx instead of on the stack, a real ABI mismatch.
struct SGameUIApplyReceiver { void ApplyField(void* stream); };
__declspec(naked) void SGameUIApplyReceiver::ApplyField(void*)
{
    __asm mov eax, 06F2FC040h
    __asm jmp eax
}

//  0x6F371370 - retn 4.  Stores `value` into the GameUI hero bar's own
//  +0x180 and tail-calls into an unrelated helper (sub_6F370A90) this
//  repo has no other reason to reconstruct.
struct SGameUIHeroBarReceiver { void SetField180(unsigned int value); };
__declspec(naked) void SGameUIHeroBarReceiver::SetField180(unsigned int)
{
    __asm mov eax, 06F371370h
    __asm jmp eax
}

//  0x6F3A28B0 - retn 4.  __thiscall(this, raceIndexOrValue) -> unsigned
//  int.  The pre-0x177E legacy fallback for m_field338: a 7-way switch
//  (with `this`'s own current localPlayerId's race substituted for the
//  argument when it arrives as 0) mapping a race enum to a small integer.
struct SGameWar3LegacyField338View
{
    unsigned int LegacyField338FromRace(unsigned int raceIndexOrZero);
};
__declspec(naked) unsigned int
SGameWar3LegacyField338View::LegacyField338FromRace(unsigned int)
{
    __asm mov eax, 06F3A28B0h
    __asm jmp eax
}

//  m_field30's own vtable slot 2 (offset 8) and m_field34's own vtable
//  slot 3 (offset 0xC) - the read-side counterparts of Save's own slot
//  1/2 calls on the same two sub-objects.  Same raw-slot-dispatch
//  reasoning as gamewar3save.cpp's own GameWar3SubObjectWriteFn.
typedef void (__thiscall *GameWar3SubObjectReadFn)(void* self, void* stream);

void CGameWar3::Load(CDataStore* stream)
{
    stream->ReadDword(&m_field04);
    stream->ReadDword(&m_currentJassInstanceSlot);

    GameWar3_DeserializeGameSlotRef(stream, &m_gameSlotValue);

    GameWar3_DeserializeIdStringTable(stream, (char*)m_field20 + 4);
    GameWar3_DeserializeIdStringTable(stream, (char*)m_field20 + 0x54);
    GameWar3_DeserializeIdStringTable(stream, (char*)m_field20 + 0x7C);

    stream->ReadWord(&m_field28);
    stream->ReadWord(&m_field2C);

    ((GameWar3SubObjectReadFn)(*(void***)m_field30)[2])(m_field30, stream);
    ((GameWar3SubObjectReadFn)(*(void***)m_field34)[3])(m_field34, stream);

    GameWar3_DeserializeAllySlots(stream, &m_fieldA8);

    stream->ReadDwordAlt(&m_field44);
    stream->ReadDwordAlt(&m_field48);
    stream->ReadDwordAlt(&m_field4C);

    GameWar3_DeserializePlayerSlots(stream, &m_field54);

    stream->ReadDwordAlt(&m_field50);

    GameWar3_DeserializeNestedRefTable(stream, &m_field98);

    stream->ReadDwordAlt2(&m_field3C0);

    GameWar3_LegacyRaceSideEffect(this);

    void* gameUI = (void*)GetGameUI(1, 0);
    ((SGameUIApplyReceiver*)gameUI)->ApplyField(stream);

    unsigned int registryLocal = 3;
    ReadGatedDword(stream, ConfigureIconGate(0x1788, -1, &registryLocal));

    void* heroBar = *(void**)((char*)gameUI + 0x3DC);
    ((SGameUIHeroBarReceiver*)heroBar)->SetField180(registryLocal);

    unsigned int registryLocal2 = 0;
    void* entry2 = GameWar3_TableEntry2(0x178A, (unsigned int)-1, &registryLocal2);
    GameWar3_ApplyTableEntry2(stream, entry2);

    *(unsigned int*)((char*)gameUI + 0x28C) = registryLocal2;
    stream->ReadDwordAlt2(&m_field334);

    //  Unsigned on purpose: the shipped comparisons against 0x177E/0x1182/
    //  0x11DC are `jnb`/`jb`, not `jge`/`jl` - GetVersion() itself returns
    //  `int` (cdatastore.h), but every comparison here is unsigned.
    unsigned int version = (unsigned int)stream->GetVersion();

    if (version < 0x177E)
    {
        unsigned int raceArg;
        stream->ReadDword(&raceArg);
        m_field338 = ((SGameWar3LegacyField338View*)this)->LegacyField338FromRace(raceArg);
    }
    else
    {
        stream->ReadDwordAlt2(&m_field338);
    }

    stream->ReadDwordAlt2(&m_field3C4);

    m_field32C = 1;
    GameWar3_DeserializeRCStringArray(stream, &m_field3F0);

    if (version >= 0x1182)
        GameWar3_DeserializeField400(stream, &m_field400);

    if (version >= 0x11DC)
        GameWar3_DeserializeField2C0Array(stream, &m_field2BC);
}
