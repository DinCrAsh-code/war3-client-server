//============================================================================
//  0x6F478830 - SPositionRecordSink::SubmitRecord. `this` (the
//  SPositionRecordSink receiver) is never read: the shipped body's only
//  real receiver is its own `b` argument, which - once `b != 0` - it
//  treats as a *different* object (SPositionRecordArraySink,
//  positionrecordarray.h - a sink with a growable array of 0x2C-byte
//  SPositionRecordBlock entries at its own +0x108) and calls two of *that*
//  object's members on: an append (0x6F4A5440) with the block already
//  built here, then a second entry point (0x6F4A54A0) that builds its own
//  local record from `a`, the literal 1 and `block`'s own +0x08 field
//  before appending that too. `retn 10h` - four stack arguments, and
//  `this` arrives in ecx unchanged from this function's own caller, which
//  is why the declaration stays a member rather than a __fastcall free
//  function even though the body never touches it. The return value is
//  always `a`, regardless of what either append call answers - the shipped
//  stream never re-reads eax after the second call.
//
//  Own translation unit: PositionTrack::RecordSlot8
//  (Pathfinding/positiontrackrecordbuild.cpp) reaches this with a real
//  `call` and it makes two of its own.
//
//  SPositionRecordSink itself is declared in positionrecordarray.h, so
//  Pathfinding/smartpositiontrackrecordslot8.cpp can call it too.
//============================================================================
#include "positionrecordarray.h"
#include "positionrecordblock.h"

int __thiscall SPositionRecordSink::SubmitRecord(int a, int b, void* block,
                                                  int d)
{
    if (b == 0)
        return a;

    SPositionRecordArraySink* sink = (SPositionRecordArraySink*)b;
    SPositionRecordBlock* rec = (SPositionRecordBlock*)block;

    sink->AppendRecord(rec, d);
    sink->BuildAndInsertRecord(a, 1, rec->m_argC);

    return a;
}
