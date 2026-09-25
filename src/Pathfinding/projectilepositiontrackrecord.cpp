//============================================================================
//  0x6F479AC0 - shares PositionTrack::SetMaxSpeed's own numeric vtable slot
//  (6, +0x18) but is not really an override of it at all once its own bytes
//  are read: three plain `int` arguments (`retn 0Ch`), no CFloat anywhere,
//  building the same 0x2C-byte SPositionRecordBlock request
//  Widget/positionrecordbuild.cpp's `Position::RecordSlot8` already builds
//  and forwarding it into the identical `SubmitRecord` (0x6F478830) - a
//  direct call, `this` passed through unchanged, not a virtual dispatch.
//  Own translation unit: 0x6F479Axx is its own shipped module.
//
//  Field values (not declaration order) drove the store order below,
//  matching the shipped stream instruction for instruction: the global is
//  written into three different fields from the one cached register before
//  the two -1 sentinels reuse the next value that register holds, and the
//  two FourCC tags land dead last, right before the call - after `block`'s
//  own address has already been taken.
//============================================================================
#include "projectilepositiontrack.h"
#include "positionrecordblock.h"
#include "floatmini.h"   // g_unk6FAAE624

//  positionrecordbuild.cpp's own local declaration - the real receiver
//  `this` passes through to unchanged, so no header carries this type.
struct SPositionRecordSink
{
    int __thiscall SubmitRecord(int a, int b, void* block, int d);
};

int ProjectilePositionTrack::SubmitAttachRecord(int a, int b, int d)
{
    SPositionRecordBlock block;

    unsigned int global = g_unk6FAAE624;
    int zero = 0;
    block.m_global = global;
    block.m_zero10 = global;
    block.m_zero14 = global;
    int negOne = -1;
    block.m_sentinel28 = negOne;
    block.m_sentinel24 = negOne;
    block.m_argC   = zero;
    block.m_zero18 = zero;
    block.m_zero1C = zero;
    block.m_zero20 = zero;
    block.m_tag0 = 0x5E70726F;
    block.m_tag1 = 0x60706268;

    return ((SPositionRecordSink*)this)->SubmitRecord(a, b, &block, d);
}
