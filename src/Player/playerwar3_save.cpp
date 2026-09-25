//============================================================================
//  0x6F415BF0 - CPlayerWar3::Save, vtable slot 14 (+0x38).  See playerwar3.h.
//
//  Base call, three fixed message-id sweeps through slot 20 (CAgentWar3's
//  own Method_0x50, exactly the shape CAgentWar3::Save's own tail already
//  established in agentwar3_savemessages.cpp - two loops and two singles
//  here instead of one loop, all still literal id ranges in the shipped
//  code, not counts read from anywhere), then a long, mechanical run of
//  field writes and three sub-object ::Save calls (CSelectionWar3,
//  CTechTree, CAgentTimer) in the object's own address order - the same
//  "flat field dump" shape DumpState (playerwar3_dumpstate.cpp) already
//  established for this class, just writing to the wire instead of a text
//  sink, and reaching two fields (+0x24, +0x310) that are RCStrings rather
//  than SIntMiniValue/FloatMini.
//
//  Own translation unit: every call in it (CAgentWar3::Save, three
//  CSelectionWar3::Save-closure functions, one CTechTree::Save, one
//  BestHeroData writer, a dozen-plus CDataStoreScratch writers, one
//  CAgentTimer vtable-slot-8 dispatch) is real.
//============================================================================
#include "playerwar3.h"
#include "cdatastorescratch.h"
#include "integerlistener.h"
#include "CFloat.h"

//  0x6F6EFE00 - Net/cdatastore_writecfloat.cpp.
CDataStoreScratch* __fastcall WriteCFloat(CDataStoreScratch* store,
                                          const CFloat* value);

//  Slot 20 (+0x50), reached through the vtable rather than by name - a
//  base-class body any of the six classes below it may override.  Same
//  typedef agentwar3_savemessages.cpp already declares for its own call
//  site.
typedef void (__thiscall *Slot0x50Fn)(void* self, CDataStoreScratch* store,
                                      unsigned int msgId, int count);

//  playerwar3_savewritehandlepair.cpp.
CDataStoreScratch* __fastcall WriteAgentHandlePairDupA(
        CDataStoreScratch* store, CAgent** slot);
CDataStoreScratch* __fastcall WriteAgentHandlePairDupC(
        CDataStoreScratch* store, CAgent** slot);

//  0x6F415530 - append `count` = the leading dword at `arr+4` and then that
//  many 0xC-byte BestHeroDataElement entries out of the pointer at `arr+8`,
//  three raw dwords each.  `arr` itself (`arr+0`) is never read - the
//  caller passes `&this->m_bestHeroData`, whose own +0x0 "alloc" field this
//  function's own body skips straight past, matching TSFixedArray<T>'s own
//  three-field layout (storm.h) exactly.  `retn 0`.
CDataStoreScratch* __fastcall PlayerWar3WriteBestHeroDataArray(
        CDataStoreScratch* store, void* arr)
{
    char* base = (char*)arr;
    unsigned int count = *(unsigned int*)(base + 4);
    store->WriteDword(count);

    //  The shipped stream reloads the data pointer (`arr+8`) on every
    //  iteration, walking it by an accumulated byte offset rather than a
    //  scaled index - reproduced literally, the same reload idiom the
    //  dead-hero-array loop above uses.
    unsigned int byteOffset = 0;
    for (unsigned int i = 0; i < count; i++)
    {
        char* elem = *(char**)(base + 8) + byteOffset;
        store->WriteDword(*(unsigned int*)(elem + 0));
        store->WriteDword(*(unsigned int*)(elem + 4));
        store->WriteDword(*(unsigned int*)(elem + 8));
        byteOffset += 0x0C;
    }

    return store;
}

void CPlayerWar3::Save(CDataStoreScratch* store)
{
    CAgentWar3::Save(store);

    for (unsigned int msgId = 0x8020B; msgId < 0x80234; msgId++)
        ((Slot0x50Fn)(*(void***)this)[0x50 / 4])(this, store, msgId, 0x400);
    for (unsigned int msgId = 0x80305; msgId < 0x8030D; msgId++)
        ((Slot0x50Fn)(*(void***)this)[0x50 / 4])(this, store, msgId, 0x400);
    for (unsigned int msgId = 0x8030D; msgId < 0x80319; msgId++)
        ((Slot0x50Fn)(*(void***)this)[0x50 / 4])(this, store, msgId, 0x400);
    ((Slot0x50Fn)(*(void***)this)[0x50 / 4])(this, store, 0x80260, 0x400);
    ((Slot0x50Fn)(*(void***)this)[0x50 / 4])(this, store, 0x80261, 0x400);

    //  +0x24: RCString.  Kept as raw bytes in the header (see its own
    //  comment) - GetHandleOrZero (0x6F4C4630, rcstring.h's own note on
    //  this being really RCString::GetCStr) reads the same two fields
    //  either way.
    const char* text = (const char*)GetHandleOrZero(
            (const SHandleHolder*)m_valueNameStorage);
    static const char kValueName[] = "";   // IDA's own name, 0x6F87529C
    store->WriteString(text != 0 ? text : kValueName);

    store->WriteByte(m_flag30);

    m_selectionWar3->Save(store);

    WriteAgentHandlePairDupA(store, (CAgent**)m_allySlotStorage);

    //  +0x1E0/+0x1E4: count, then that many raw SIntMiniValue dwords.
    unsigned int dumpCountA = m_dumpArrayACount;
    store->WriteDword(dumpCountA);
    for (unsigned int i = 0; i < dumpCountA; i++)
        store->WriteDword(m_dumpArrayA[i].m_value);

    unsigned int dumpCountB = m_dumpArrayBCount;
    store->WriteDword(dumpCountB);
    for (unsigned int i = 0; i < dumpCountB; i++)
        store->WriteDword(m_dumpArrayB[i].m_value);

    //  +0x26C is *not* touched here - Save skips straight from +0x268 to
    //  +0x270 in its first pass, the same field DumpState (already EXACT)
    //  reads twice (playerwar3_dumpstate.cpp's own note); Save reads it
    //  exactly once, in its own tail sequence below.
    store->WriteDword(m_reserved248.m_value);
    store->WriteDword(m_reserved24C.m_value);
    store->WriteDword(m_reserved250.m_value);
    store->WriteDword(m_reserved254.m_value);
    store->WriteDword(m_reserved258.m_value);
    store->WriteDword(m_reserved25C.m_value);
    store->WriteDword(m_reserved260.m_value);
    store->WriteDword(m_reserved264.m_value);
    store->WriteDword(m_reserved268.m_value);
    store->WriteDword(m_reserved270.m_value);
    store->WriteDword(m_reserved274.m_value);
    store->WriteDword(m_reserved278.m_value);

    //  +0x27C..+0x294: seven IntegerListener* fields (integerlistener.h -
    //  +0x27C is confirmed IntegerListener-typed by slot 30's own store
    //  into it, playerwar3_slot30.cpp).  Each writes its own two reserved
    //  ints (+0x08/+0x0C), -1/-1 when null - the read offsets (+8/+0xC, not
    //  CAgent's own +0xC/+0x10) are what rule out WriteAgentHandlePairDup*
    //  here.  One shared if/else (not a ternary - a ternary compiles to a
    //  single merged test, where the shipped stream tests once and repeats
    //  both writes on each side) called once per field.
    struct WriteListenerPair
    {
        static void Do(CDataStoreScratch* store, IntegerListener* p)
        {
            if (p == 0)
            {
                store->WriteDword(0xFFFFFFFFu);
                store->WriteDword(0xFFFFFFFFu);
            }
            else
            {
                store->WriteDword((unsigned int)p->m_reserved08);
                store->WriteDword((unsigned int)p->m_reserved0C);
            }
        }
    };
    WriteListenerPair::Do(store, *(IntegerListener**)&m_reserved27C);
    WriteListenerPair::Do(store, *(IntegerListener**)&m_reserved280);
    WriteListenerPair::Do(store, *(IntegerListener**)&m_reserved284);
    WriteListenerPair::Do(store, *(IntegerListener**)((char*)this + 0x288));
    WriteListenerPair::Do(store, *(IntegerListener**)((char*)this + 0x28C));
    WriteListenerPair::Do(store, *(IntegerListener**)&m_reserved290);
    WriteListenerPair::Do(store, *(IntegerListener**)&m_reserved294);

    //  +0x298/+0x2A0: the two embedded FloatMini's own raw CFloat value,
    //  written directly (WriteCFloat) rather than through DumpTo.
    WriteCFloat(store, &m_reserved298.m_value);
    WriteCFloat(store, &m_reserved2A0.m_value);

    //  +0x2AC: the "dead hero" TSGrowableArray<CAgentPtr>.  Kept as raw
    //  bytes in the header (see its own comment); read here the same way.
    unsigned int deadHeroCount = *(unsigned int*)(m_deadHeroArrayStorage + 4);
    store->WriteDword(deadHeroCount);
    //  The shipped stream re-reads the data pointer (`[edi+2B4h]`) on every
    //  iteration rather than hoisting it once - reproduced by re-reading it
    //  here too, which is what makes the loop index-based instead of the
    //  pointer-increment shape a hoisted load would give.
    for (unsigned int i = 0; i < deadHeroCount; i++)
    {
        CAgent** deadHeroes = *(CAgent***)(m_deadHeroArrayStorage + 8);
        WriteAgentHandlePairDupC(store, &deadHeroes[i]);
    }

    //  +0x2C0/+0x2C4: the IntegerMini array.
    unsigned int intMiniCount = m_intMiniCount;
    store->WriteDword(intMiniCount);
    for (unsigned int i = 0; i < intMiniCount; i++)
        store->WriteDword(m_intMiniArray[i].m_value);

    store->WriteDword(m_reserved2CC.m_value);

    m_techTree->Save(store);

    store->WriteDword(m_reserved2D4.m_t);
    store->WriteDword(m_reserved2D4.m_b);

    //  +0x2DC: the raw 4-byte pad field right after m_reserved2D4
    //  (playerwar3.h) - written via WriteDwordAlt2, not WriteDword.
    store->WriteDwordAlt2(*(unsigned int*)((char*)this + 0x2DC));

    store->WriteDword(m_reserved2E0.m_value);

    PlayerWar3WriteBestHeroDataArray(store, &m_bestHeroData);

    store->WriteDword(m_reserved2F0);
    store->WriteDword(m_reserved2F4);
    store->WriteDwordAlt2(m_reserved2F8);

    //  +0x2FC: CAgentTimer, own vtable slot 8 (+0x20).  Kept as a raw slot
    //  dispatch (see playerwar3.h's own comment) - CAgentTimer's own
    //  vtable is not otherwise reconstructed past slots 0/1.
    typedef void (__thiscall *AgentTimerSlot8Fn)(void* self, CDataStoreScratch* store);
    void* timer = m_bestHeroTimerStorage;
    ((AgentTimerSlot8Fn)(*(void***)timer)[0x20 / 4])(timer, store);

    //  +0x31C/+0x320/+0x324: three raw dwords via WriteDwordAlt3, still
    //  inside the not-otherwise-modelled +0x31C..+0x32B gap (playerwar3.h).
    store->WriteDwordAlt3(*(unsigned int*)((char*)this + 0x31C));
    store->WriteDwordAlt3(*(unsigned int*)((char*)this + 0x320));
    store->WriteDwordAlt3(*(unsigned int*)((char*)this + 0x324));

    //  +0x310: a second RCString, same fallback as +0x24.  Kept raw.
    const char* text2 = (const char*)GetHandleOrZero(
            (const SHandleHolder*)m_reserved310Storage);
    store->WriteString(text2 != 0 ? text2 : kValueName);

    store->WriteDword(m_reserved26C.m_value);
    store->WriteDword(m_reserved2A8.m_value);
    store->WriteDword(m_reserved32C.m_value);
    store->WriteDword(m_reserved330.m_value);
    //  +0x328: a fourth raw dword out of the same +0x31C..+0x32B gap.
    store->WriteDword(*(unsigned int*)((char*)this + 0x328));
    store->WriteDword(m_reserved334.m_value);
}
