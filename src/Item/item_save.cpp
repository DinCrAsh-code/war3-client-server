//============================================================================
//  0x6F2B69B0 - CItem's override of vtable slot 14 (+0x38), CSelectable's
//  Save plus every one of CItem's own persistent members.
//
//  In order: the base, the three FloatMiniB CFloats (value, min, max), the
//  Position record's own five dwords at +0x84..+0x94, its +0x74, the
//  four-triple record array at +0x98, the first CAgentTimer through its
//  vtable slot 8, the Position word at +0xCC, the one at +0x80, and finally
//  the second CAgentTimer.
//
//  Four *different* four-byte writers are used across those nine dwords
//  (0x6F4C2360, 0x6F4C2310, 0x6F4C2270 and - through WriteCFloat -
//  0x6F4C22C0), all four byte-for-byte identical.  That is not a
//  transcription slip: this image was linked without /OPT:ICF, so the four
//  survive as four (cdatastorescratch.h).
//
//  Own translation unit: eleven real calls out of it.
//============================================================================
#include "item.h"
#include "cdatastorescratch.h"

//  0x6F6EFE00 - cdatastore_writecfloat.cpp.
CDataStoreScratch* __fastcall WriteCFloat(CDataStoreScratch* store,
                                          const CFloat* value);
//  0x6F2B3130 - item_saverecords.cpp.
CDataStoreScratch* __fastcall WriteRecordTriples(CDataStoreScratch* store,
                                                 const void* array);

//  CAgentTimer's vtable slot 8 - its own save.  Reached by offset for the
//  same reason widget.h's slots are.
typedef void (__thiscall *TimerSlot8Fn)(void* self, CDataStoreScratch* store);

void CItem::Save(CDataStoreScratch* store)
{
    CSelectable::Save(store);

    //  m_value copied into a scratch slot first and written from there: the
    //  shipped code reads [this+58h] into a register and spills it before
    //  the call, which is what passing a copy by address does and what
    //  `&m_floatB.m_value` does not.
    CFloat value = m_floatB.m_value;
    WriteCFloat(store, &value);
    //  The pair addressed off one pointer, not two independent member
    //  addresses: the shipped code holds &m_min in a callee-saved register
    //  and reaches m_max as `[ebx+4]`, which is what `pair`/`pair + 1`
    //  gives and what two `lea`s off `this` do not.
    const CFloat* pair = &m_floatB.m_min;
    WriteCFloat(store, pair);
    WriteCFloat(store, pair + 1);

    store->WriteDword(m_position.m_reserved84);
    store->WriteDword(m_position.m_reserved88);
    store->WriteDword(m_position.m_placementRef.m_handle);
    store->WriteDword((unsigned int)m_position.m_placementRef.m_typeTag);
    store->WriteDwordAlt2(m_position.m_reserved94);
    store->WriteDword(m_position.m_reserved74);

    //  +0x98 is the count and +0x9C the four {a,b,c} triples behind it -
    //  see position.h.  Reached as one object because that is what the
    //  writer takes.
    WriteRecordTriples(store, &m_position.m_reserved98);

    ((TimerSlot8Fn)(*(void***)&m_timer1)[8])(&m_timer1, store);

    store->WriteDword(m_position.m_reservedCC);
    store->WriteDwordAlt3(m_position.m_reserved80);

    ((TimerSlot8Fn)(*(void***)&m_timer2)[8])(&m_timer2, store);
}
