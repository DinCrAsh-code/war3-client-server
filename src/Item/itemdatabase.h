//============================================================================
//  CItemDatabase - a CWar3SlkDatabase-derived sibling of CUnitDatabase
//  (Unit/unitdatabase.h) and CAbilityDatabase (Item/abilitydatabase.h) in
//  the checksum-provider registry family (docs/notes/checksum-provider-registry.md).
//  Shares slot 0 (ComputeChecksum, 0x6F005D60) and does NOT share slot 2
//  with the base - CItemDatabase overrides SetRecordSet with its own
//  0x6F25F120, confirmed by reading it (it is NOT the shared
//  0x6F005D30 address - it forwards to that base implementation and then
//  wires up 33 named SLK fields of its own, see itemdatabase_setrecordset.cpp).
//
//  Layout: the base CWar3SlkDatabase fields (vtable pointer at +0x00,
//  m_recordSet at +0x04 - the same field CAbilityDatabase's own
//  SetRecordSet/GetChecksum touch, per unitdatabase_checksum.cpp's own
//  header note on the shared base) followed directly by 33 field-descriptor
//  pairs, 8 bytes each, from +0x10 to +0x110 with NO gaps between them -
//  the SLK field table itself, in dump order: itemID, scriptname, file,
//  uses, goldcost, lumbercost, stockMax, usable, perishable, droppable,
//  powerup, sellable, pawnable, drop, HP, stockRegen, stockStart, targType,
//  armor, prio, abilList, Level, morph, pickRandom, version, oldLevel,
//  class, cooldownID, ignoreCD, scale, colorR, colorG, colorB.  Each pair
//  is {the resolved WEDB field index (or its release/warn result -
//  see itemdatabase_setrecordset.cpp's own note on ReleaseWEDBFieldIndexOrWarn's
//  real, ignored-`name` semantics), a value read back out of the generic
//  SLK field-registration engine at 0x6F7127A0
//  (Item/registertypedfield_thunk.cpp)}.
//
//  **Re-based onto real `public CWar3SlkDatabase` inheritance** once that
//  class's own RTTI-confirmed hierarchy landed (war3slkdatabase.h) -
//  `agent_worktrees/classes/0x6F92CC68.json`'s own `hierarchy` names
//  `CWar3SlkDatabase, CSynchronousData` and `class_hierarchy_audit.py`
//  flagged the earlier flat-struct model (written before that base class
//  existed, see docs/notes/checksum-provider-registry.md) as missing both
//  ancestors.  Slot 0 (ComputeChecksum, 0x6F005D60) is inherited unchanged
//  - not redeclared here.  Slot 1 (shared nullsub_53, 0x6F002000) gets
//  this class's own distinct trivial override, same convention
//  war3slkdatabase.h's own DebugDumpTo establishes.  Slot 2 (SetRecordSet)
//  and the new slot 3 (the scalar deleting destructor) are real.
//============================================================================
#ifndef ITEMDATABASE_H
#define ITEMDATABASE_H

#include "war3slkdatabase.h"

struct SItemDatabaseFieldPair
{
    void* m_wedbResult;
    unsigned int m_registeredValue;
};

class CItemDatabase : public CWar3SlkDatabase
{
public:
    SItemDatabaseFieldPair m_itemID;       // +0x10
    SItemDatabaseFieldPair m_scriptname;   // +0x18
    SItemDatabaseFieldPair m_file;         // +0x20
    SItemDatabaseFieldPair m_uses;         // +0x28
    SItemDatabaseFieldPair m_goldcost;     // +0x30
    SItemDatabaseFieldPair m_lumbercost;   // +0x38
    SItemDatabaseFieldPair m_stockMax;     // +0x40
    SItemDatabaseFieldPair m_usable;       // +0x48
    SItemDatabaseFieldPair m_perishable;   // +0x50
    SItemDatabaseFieldPair m_droppable;    // +0x58
    SItemDatabaseFieldPair m_powerup;      // +0x60
    SItemDatabaseFieldPair m_sellable;     // +0x68
    SItemDatabaseFieldPair m_pawnable;     // +0x70
    SItemDatabaseFieldPair m_drop;         // +0x78
    SItemDatabaseFieldPair m_HP;           // +0x80
    SItemDatabaseFieldPair m_stockRegen;   // +0x88
    SItemDatabaseFieldPair m_stockStart;   // +0x90
    SItemDatabaseFieldPair m_targType;     // +0x98
    SItemDatabaseFieldPair m_armor;        // +0xA0
    SItemDatabaseFieldPair m_prio;         // +0xA8
    SItemDatabaseFieldPair m_abilList;     // +0xB0
    SItemDatabaseFieldPair m_Level;        // +0xB8
    SItemDatabaseFieldPair m_morph;        // +0xC0
    SItemDatabaseFieldPair m_pickRandom;   // +0xC8
    SItemDatabaseFieldPair m_version;      // +0xD0
    SItemDatabaseFieldPair m_oldLevel;     // +0xD8
    SItemDatabaseFieldPair m_class;        // +0xE0
    SItemDatabaseFieldPair m_cooldownID;   // +0xE8
    SItemDatabaseFieldPair m_ignoreCD;     // +0xF0
    SItemDatabaseFieldPair m_scale;        // +0xF8
    SItemDatabaseFieldPair m_colorR;       // +0x100
    SItemDatabaseFieldPair m_colorG;       // +0x108
    SItemDatabaseFieldPair m_colorB;       // +0x110

    //  0x6F257370 - the real constructor: zero every field, then stamp
    //  the vtable.  Not a real C++ constructor (kept a plain method,
    //  named `Construct` per this family's own convention -
    //  Item/abilitycustomdata.h) even now that this class has real
    //  virtuals of its own - AcquireItemDatabase's own inline/`call`
    //  zero-then-stamp shape is not a placement-new, so `funcmap.py`/
    //  ctor_vtable_audit.py see a normal function rather than a
    //  constructor stamping a raw vtable literal.  itemdatabase_ctor.cpp.
    void __thiscall Construct();

    //  Shared nullsub_53 (0x6F002000) - vtable slot 1, NOT overridden by
    //  the shipped class.  This class's own distinct, real, empty
    //  definition - see this header's own note above.  No address of its
    //  own to claim in funcmap.py.
    virtual void DebugDumpTo(void* /*buffer*/) {}

    //  0x6F25F120 - vtable slot 2 override.  itemdatabase_setrecordset.cpp.
    virtual void SetRecordSet(void* recordSet);

    //  0x6F2576D0 - vtable slot 3, a genuinely NEW virtual this class adds
    //  beyond CWar3SlkDatabase's own 3-slot table: the scalar deleting
    //  destructor.  itemdatabase_destructor.cpp.
    virtual CItemDatabase* DeleteSelf(unsigned int flags);

    //  0x6F2574C0 - the real ~CItemDatabase(), a naked redirect; see
    //  itemdatabase_destructor.cpp's own header note for why.
    void DestructInternal();
};

#endif
