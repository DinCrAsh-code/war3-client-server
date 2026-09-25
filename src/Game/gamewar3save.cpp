//============================================================================
//  0x6F3A6C70 - CGameWar3::Save, vtable slot 4.  See gamewar3.h's own doc
//  comment for the field walk and what this batch left thunked.
//
//  Two small shared helpers came out of tracing this body:
//
//    * SerializeOptionalNodeRef (sub_6F3A0950/sub_6F3A0910/sub_6F3A0990 -
//      three /OPT:ICF-less duplicates of the identical body, the same
//      linker artefact cdatastorescratch.h's own WriteDwordAlt2/3 document)
//      writes a node's own +0xC/+0x10 dwords, or -1 twice for a null node -
//      called once for m_gameSlotValue and once per live m_playerSlots[i]
//      entry.
//    * SerializeNestedRefTable (sub_6F3A4460) walks the small nested
//      count/pointer table at m_field98..m_fieldA4 and writes every leaf
//      through SerializeOptionalNodeRef.
//
//  Left as naked thunks (own translation unit not worth the depth for a
//  save-format field this checksum-family batch was not asked to chase
//  further): sub_6F3A6490 (the m_field20 IdStringReg hash-table walk, its
//  own /GS-framed 0x144-byte body - see gamewar3.h's own comment on
//  m_field20), sub_6F3A3B60 (one AllySlot element's own writer, which
//  calls two more not-dumped subs on its own +0x00/+0x04/+0x08), and the
//  two GameUI-adjacent fragments near the tail (sub_6F2F6020, a plain
//  method call on the GetGameUI(1,0) singleton; sub_6F3339E0/sub_6F3A22D0,
//  the RCString-array writer at m_field3F0).
//============================================================================
#include "gamewar3.h"
#include "cdatastorescratch.h"
#include "agent.h"

extern "C" {

#define ADDR_THUNK(sig, addr)                     \
    __declspec(naked) sig                         \
    {                                              \
        __asm mov eax, addr                       \
        __asm jmp eax                              \
    }

//  0x6F3A6490 - retn 0.  __fastcall(stream, &field-inside-m_field20).
ADDR_THUNK(void __fastcall GameWar3_SerializeIdStringTable(void*, void*), 0x6F3A6490)

//  0x6F3A3B60 - retn 0.  __fastcall(stream, &AllySlot).
ADDR_THUNK(void __fastcall GameWar3_SerializeAllySlot(void*, void*), 0x6F3A3B60)

//  0x6F3339E0 - retn 0.  __thiscall(&field3F0), no other arguments.
ADDR_THUNK(void __fastcall GameWar3_PrepareRCStringArray(void*), 0x6F3339E0)

//  0x6F3A22D0 - retn 0.  __fastcall(stream, &field3F0).
ADDR_THUNK(void __fastcall GameWar3_SerializeRCStringArray(void*, void*), 0x6F3A22D0)

}

//  0x6F2F6020 - retn 4.  __thiscall(gameUI, stream) - one stack argument,
//  not a second register, so this is a struct member (this in ecx, stream
//  on the stack) rather than a free `__fastcall` function, which would put
//  `stream` in edx instead of on the stack.
struct SGameUIFieldWriteReceiver
{
    void SerializeField(void* stream);
};
__declspec(naked) void SGameUIFieldWriteReceiver::SerializeField(void*)
{
    __asm mov eax, 06F2F6020h
    __asm jmp eax
}

//  0x6F300710 - GetGameUI(create, reset), already reconstructed
//  (GameUI/gameui.h).  Spelled *exactly* as that header declares it
//  (`SGameUI*` return, no `extern "C"`) rather than pulling the whole
//  header in for one call - MSVC's C++ name mangling folds the return type
//  into the symbol, so a second spelling here would be a second symbol for
//  one shipped address, the defect docs/notes/verifier-gate-link-failures.md
//  catalogues.
struct SGameUI;
SGameUI* __fastcall GetGameUI(int create, int reset);

//  sub_6F3A0950/sub_6F3A0910/sub_6F3A0990 and sub_6F3A4460 - real bodies in
//  their own translation units now (gamewar3serializegameslotref.cpp,
//  gamewar3serializeplayerslotref.cpp, gamewar3serializefield400ref.cpp,
//  gamewar3serializenestedreftable.cpp) - a single `static`/non-static
//  definition living in this file scored badly (~0.44 on `Save`) because
//  /Ob2 inlined it at every one of its own ~30 call sites instead of
//  leaving a real call, the class of gap CLAUDE.md's own house rules call
//  out ("put a callee in its own TU rather than reaching for
//  __declspec(noinline)").
CDataStoreScratch* __fastcall SerializeGameSlotRef(CDataStoreScratch* stream,
                                                     CAgent* const* ref);
CDataStoreScratch* __fastcall SerializePlayerSlotRef(CDataStoreScratch* stream,
                                                       CAgent* const* ref);
CDataStoreScratch* __fastcall SerializeField400Ref(CDataStoreScratch* stream,
                                                     CAgent* const* ref);

struct SGameWar3RefTable;
CDataStoreScratch* __fastcall SerializeNestedRefTable(CDataStoreScratch* stream,
                                                        SGameWar3RefTable* header);

//  m_field30's own vtable slot 1 (offset 4) and m_field34's own vtable slot
//  2 (offset 8) - both raw slot dispatches, same reasoning as
//  DumpPlayerDebugLog/FormatWar3BuildLog's own in gamewar3.cpp: neither
//  receiver's class is established anywhere in this repo.
typedef void (__thiscall *GameWar3SubObjectWriteFn)(void* self, void* stream);

void CGameWar3::Save(CDataStoreScratch* stream)
{
    stream->WriteDword(m_field04);
    stream->WriteDword(m_currentJassInstanceSlot);

    SerializeGameSlotRef(stream, (CAgent* const*)&m_gameSlotValue);

    GameWar3_SerializeIdStringTable(stream, (char*)m_field20 + 4);
    GameWar3_SerializeIdStringTable(stream, (char*)m_field20 + 0x54);
    GameWar3_SerializeIdStringTable(stream, (char*)m_field20 + 0x7C);

    stream->WriteWord(m_field28);
    stream->WriteWord(m_field2C);

    ((GameWar3SubObjectWriteFn)(*(void***)m_field30)[1])(m_field30, stream);
    ((GameWar3SubObjectWriteFn)(*(void***)m_field34)[2])(m_field34, stream);

    stream->WriteDword(m_fieldA8);
    for (unsigned int i = 0; i < m_fieldA8; ++i)
        GameWar3_SerializeAllySlot(stream, &m_allySlots[i]);

    stream->WriteDwordAlt(m_field44);
    stream->WriteDwordAlt(m_field48);
    stream->WriteDwordAlt(m_field4C);

    stream->WriteDword(m_field54);
    for (unsigned int i = 0; i < m_field54; ++i)
        SerializePlayerSlotRef(stream, (CAgent* const*)&m_playerSlots[i]);

    stream->WriteDwordAlt(m_field50);

    SerializeNestedRefTable(stream, (SGameWar3RefTable*)&m_field98);

    stream->WriteDwordAlt3(m_field3C0);

    void* gameUI = (void*)GetGameUI(1, 0);
    ((SGameUIFieldWriteReceiver*)gameUI)->SerializeField(stream);

    void* heroBar = *(void**)((char*)gameUI + 0x3DC);
    stream->WriteDwordAlt(*(unsigned int*)((char*)heroBar + 0x180));

    stream->WriteDwordAlt3(*(unsigned int*)((char*)gameUI + 0x28C));

    stream->WriteDwordAlt3(m_field334);
    stream->WriteDwordAlt3(m_field338);
    stream->WriteDwordAlt3(m_field3C4);

    GameWar3_PrepareRCStringArray(&m_field3F0);
    GameWar3_SerializeRCStringArray(stream, &m_field3F0);

    SerializeField400Ref(stream, (CAgent* const*)&m_field400);

    stream->WriteDword(m_field2BC);
    for (unsigned int i = 0; i < m_field2BC; ++i)
        stream->WriteDwordAlt(m_field2C0[i]);
}
