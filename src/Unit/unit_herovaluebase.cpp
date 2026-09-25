//============================================================================
//  0x6F0DB3A0 - the middle term's base figure, out of the unit-type record.
//
//  Two published counters and one CFloat scale off the same record: the one
//  at +0x6C scaled by +0xDC and truncated, plus the one at +0x7C flat.  The
//  +0x7C read happens *first* in the shipped stream and its answer is held
//  in a callee-saved register across the whole conversion, which is what
//  naming it before the scaled term produces.
//
//  Own translation unit: 0x6F277850 calls it for real.
//============================================================================
#include "unitvalueblock.h"
#include "playerscore.h"       // HandleRefFieldOwner_6F473170

struct SUnitRecordValueFields
{
    char   m_reserved000[0x6C];
    HandleRefFieldOwner_6F473170 m_counter6C;   // +0x6C
    HandleRefFieldOwner_6F473170 m_counter7C;   // +0x7C
    char   m_reserved08C[0xDC - 0x8C];
    CFloat m_scaleDC;      // +0xDC
};

int __fastcall UnitRecordValueBase(void* record)
{
    SUnitRecordValueFields* self = (SUnitRecordValueFields*)record;

    CFloat scale = self->m_scaleDC;
    int flat = self->m_counter7C.QueryField78();

    //  The counter is named rather than left inside the conversion: written
    //  inline, MSVC hoists operator*'s own `push &scale` above the second
    //  QueryField78 call, where the shipped stream pushes it after.
    int counter = self->m_counter6C.QueryField78();

    return CFloatToInt(CFloatFromInt(counter) * scale) + flat;
}
