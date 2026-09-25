//============================================================================
//  0x6F252A40 - CDestructableDatabase::DestructInternal, the real
//  ~CDestructableDatabase().  Found but not claimed by checksum-batch-A
//  (see its own "still_open" list); reconstructed here
//  (checksum-round2-F).
//
//  Same `push -1 / push offset SEH_.../ mov eax, fs:0`
//  __CxxFrameHandler3-personality prologue as CUpgradeDatabase's own
//  sibling (Game/upgradedatabase_destructor.cpp) - toolchain-unreproducible
//  at this repo's fixed `/GS- /EHs-c-` (docs/msvc-vc8-idioms.md), filed
//  DIFFERS for that reason.
//
//  Body: stamp the derived vtable, then - only if `m_recordSet` is
//  non-null - release each of the 37 simple field pairs' own
//  `m_wedbResult` pointers (ReleaseForward_6F4C8880, Agent/refcnt.h), in
//  declaration order, then tail into the real base ~CWar3SlkDatabase()
//  (0x6F005D40, Item/war3slkdatabase.cpp) - this class now really derives
//  from CWar3SlkDatabase (2026-09-13), so that base dtor's own
//  compiler-emitted vtable restore is exactly the shipped tail.  No
//  indexed groups on this class (destructabledatabase.h has none), so
//  this one is a plain flat walk - simpler than CUpgradeDatabase's own.
//============================================================================
#include "destructabledatabase.h"
#include "refcnt.h"

#define DESTRUCTABLE_DATABASE_VTABLE ((void*)0x6F92C6FC)

void __thiscall CDestructableDatabase::DestructInternal()
{
    *(void**)this = DESTRUCTABLE_DATABASE_VTABLE;   // +0x00, the implicit CWar3SlkDatabase vfptr

    if (m_recordSet != 0)
    {
        ReleaseForward_6F4C8880((TRefCnt*)m_DestructableID.m_wedbResult);
        ReleaseForward_6F4C8880((TRefCnt*)m_dir.m_wedbResult);
        ReleaseForward_6F4C8880((TRefCnt*)m_file.m_wedbResult);
        ReleaseForward_6F4C8880((TRefCnt*)m_targType.m_wedbResult);
        ReleaseForward_6F4C8880((TRefCnt*)m_armor.m_wedbResult);
        ReleaseForward_6F4C8880((TRefCnt*)m_HP.m_wedbResult);
        ReleaseForward_6F4C8880((TRefCnt*)m_lightweight.m_wedbResult);
        ReleaseForward_6F4C8880((TRefCnt*)m_fatLOS.m_wedbResult);
        ReleaseForward_6F4C8880((TRefCnt*)m_numVar.m_wedbResult);
        ReleaseForward_6F4C8880((TRefCnt*)m_maxPitch.m_wedbResult);
        ReleaseForward_6F4C8880((TRefCnt*)m_maxRoll.m_wedbResult);
        ReleaseForward_6F4C8880((TRefCnt*)m_radius.m_wedbResult);
        ReleaseForward_6F4C8880((TRefCnt*)m_fogRadius.m_wedbResult);
        ReleaseForward_6F4C8880((TRefCnt*)m_fogVis.m_wedbResult);
        ReleaseForward_6F4C8880((TRefCnt*)m_pathTex.m_wedbResult);
        ReleaseForward_6F4C8880((TRefCnt*)m_pathTexDeath.m_wedbResult);
        ReleaseForward_6F4C8880((TRefCnt*)m_deathSnd.m_wedbResult);
        ReleaseForward_6F4C8880((TRefCnt*)m_shadow.m_wedbResult);
        ReleaseForward_6F4C8880((TRefCnt*)m_texID.m_wedbResult);
        ReleaseForward_6F4C8880((TRefCnt*)m_texFile.m_wedbResult);
        ReleaseForward_6F4C8880((TRefCnt*)m_occH.m_wedbResult);
        ReleaseForward_6F4C8880((TRefCnt*)m_flyH.m_wedbResult);
        ReleaseForward_6F4C8880((TRefCnt*)m_walkable.m_wedbResult);
        ReleaseForward_6F4C8880((TRefCnt*)m_cliffHeight.m_wedbResult);
        ReleaseForward_6F4C8880((TRefCnt*)m_fixedRot.m_wedbResult);
        ReleaseForward_6F4C8880((TRefCnt*)m_goldRep.m_wedbResult);
        ReleaseForward_6F4C8880((TRefCnt*)m_lumberRep.m_wedbResult);
        ReleaseForward_6F4C8880((TRefCnt*)m_buildTime.m_wedbResult);
        ReleaseForward_6F4C8880((TRefCnt*)m_repairTime.m_wedbResult);
        ReleaseForward_6F4C8880((TRefCnt*)m_colorR.m_wedbResult);
        ReleaseForward_6F4C8880((TRefCnt*)m_colorG.m_wedbResult);
        ReleaseForward_6F4C8880((TRefCnt*)m_colorB.m_wedbResult);
        ReleaseForward_6F4C8880((TRefCnt*)m_version.m_wedbResult);
        ReleaseForward_6F4C8880((TRefCnt*)m_selectable.m_wedbResult);
        ReleaseForward_6F4C8880((TRefCnt*)m_selcircsize.m_wedbResult);
        ReleaseForward_6F4C8880((TRefCnt*)m_portraitmodel.m_wedbResult);
        ReleaseForward_6F4C8880((TRefCnt*)m_Name.m_wedbResult);
    }

    this->CWar3SlkDatabase::~CWar3SlkDatabase();
}
