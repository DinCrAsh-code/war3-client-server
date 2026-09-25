//============================================================================
//  CDestructableDatabase - the last of the checksum-provider registry's
//  CWar3SlkDatabase-derived siblings this batch reconstructed (alongside
//  CUnitDatabase, CItemDatabase and CUpgradeDatabase - see
//  docs/notes/checksum-provider-registry.md).  Distinct from CDestructable
//  itself (Unit/destructable.h), the per-instance game object this class's
//  own SLK sheet feeds - own header/file, own address range entirely
//  (0x6F260xxx vs the destructable.h family's 0x6F25xxx-0x6F26x range for
//  the live object).  Only 3 vtable slots total, shares slot 0
//  (ComputeChecksum, 0x6F005D60) with the rest of the family and
//  overrides slot 2 (SetRecordSet) with its own 0x6F260E30.
//
//  Layout: the base CWar3SlkDatabase fields (vtable pointer at +0x00,
//  m_recordSet at +0x04) followed by 37 simple field-descriptor pairs
//  (8 bytes each, +0x10..+0x134, no gaps): DestructableID, dir, file,
//  targType, armor, HP, lightweight, fatLOS, numVar, maxPitch, maxRoll,
//  radius, fogRadius, fogVis, pathTex, pathTexDeath, deathSnd, shadow,
//  texID, texFile, occH, flyH, walkable, cliffHeight, fixedRot, goldRep,
//  lumberRep, buildTime, repairTime, colorR, colorG, colorB, version,
//  selectable, selcircsize, portraitmodel, Name.
//
//  **This class's own SetRecordSet does more than the field table** - see
//  destructabledatabase_setrecordset.cpp's own header note on the ~150
//  instruction cross-row "Name" uniqueness validation/de-duplication pass
//  that follows the 37th field, left out of this batch's own scope.
//
//  **Re-based onto real `public CWar3SlkDatabase` inheritance** once that
//  class's own RTTI-confirmed hierarchy landed (war3slkdatabase.h) -
//  `agent_worktrees/classes/0x6F92C6FC.json`'s own `hierarchy` names
//  `CWar3SlkDatabase, CSynchronousData` and `class_hierarchy_audit.py`
//  flagged the earlier flat-struct model (written before that base class
//  existed in this repo, see docs/notes/checksum-provider-registry.md) as
//  missing both ancestors.  Slot 0 (ComputeChecksum, 0x6F005D60) is
//  inherited unchanged - not redeclared here at all, real inheritance
//  gives the derived vtable the identical base slot for free.  Slot 1
//  (the shared nullsub_53, 0x6F002000) gets this class's own distinct
//  trivial override, per war3slkdatabase.h's own convention for that slot
//  (a real, separately-compiled empty definition rather than a second
//  reconstruction filed under an address that belongs to no one class).
//  Slot 2 (SetRecordSet, 0x6F260E30) is the one real override.
//============================================================================
#ifndef DESTRUCTABLEDATABASE_H
#define DESTRUCTABLEDATABASE_H

#include "war3slkdatabase.h"

struct SDestructableFieldPair
{
    void* m_wedbResult;
    unsigned int m_registeredValue;
};

class CDestructableDatabase : public CWar3SlkDatabase
{
public:
    SDestructableFieldPair m_DestructableID;   // +0x10
    SDestructableFieldPair m_dir;              // +0x18
    SDestructableFieldPair m_file;             // +0x20
    SDestructableFieldPair m_targType;         // +0x28
    SDestructableFieldPair m_armor;            // +0x30
    SDestructableFieldPair m_HP;               // +0x38
    SDestructableFieldPair m_lightweight;      // +0x40
    SDestructableFieldPair m_fatLOS;           // +0x48
    SDestructableFieldPair m_numVar;           // +0x50
    SDestructableFieldPair m_maxPitch;         // +0x58
    SDestructableFieldPair m_maxRoll;          // +0x60
    SDestructableFieldPair m_radius;           // +0x68
    SDestructableFieldPair m_fogRadius;        // +0x70
    SDestructableFieldPair m_fogVis;           // +0x78
    SDestructableFieldPair m_pathTex;          // +0x80
    SDestructableFieldPair m_pathTexDeath;     // +0x88
    SDestructableFieldPair m_deathSnd;         // +0x90
    SDestructableFieldPair m_shadow;           // +0x98
    SDestructableFieldPair m_texID;            // +0xA0
    SDestructableFieldPair m_texFile;          // +0xA8
    SDestructableFieldPair m_occH;             // +0xB0
    SDestructableFieldPair m_flyH;             // +0xB8
    SDestructableFieldPair m_walkable;         // +0xC0
    SDestructableFieldPair m_cliffHeight;      // +0xC8
    SDestructableFieldPair m_fixedRot;         // +0xD0
    SDestructableFieldPair m_goldRep;          // +0xD8
    SDestructableFieldPair m_lumberRep;        // +0xE0
    SDestructableFieldPair m_buildTime;        // +0xE8
    SDestructableFieldPair m_repairTime;       // +0xF0
    SDestructableFieldPair m_colorR;           // +0xF8
    SDestructableFieldPair m_colorG;           // +0x100
    SDestructableFieldPair m_colorB;           // +0x108
    SDestructableFieldPair m_version;          // +0x110
    SDestructableFieldPair m_selectable;       // +0x118
    SDestructableFieldPair m_selcircsize;      // +0x120
    SDestructableFieldPair m_portraitmodel;    // +0x128
    SDestructableFieldPair m_Name;             // +0x130
    //  Field table ends at +0x138.

    //  0x6F267E90's own inline constructor - zero the three
    //  CWar3SlkDatabase checksum-cache fields, then stamp the vtable.
    //  Same shape (and same "the 37 field-pair bytes are NOT zeroed here"
    //  note) as CUpgradeDatabase::Construct - Game/upgradedatabase.h's
    //  own header note.  Not a real C++ constructor - the same
    //  `AcquireXDatabase` inline zero-then-stamp shape as every sibling in
    //  this family, so it stays a plain named method rather than a
    //  mangled ctor.  destructabledatabase_ctor.cpp.
    void __thiscall Construct();

    //  0x6F252A40 - the real (SEH-framed) ~CDestructableDatabase(), found
    //  but not claimed by checksum-batch-A; reconstructed round2-F.  Now
    //  that this class really derives from CWar3SlkDatabase, its own tail
    //  explicit-calls the base's own real, independently-scored
    //  ~CWar3SlkDatabase() (0x6F005D40, war3slkdatabase.cpp) instead of
    //  hand-stamping a flat base view.  See destructabledatabase_destructor.cpp.
    void __thiscall DestructInternal();

    //  Shared nullsub_53 (0x6F002000) - vtable slot 1, NOT overridden by
    //  the shipped class (same address as the base's own slot 1).  This
    //  class's own distinct, real, empty definition - see this header's
    //  own note above and war3slkdatabase.h's identical convention.  No
    //  address of its own to claim in funcmap.py.
    virtual void DebugDumpTo(void* /*buffer*/) {}

    //  0x6F260E30 - vtable slot 2 override.  destructabledatabase_setrecordset.cpp.
    virtual void SetRecordSet(void* recordSet);
};

#endif
