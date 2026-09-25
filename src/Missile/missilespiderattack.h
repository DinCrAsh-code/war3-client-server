//============================================================================
//  CMissileSpiderAttack - RTTI-named (agent_worktrees/classes/0x6F916F9C
//  .json, vtable @ 0x6F916F9C, hierarchy "CMissileSpiderAttack:
//  CMissilePath, CBulletPath, CBulletBase, CWar3Image, CAgentWar3, CAgent,
//  CObserver, TRefCnt;") - a direct CMissilePath descendant
//  (missilepath.h), the deepest class on this parallel branch.
//
//  Same scope note as bulletpath.h/missilepath.h: only the two
//  single-purpose overrides (GetAgileTypeId slot 7, GetClassName slot 22)
//  are closed this session; this class's own remaining genuinely-
//  overridden slots (1, 3, 7 handled, 8-10, 13-15, 17-19, 22 handled, 40,
//  42, 45-50) are new work this session's own budget did not reach.
//============================================================================
#ifndef MISSILESPIDERATTACK_H
#define MISSILESPIDERATTACK_H

#include "missilepath.h"

class CMissileSpiderAttack : public CMissilePath
{
public:
    //  slot 7 / +0x1C (0x6F1D70D0) - bare FourCC constant ("Mspa").
    virtual unsigned int GetAgileTypeId() const;
    //  slot 22 / +0x58 (0x6F1D7080) - "CMissileSpiderAttack".
    virtual const char* GetClassName();
};

#endif
