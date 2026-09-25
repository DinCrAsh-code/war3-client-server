//============================================================================
//  0x6F473A00 - SmartPositionTrack's vtable slot 8, SmartPosition's own
//  addition with no PositionTrack/ProjectilePosition counterpart.
//
//  The same "build a 0x2C-byte request block, forward it" shape
//  Widget/position.h's own Position::RecordSlot8 has (0x6F473980,
//  Widget/positionrecordbuild.cpp) - two FourCC-tag-shaped words, one
//  global (g_unk6FAAE624), five zeroed words, two -1 sentinels, then one
//  call into SPositionRecordSink::SubmitRecord (0x6F478830) with `this`
//  passed through untouched - but simpler in two ways:
//
//   * no `b == 0` early-out: this build always constructs the block and
//     always calls SubmitRecord, which itself already handles b == 0 (see
//     positionrecordbuild.cpp's own SubmitRecord) - so the early-out just
//     isn't here, not missing;
//   * the third stack argument (`tag`) is never read at all - `m_argC` is
//     hard-coded to 0 rather than taking a caller-supplied value the way
//     Position::RecordSlot8's own `c` does.
//
//  Evidence this really is SmartPosition's own slot and not a guess: `this`
//  is never touched anywhere in the body (it travels in ecx straight
//  through to SubmitRecord, the identical "member only because ecx carries
//  it" shape RecordSlot8 itself has), and CUnit::Method_0x20
//  (Unit/unit_dumpstate.cpp) dispatches to `m_movement`'s own vtable+0x20
//  (slot 8) with exactly this signature's shape - `(index, context, tag,
//  flag)`, `tag` a pointer the callee never reads - which is the concrete
//  proof that m_movement really is a SmartPositionTrack (see
//  Unit/unit_ctor.cpp's own note on the trivial local `SmartPosition`
//  vtable-symbol stub there, and docs/targets/Position-family.md's
//  "SmartPositionTrack" section).
//
//  Own translation unit: slot 8 reaches SubmitRecord with a real call, the
//  same reason positionrecordbuild.cpp keeps its own twin apart from its
//  callees.
//============================================================================
#include "positiontrack.h"
#include "positionrecordarray.h"
#include "positionrecordblock.h"
#include "floatmini.h"   // g_unk6FAAE624

int SmartPositionTrack::RecordSlot8(int index, int context, const void* tag,
                                     int flag)
{
    (void)tag;   // read by nobody in this build either - see comment above

    SPositionRecordBlock block;

    block.m_argC   = 0;
    block.m_zero10 = 0;
    block.m_zero14 = 0;
    block.m_zero18 = 0;
    block.m_zero1C = 0;
    block.m_zero20 = 0;

    //  +0x28 before +0x24: the shipped code stores the sentinel into var_4
    //  and then into var_8, the same order positionrecordbuild.cpp's own
    //  builder uses.
    block.m_sentinel28 = -1;
    block.m_sentinel24 = -1;

    unsigned int global = g_unk6FAAE624;
    block.m_global = global;
    block.m_tag0   = 0x5E70726F;
    block.m_tag1   = 0x60706375;

    return ((SPositionRecordSink*)this)->SubmitRecord(index, context, &block,
                                                        flag);
}
