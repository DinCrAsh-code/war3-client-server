//============================================================================
//  0x6F2777B0 / 0x6F277850 / 0x6F2778B0 - the three per-hero value terms the
//  ranking score adds up (Player/playerheroselect.cpp).
//
//  All three have the same shape.  A base figure comes out of the unit-type
//  record at +0x1F0 - a published counter scaled by one of the record's own
//  CFloats and truncated back to an integer, plus a flat field - and then,
//  unless the caller asked for the summary only, one of the three int
//  totals the accumulation walk leaves behind is subtracted from it.
//
//  Which record fields and which total distinguish them:
//
//      0x6F2777B0   scale +0xD4, flat +0x94, total +0x50
//      0x6F2778B0   scale +0xE4, flat +0xA8, total +0x54
//      0x6F277850   the whole base figure comes from 0x6F0DB3A0
//                   (Unit/unit_herovaluebase.cpp) instead, total +0x58
//
//  The two 0x5C-byte blocks are stack locals, initialised in place and
//  handed to the walk; only the first one's totals are ever read back.
//
//  All three share a translation unit: none calls another, they are one
//  module in the image, and every one of their callees has to stay a real
//  call.
//============================================================================
#include "unitvalueblock.h"
#include "playerscore.h"       // HandleRefFieldOwner_6F473170
#include "playerheroeval.h"    // SHeroValueSource

//  0x6F0DB3A0 - Unit/unit_herovaluebase.cpp.
int __fastcall UnitRecordValueBase(void* record);

struct SUnitTypeRecordValue
{
    char   m_reserved000[0x6C];
    HandleRefFieldOwner_6F473170 m_counter6C;   // +0x6C
    char   m_reserved07C[0x94 - 0x7C];
    int    m_flat94;      // +0x94
    char   m_reserved098[0xA8 - 0x98];
    int    m_flatA8;      // +0xA8
    char   m_reserved0AC[0xD4 - 0xAC];
    CFloat m_scaleD4;     // +0xD4
    char   m_reserved0D8[0xE4 - 0xD8];
    CFloat m_scaleE4;     // +0xE4
};

struct SUnitValueHost
{
    char                  m_reserved000[0x1F0];
    SUnitTypeRecordValue* m_pRecord;   // +0x1F0
};

int SHeroValueSource::TermA(int summaryOnly)
{
    SUnitTypeRecordValue* record = ((SUnitValueHost*)this)->m_pRecord;

    //  The scale is copied into a local first: the shipped code loads
    //  [record+0D4h] into a slot and passes that slot's address, which is
    //  what a `CFloat` by value gives.
    CFloat scale = record->m_scaleD4;
    int flat = record->m_flat94;

    //  The counter is named rather than left inside the conversion: written
    //  inline, MSVC hoists operator*'s own `push &scale` above the
    //  QueryField78 call, where the shipped stream pushes it after.
    int counter = record->m_counter6C.QueryField78();

    int value = CFloatToInt(CFloatFromInt(counter) * scale) + flat;
    if (summaryOnly != 0)
        return value;

    SUnitValueBlock first(kCFloatNoInit);
    SUnitValueBlock second(kCFloatNoInit);
    first.Init();
    second.Init();
    ((SUnitValueSource*)this)->Accumulate(&first, &second);

    return value - first.m_total50;
}

int SHeroValueSource::TermB(int summaryOnly)
{
    int value = UnitRecordValueBase(((SUnitValueHost*)this)->m_pRecord);
    if (summaryOnly != 0)
        return value;

    SUnitValueBlock first(kCFloatNoInit);
    SUnitValueBlock second(kCFloatNoInit);
    first.Init();
    second.Init();
    ((SUnitValueSource*)this)->Accumulate(&first, &second);

    return value - first.m_total58;
}

int SHeroValueSource::TermC(int summaryOnly)
{
    SUnitTypeRecordValue* record = ((SUnitValueHost*)this)->m_pRecord;

    CFloat scale = record->m_scaleE4;
    int flat = record->m_flatA8;

    //  The counter is named rather than left inside the conversion: written
    //  inline, MSVC hoists operator*'s own `push &scale` above the
    //  QueryField78 call, where the shipped stream pushes it after.
    int counter = record->m_counter6C.QueryField78();

    int value = CFloatToInt(CFloatFromInt(counter) * scale) + flat;
    if (summaryOnly != 0)
        return value;

    SUnitValueBlock first(kCFloatNoInit);
    SUnitValueBlock second(kCFloatNoInit);
    first.Init();
    second.Init();
    ((SUnitValueSource*)this)->Accumulate(&first, &second);

    return value - first.m_total54;
}
