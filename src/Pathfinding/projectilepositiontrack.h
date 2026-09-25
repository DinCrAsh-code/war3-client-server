//============================================================================
//  ProjectilePositionTrack - the shipped `ProjectilePosition`, RTTI vtable
//  `??_7ProjectilePosition@@6B@` @ 0x6F877F28 (agent_worktrees/classes/
//  0x6F877F28.json, Position_family_calltree_asm.md).  Same disambiguation
//  as PositionTrack itself (positiontrack.h) - `ProjectilePosition` is kept
//  out of the class name here purely for naming-convention consistency with
//  its two siblings, not because of a name collision this session found.
//
//  Only 7 vtable slots (0x00-0x18), one short of PositionTrack's own 8:
//  slots 0/2/4/5 are literally CAgentField's own addresses (inherited,
//  not overridden - TRefCnt::ReleaseSelf, Detach, GetTrackedAgent,
//  GetAgent - see Agent/agentfield.h), slot 3 (AttachFromTable) and slot 1
//  (the scalar deleting destructor) are overridden here, and the slot 6
//  address (0x6F479AC0) turns out - once actually read - not to be a
//  speed-clamping override compatible with PositionTrack::SetMaxSpeed's own
//  one-argument signature at all (see projectilepositiontrackrecord.cpp):
//  it is a 3-argument forwarder into the same SubmitRecord/record-array
//  family Widget/positionrecordbuild.cpp's `Position::RecordSlot8` already
//  reconstructs, sharing this numeric slot only because of where it falls
//  in the shipped class's own declaration order - not a real override of
//  PositionTrack::SetMaxSpeed's virtual.  Slot 7 (GetField8C) has no
//  counterpart at all - `ProjectilePosition` inherits straight from
//  `Position` in the real chain (Pathfinding/positiontrack.h's own header
//  comment), one level below `Position`'s own slot-7 addition, so a
//  7-slot `ProjectilePosition` sitting one slot short of an 8-slot
//  `Position` is exactly what real single inheritance produces - not a
//  gap. (This resolves the "unmodelled common base" this header used to
//  flag before IDA's own RTTI base-class list settled it - see
//  positiontrack.h's own header comment for the confirmed chain.)
//============================================================================
#ifndef PROJECTILEPOSITIONTRACK_H
#define PROJECTILEPOSITIONTRACK_H

#include "positiontrack.h"

class ProjectilePositionTrack : public PositionTrack
{
public:
    //  slot 1 / +0x04 (0x6F03BA70) - byte-identical to PositionTrack's own
    //  scalar deleting destructor (0x6F03A9F0, positiontrackdeleteself.cpp):
    //  ~ProjectilePositionTrack() is just as trivial.  See
    //  projectilepositiontrackdeleteself.cpp.
    virtual PositionTrack* DeleteSelf(int flags);

    //  slot 3 / +0x0C (0x6F47AAA0) - overrides PositionTrack::
    //  AttachFromTable.  By far the largest function in the whole family:
    //  resolves the handle-table entry the same way the base does, then -
    //  once it resolves to a live CBhPoProjectile (Missile/
    //  bhpoprojectile.h) - arms a tick-relay slot on it and flips two
    //  bits on a pair of related owner objects.  See
    //  projectilepositiontrackattach.cpp.
    virtual void AttachFromTable(int index, SIndexedHandleTable* table);

    //  0x6F479AC0 - shares PositionTrack::SetMaxSpeed's own numeric vtable
    //  slot (6 / +0x18) but not its signature or its behaviour: three plain
    //  int arguments, no CFloat in sight, forwarding straight into
    //  SubmitRecord the same way Widget/positionrecordbuild.cpp's
    //  `Position::RecordSlot8` does.  Not spelled `virtual` / `override` -
    //  a 3-int member cannot actually override a 1-pointer-arg base virtual
    //  in real C++, so this is declared as the ordinary member its own
    //  signature calls for.  See projectilepositiontrackrecord.cpp.
    int SubmitAttachRecord(int a, int b, int d);
};

#endif
