//============================================================================
//  CMissilePath - RTTI-named (agent_worktrees/classes/0x6F916A8C.json,
//  vtable @ 0x6F916A8C, hierarchy "CMissilePath: CBulletPath, CBulletBase,
//  CWar3Image, CAgentWar3, CAgent, CObserver, TRefCnt;") - a direct
//  CBulletPath descendant (bulletpath.h), on the same parallel branch, not
//  the CBullet/CMissile/CMissileFire one.
//
//  Same scope note as bulletpath.h: only the two single-purpose overrides
//  (GetAgileTypeId slot 7, GetClassName slot 22) are closed this session.
//  The rest of this class's own genuinely-overridden slots (1, 8, 9, 10,
//  13, 17, 18, 19, 22 handled, 40 shared with CBulletPath's own unresolved
//  slot, 42, 45, 46, 47, 48, 49) mostly route through the same unidentified
//  +0x78 embedded object CBulletPath's own header documents, or are new
//  work this session's own budget did not reach - left un-declared.
//============================================================================
#ifndef MISSILEPATH_H
#define MISSILEPATH_H

#include "bulletpath.h"

class CMissilePath : public CBulletPath
{
public:
    //  slot 7 / +0x1C (0x6F2BB7C0) - bare FourCC constant.
    virtual unsigned int GetAgileTypeId() const;
    //  slot 22 / +0x58 (0x6F2BB770) - "CMissilePath".
    virtual const char* GetClassName();
};

#endif
