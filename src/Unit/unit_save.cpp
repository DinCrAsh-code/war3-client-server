//============================================================================
//  0x6F278660 - CUnit's vtable slot 14 (+0x38), Save.  See unit.h.
//
//  CSelectable::Save(store) first (a real call, not a vtable dispatch - the
//  override calls its own base directly, the same shape widget_save.cpp and
//  selectable_save.cpp already use), then CUnit's own record: two runs of
//  CAgent::Method_0x50 announcing a fixed range of message ids (0x80234..
//  0x80258, then 0x80319..0x80329 - 54 in all), the two embedded
//  CAgentTimers' own save records (through a vtable slot neither
//  agenttimer.h nor any earlier target had reached before - see the note
//  on CAgentTimer's slot 8 below), and then a long, flat run of field
//  writes with no control flow of their own beyond the nine refcounted
//  pointers that publish either a resolved {handle,type} pair or {-1,-1}
//  when unset.
//
//  The field order is the shipped order, not declaration order - the same
//  "not tidy" property unit_dumpstate.cpp's own header already notes for
//  DumpState.  Every write is commented with the field(s) it reaches.
//
//  +0x30C is read and published (WriteDwordAlt2) right after m_ref304's own
//  two words, one dword past where unit.h's own field table and its two
//  "runs from +0x54 to 0x30C" sources (DumpState's 45 fields, ~CUnit's own
//  reverse-offset destructor walk) say the class ends. Neither of those two
//  sources is exhaustive by construction - DumpState only prints fields
//  worth printing and ~CUnit only visits fields with a destructor - so this
//  is read by raw offset off `this` rather than promoted to a named field:
//  real evidence that one more dword exists here, not evidence of what it
//  is for.
//
//  Four FloatMini values (+0xB0, +0xD4, +0xE0, +0xCC) and the FloatMiniB's
//  own val (+0x208) go out through a *local copy* - `mov eax,[edi+off]`
//  into a stack scratch slot, only then `lea`'d and handed to WriteCFloat -
//  where the FloatMiniB's own min/max (+0x20C/+0x210) and every CFloat in
//  the +0x228 reserved run are addressed directly off the field with no
//  copy.  That is a real, dump-confirmed asymmetry (a plain `const CFloat*`
//  onto the member produces the direct-address form every time it is
//  tried), so those five are spelled as a `CFloat v = *(...); WriteCFloat(
//  store, &v);` local rather than a straight pointer cast.
//
//  Own translation unit: nothing else in this call tree needs a real call
//  out of this file beyond the writers themselves, all already
//  reconstructed (cdatastorescratch.h, cdatastore_writecfloat.cpp,
//  cdatastore_writerefhandlepair.cpp) and CAgent::Method_0x50
//  (agent_slot20.cpp).
//============================================================================
#include "unit.h"
#include "cdatastorescratch.h"

CDataStoreScratch* __fastcall WriteCFloat(CDataStoreScratch* store,
                                           const CFloat* value);
//  0x6F03E620 - cdatastore_writerefhandlepair.cpp.  A different
//  {handle,type} offset pair (+0xC/+0x10 on the resolved pointer) from the
//  inline {-1,-1}-or-{[+8],[+0xC]} shape this file's other refcounted
//  pointers use - see that file for why it is its own function rather than
//  a duplicate of the inline one.
CDataStoreScratch* __fastcall WriteRefHandlePair(CDataStoreScratch* store,
                                                  void** pRef);

//  CAgentTimer's own vtable slot 8 (+0x20) - reached here for the first
//  time in this repo: agenttimer.h only ever needed TRefCnt's own two
//  slots (ReleaseSelf/DeleteSelf) before, because nothing else in any
//  earlier target dispatched through a CAgentTimer's vtable at all.  Both
//  m_timer68's sibling members (+0x7C, +0x180) reach it here with the
//  identical `mov eax,[obj]; mov edx,[eax+20h]; lea ecx,obj; push store;
//  call edx` shape, which is what says it is real and not two coincidental
//  one-off calls - but nothing establishes what the other six slots
//  (2..7) between TRefCnt's own and this one do, so this is a hand-typed
//  dispatch (CLAUDE.md's own pattern for an unmodeled vtable) rather than
//  a widened `virtual` on the class itself, which would have to invent
//  placeholders for all six to keep this slot's offset right.
typedef void (__thiscall *AgentTimerSaveFn)(void*, CDataStoreScratch*);

//  The refcounted-pointer / handle-pair idiom every plain `void*` field in
//  this class shares: publish the resolved object's own +0x08/+0x0C, or
//  {-1,-1} when the pointer is unset.  Written as a real out-of-line
//  helper here (unlike the inline copies elsewhere in this repo) because
//  it recurs nine times in this one function with nothing else between
//  occurrences to make each one a genuinely different shape - a shared
//  function costs this call site nothing the inlined copies do not also
//  pay, and keeps the body from being three times longer than the
//  instruction stream it has to match.  `store` is threaded through
//  rather than closed over so this stays a plain function, not a member.
static void SaveRefHandlePair(CDataStoreScratch* store, void* p)
{
    if (p == 0)
    {
        store->WriteDword((unsigned int)-1);
        store->WriteDword((unsigned int)-1);
    }
    else
    {
        store->WriteDword(*(unsigned int*)((char*)p + 8));
        store->WriteDword(*(unsigned int*)((char*)p + 0xC));
    }
}

void CUnit::Save(CDataStoreScratch* store)
{
    CSelectable::Save(store);

    //  Two runs of the same announcement, back to back with no field
    //  behind either loop counter - just two fixed ranges of message ids.
    for (unsigned int id = 0x80234; id < 0x80259; ++id)
        Method_0x50(store, id, 0x400);
    for (unsigned int id = 0x80319; id < 0x8032A; ++id)
        Method_0x50(store, id, 0x400);

    ((AgentTimerSaveFn)(*(void***)&m_timer7C)[0x20 / 4])(&m_timer7C, store);

    //  +0x130/+0x134 (m_ref130, a handle pair) and +0x138 (m_int138).
    store->WriteDword(m_ref130.m_t);
    store->WriteDword(m_ref130.m_b);
    store->WriteDwordAlt2(m_int138);
    //  +0x13C/+0x140 (m_ref13C, a handle pair) and the first two dwords of
    //  the +0x144 reserved gap - unnamed, so read by raw offset.
    store->WriteDword(m_ref13C.m_t);
    store->WriteDword(m_ref13C.m_b);
    store->WriteDwordAlt2(*(unsigned int*)(m_reserved144 + 0));
    store->WriteDwordAlt3(*(unsigned int*)(m_reserved144 + 4));

    //  +0x110 - a refcounted pointer.
    SaveRefHandlePair(store, m_pRef110);

    //  +0x10C - only the low word goes out; nothing else in this call tree
    //  reads the field's own upper two bytes.
    store->WriteWord((unsigned short)m_int10C);

    //  +0x174/+0x178 (m_ref174, a handle pair) and +0x17C (m_int17C).
    store->WriteDword(m_ref174.m_t);
    store->WriteDword(m_ref174.m_b);
    store->WriteDwordAlt2(m_int17C);

    ((AgentTimerSaveFn)(*(void***)&m_timer180)[0x20 / 4])(&m_timer180, store);

    //  +0x19C/+0x1A0 (m_ref19C) and +0x1A4 (m_int1A4).
    store->WriteDword(m_ref19C.m_t);
    store->WriteDword(m_ref19C.m_b);
    store->WriteDwordAlt2(m_int1A4);
    //  +0x1A8/+0x1AC (m_ref1A8) and +0x1B0 (m_int1B0).
    store->WriteDword(m_ref1A8.m_t);
    store->WriteDword(m_ref1A8.m_b);
    store->WriteDwordAlt2(m_int1B0);
    //  +0x1DC/+0x1E0 (m_ref1DC, the ability-list handle pair) and the first
    //  dword of the +0x1E4 reserved gap.
    store->WriteDword(m_ref1DC.m_t);
    store->WriteDword(m_ref1DC.m_b);
    store->WriteDwordAlt2(*(unsigned int*)(m_reserved1E4 + 0));

    //  Four back-to-back {-1, -1, 0} triples with no field behind any of
    //  them - not a loop in the shipped code (each is a separate unrolled
    //  push/call run), and no memory is read for any of the twelve values,
    //  so nothing here is guessed at by picking an offset: these are
    //  literal placeholder records, most likely four empty order/ability
    //  save slots given the module they sit in, but nothing in this call
    //  tree confirms that reading.
    store->WriteDword((unsigned int)-1);
    store->WriteDword((unsigned int)-1);
    store->WriteDwordAlt2(0);
    store->WriteDword((unsigned int)-1);
    store->WriteDword((unsigned int)-1);
    store->WriteDwordAlt2(0);
    store->WriteDword((unsigned int)-1);
    store->WriteDword((unsigned int)-1);
    store->WriteDwordAlt2(0);
    store->WriteDword((unsigned int)-1);
    store->WriteDword((unsigned int)-1);
    store->WriteDwordAlt2(0);

    //  +0x250/+0x254 (m_ref250, a handle pair) and +0x258 (m_int258).
    store->WriteDword(m_ref250.m_t);
    store->WriteDword(m_ref250.m_b);
    store->WriteDwordAlt2(m_int258);
    //  +0x304/+0x308 (m_ref304, a handle pair) and +0x30C - one dword past
    //  the class's own believed end; see the file header.
    store->WriteDword(m_ref304.m_t);
    store->WriteDword(m_ref304.m_b);
    store->WriteDwordAlt2(*(unsigned int*)((char*)this + 0x30C));

    //  +0xA8 (m_pRefA8, a refcounted pointer) and +0xB0 (the FloatMini at
    //  +0xAC's own value word).  Read into a local first, not addressed
    //  straight off the field - see the file header.
    SaveRefHandlePair(store, m_pRefA8);
    {
        CFloat v = *(const CFloat*)(m_fltAC + 4);
        WriteCFloat(store, &v);
    }
    //  +0xB4 (m_pRefB4) and +0xD4 (the FloatMini at +0xD0's own value).
    SaveRefHandlePair(store, m_pRefB4);
    {
        CFloat v = *(const CFloat*)(m_fltD0 + 4);
        WriteCFloat(store, &v);
    }
    //  +0xD8 (m_pRefD8) and +0xE0 (the FloatMini at +0xDC's own value).
    SaveRefHandlePair(store, m_pRefD8);
    {
        CFloat v = *(const CFloat*)(m_fltDC + 4);
        WriteCFloat(store, &v);
    }
    //  +0xCC - the FloatMini at +0xC8's own value, on its own with no
    //  refcounted pointer ahead of it this time.
    {
        CFloat v = *(const CFloat*)(m_fltC8 + 4);
        WriteCFloat(store, &v);
    }

    //  +0x58, +0x5C, +0x64 - not +0x60, which this function never reaches
    //  here (it comes much later, out of order, near the very end).
    store->WriteDword(m_owningPlayer58);
    store->WriteDword(m_flags5C);
    store->WriteDword(m_int64);
    //  +0xE8, +0xEC.
    store->WriteDword(m_intE8);
    store->WriteDword(m_intEC);
    //  +0x54.
    store->WriteDword(m_int54);
    //  +0x114.
    store->WriteDword(m_int114);
    //  +0x150, +0x154.
    store->WriteDword(m_int150);
    store->WriteDword(m_int154);
    //  +0x198.
    store->WriteDword(m_int198);
    //  +0x1B8, +0x1BC, +0x1C0, +0x1C4, +0x1C8, +0x1CC, +0x1D4, +0x1D8 - all
    //  eight through the same writer, none of them +0x1D0 (also saved,
    //  separately, near the very end).
    store->WriteDword(m_int1B8);
    store->WriteDword(m_int1BC);
    store->WriteDword(m_int1C0);
    store->WriteDword(m_int1C4);
    store->WriteDword(m_int1C8);
    store->WriteDword(m_int1CC);
    store->WriteDword(m_int1D4);
    store->WriteDword(m_int1D8);
    //  +0x194.
    store->WriteDword(m_int194);

    //  +0x1FC, +0x200.
    store->WriteDword(m_int1FC);
    store->WriteDword(m_int200);
    //  +0x208/+0x20C/+0x210 - the FloatMiniB at +0x204's own {val, min,
    //  max} triple ([FltMiniB]); max is named m_bound210 in its own right
    //  (see unit.h's note on why).  Only val goes out through a local copy,
    //  the same shape the four standalone FloatMini values above use; min
    //  and max are addressed directly off the sub-object itself (`min`'s
    //  own address, then `min`'s address plus 4 for `max`) with no copy.
    {
        CFloat v = *(const CFloat*)(m_fltB204 + 4);
        WriteCFloat(store, &v);
    }
    WriteCFloat(store, (const CFloat*)(m_fltB204 + 8));
    WriteCFloat(store, &m_bound210);

    //  +0x248, +0x24C, +0xE4, +0x240, +0x244 - not in offset order.
    store->WriteDword(m_int248);
    store->WriteDword(m_int24C);
    store->WriteDword(m_intE4);
    store->WriteDword(m_int240);
    store->WriteDword(m_int244);

    //  +0x128 (m_ref128, SOwnedAgentRef), +0x12C (m_ref12C,
    //  SOwnedQueueRef), +0x224 (m_ref224, SOwnedAgentRef) - the same
    //  refcounted-pointer shape as the plain `void*` fields above, off
    //  each one's own `m_p`.
    SaveRefHandlePair(store, m_ref128.m_p);
    SaveRefHandlePair(store, m_ref12C.m_p);
    SaveRefHandlePair(store, m_ref224.m_p);

    //  +0x228, and +0x230/+0x234/+0x238 - four CFloats out of the +0x228
    //  reserved gap; the other two dwords in that 0x18-byte block are not
    //  reached here.
    WriteCFloat(store, (const CFloat*)(m_reserved228 + 0x00));
    WriteCFloat(store, (const CFloat*)(m_reserved228 + 0x08));
    WriteCFloat(store, (const CFloat*)(m_reserved228 + 0x0C));
    WriteCFloat(store, (const CFloat*)(m_reserved228 + 0x10));

    //  +0x26C, +0x270 - two flag bytes out of the +0x264 reserved gap.
    store->WriteByte((unsigned char)(*(unsigned int*)(m_reserved264 + 0x08) != 0));
    store->WriteByte((unsigned char)(*(unsigned int*)(m_reserved264 + 0x0C) != 0));
    //  +0x27C.
    store->WriteDwordAlt3(m_int27C);

    //  +0x2B8/+0x2BC/+0x2C0 - ClearMotionState's own three floats, as
    //  hardware floats (an `fld`/writer pair, not through WriteCFloat).
    store->WriteFloat(m_motion2B8);
    store->WriteFloat(m_motion2BC);
    store->WriteFloat(m_motion2C0);

    //  +0x2C4 - the resolved object's own +0xC/+0x10, or {-1,-1}; the one
    //  refcounted pointer that goes through the shared out-of-line writer
    //  instead of the +8/+0xC shape every other one here uses (see that
    //  file for why).
    WriteRefHandlePair(store, &m_pRef2C4);

    //  +0x280 - only bit 0, as a byte (slot 56 reads the same field's
    //  bit 1; this is a different bit of it).
    store->WriteByte((unsigned char)(m_flags280 & 1));

    //  +0x2D4, +0x2D8.
    store->WriteDword(m_int2D4);
    store->WriteDwordAlt(m_animMode2D8);

    //  +0x2E0..+0x2F4 - six consecutive dwords, starting at m_animName2E0
    //  and running on into the +0x2E4 reserved gap, all through the one
    //  writer and all in a real loop (the shipped code counts down from 6,
    //  not an unrolled run like the {-1,-1,0} triples above) - so this is
    //  the one place in this function where the loop shape itself is part
    //  of what has to match, not just its total effect.
    {
        unsigned int* p = (unsigned int*)((char*)this + 0x2E0);
        for (int i = 0; i < 6; ++i)
            store->WriteDwordAlt3(p[i]);
    }

    //  +0x2F8/+0x2FC/+0x300 - three more hardware floats, the tail of the
    //  +0x2E4 reserved gap.
    store->WriteFloat(*(float*)((char*)this + 0x2F8));
    store->WriteFloat(*(float*)((char*)this + 0x2FC));
    store->WriteFloat(*(float*)((char*)this + 0x300));

    //  +0x158, +0x15C, +0x160.
    store->WriteDword(m_int158);
    store->WriteDword(m_int15C);
    store->WriteDword(m_int160);
    //  +0x60 - out of offset order, same as +0xE4 above.
    store->WriteDword(m_int60);
    //  +0x1D0.
    store->WriteDword(m_int1D0);
    //  +0x280 again, this time the whole dword - a genuine second write of
    //  the same field the byte-sized write above already covered once.
    store->WriteDwordAlt(m_flags280);
}
