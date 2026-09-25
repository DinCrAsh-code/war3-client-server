//============================================================================
//  SPositionRecordArraySink - see positionrecordarray.cpp.  Declared here so
//  positionrecordbuild.cpp's own SubmitRecord (0x6F478830) can call its two
//  members without a second, disagreeing declaration.
//============================================================================
#ifndef POSITIONRECORDARRAY_H
#define POSITIONRECORDARRAY_H

#include "cellbuffer.h"
#include "positionrecordblock.h"

//----------------------------------------------------------------------------
//  0x6F478830 - SubmitRecord itself lives in positionrecordbuild.cpp (its
//  own translation unit, since it makes a real call).  Declared here, not
//  there, so Pathfinding/smartpositiontrackrecordslot8.cpp
//  (SmartPositionTrack::RecordSlot8, 0x6F473A00) can call it too without a
//  second, disagreeing declaration - the same reason
//  SPositionRecordArraySink is declared here rather than in
//  positionrecordbuild.cpp.
//----------------------------------------------------------------------------
struct SPositionRecordSink
{
    int __thiscall SubmitRecord(int a, int b, void* block, int d);
};

struct SPositionRecordArray
{
    int GrowInsert(void* insertPtr, unsigned int count);   // 0x6F4A4A40, naked

    //  0x6F4A4390 - broadcast `count` copies of the 0x2C-byte record at
    //  `record` into [start, min(start+count, m_count)) of the storage
    //  array.  Not a shift/memmove despite GrowInsert's own comment
    //  guessing "shift existing elements when an insertion point is given"
    //  before this address was reachable - the source pointer (`record`)
    //  is reloaded fresh every loop iteration while the destination
    //  advances, i.e. the same one record is copied into every slot in
    //  range, not a block moved along by one. `retn 0Ch` - three stack
    //  arguments, `this` in ecx.  Shared: also reachable from the
    //  ProjectilePosition subtree's own growable array of this same
    //  0x2C-byte record type - see Missile/bhpoprojectile_attach.cpp's own
    //  sibling for the generic dword-sized version of the identical shape
    //  (SBhPoTickRelaySlotArray::FillNewSlots).  See positionrecordarray.cpp.
    void FillRange(unsigned int start, const void* record,
                    unsigned int count);

    CMemBlock    m_storage;    // +0x00
    unsigned int m_growBy;     // +0x14
    unsigned int m_capacity;   // +0x18
    unsigned int m_count;      // +0x1C
};

struct SPositionRecordArraySink
{
    void AppendRecord(SPositionRecordBlock* block, int d);      // 0x6F4A5440
    int  BuildAndInsertRecord(int a, int flags, int c);         // 0x6F4A54A0

    char                  m_reserved00[0x108];
    SPositionRecordArray  m_array;   // +0x108
};

#endif
