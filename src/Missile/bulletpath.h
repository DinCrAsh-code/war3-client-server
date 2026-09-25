//============================================================================
//  CBulletPath - RTTI-named (agent_worktrees/classes/0x6F9169BC.json,
//  vtable @ 0x6F9169BC, hierarchy "CBulletPath: CBulletBase, CWar3Image,
//  CAgentWar3, CAgent, CObserver, TRefCnt;") - a direct CBulletBase
//  descendant, parallel to the CBullet/CMissile/CMissileFire branch
//  (bullet.h/missile.h/missilefire.h), not part of it: its own 50-slot
//  vtable shares CBulletBase's own first 48 slots' *shape* (most inherited
//  unchanged - see below) and adds two more (48/49) past CBulletBase's own.
//
//  This session closed only the two single-purpose, self-contained
//  overrides confirmed from raw_asm alone - GetAgileTypeId (slot 7) and
//  GetClassName (slot 22), same "bare FourCC constant" / "own name string"
//  shape every level of this family already uses (bulletbase_dispatch.cpp,
//  missile_core.cpp). The remaining slots this class's own vtable_members
//  lists as genuinely overridden (not simply inherited from CBulletBase) -
//  slot 1 DeleteSelf (0x6F1EBA90, restamps an embedded object at +0x78 to
//  `??_7TRefCnt@@6B@` before calling the real ~CBulletBase()), slots 8/9
//  (0x6F1EA750/0x6F1EA780, both dispatch through that same +0x78 object's
//  own vtable), slot 40 GetTrackedVector (0x6F2BCEE0) and the two new
//  slots 48/49 (0x6F2C3E90/0x6F2C3DC0) - all read a field at +0x78 this
//  session did not identify (an embedded, by-value, TRefCnt-derived
//  polymorphic sub-object CBulletBase's own layout does not have; neither
//  its type nor what constructs it was chased down) and are left
//  un-declared here rather than guessed at. See docs/targets/CBulletPath_
//  family.md for the address-by-address status this whole family batch
//  found.
//============================================================================
#ifndef BULLETPATH_H
#define BULLETPATH_H

#include "bulletbase.h"

class CBulletPath : public CBulletBase
{
public:
    //  slot 7 / +0x1C (0x6F2BB480) - bare FourCC constant.
    virtual unsigned int GetAgileTypeId() const;
    //  slot 22 / +0x58 (0x6F2BB430) - "CBulletPath".
    virtual const char* GetClassName();
};

#endif
