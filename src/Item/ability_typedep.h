//============================================================================
//  A fourth TSHashTable<T, AGILE_TYPE_ID> instantiation (Storm/storm.h,
//  Containers/tshash.inl) - the player's per-typeId dependency-alias
//  cache CAbility slot 154's "DepCheckAlias" walk reads
//  (ability_typedep.cpp).  Unlike SAbilityFieldRecordNode
//  (Item/ability_typefieldrecord.h), this node carries no vftable, so
//  TSHashObject's own fields sit at the un-shifted +0x00/+0x14 - the same
//  layout SUnitTypeRecord already documents (Unit/unittypetable.h).
//
//  Fields named only where CountDependencyRequirement/
//  GetDependencyAliasThreshold (ability_typedep.cpp) touch them; nothing
//  else in this closure reads this record.
//============================================================================
#ifndef ABILITY_TYPEDEP_H
#define ABILITY_TYPEDEP_H

#include "agiletype.h"

class CDependencyAliasRecord
    : public TSHashObject<CDependencyAliasRecord, AGILE_TYPE_ID>
{
public:
    unsigned int m_field18;   // 0x18 - GetDependencyAliasThreshold's own answer
    unsigned int m_field1C;   // 0x1C - kind bit 0
    unsigned int m_field20;   // 0x20 - kind bit 1, added in
    unsigned int m_field24;   // 0x24 - kind bit 2, added in
    unsigned int m_field28;   // 0x28 - kind bit 4, alternative to +0x1C
    //  0x2C - a plain reference count: CUnit's own FindOrCreateDependency
    //  wrapper (cunit_agent4 batch, 0x6F40B250) bumps it by one on every
    //  hit through CDependencyAliasTableOwner::FindOrCreateRecord, and
    //  nothing in this closure ever decrements it - consistent with a
    //  cache entry counting how many live holders point at it rather than
    //  a scope-owned smart-pointer slot.
    unsigned int m_refCount2C;  // 0x2C
    unsigned int m_flags;     // 0x30 - category bits (2 unit / 4 item / 8 destructable)
};

class CDependencyAliasTableClass : public TSHashTable<CDependencyAliasRecord, AGILE_TYPE_ID>
{
public:
    virtual ~CDependencyAliasTableClass();
    virtual CDependencyAliasRecord* AllocNode(
        TSExplicitList<CDependencyAliasRecord>* bucket, void* arg1, void* arg2);
};

//----------------------------------------------------------------------------
//  The table is not a standalone global: every call site loads it as
//  `ecx+4` off some other, unrecovered owner object (FindOrCreate*'s own
//  `this`), so the owner is modelled here with the one member this
//  closure actually reaches.
//----------------------------------------------------------------------------
class CDependencyAliasTableOwner
{
public:
    //  0x6F4036B0 - find or create typeId's dependency-alias record.
    CDependencyAliasRecord* FindOrCreateRecord(unsigned int typeId, int allowCreate);
    //  0x6F4038D0 - see ability_typedep.cpp.
    int CountRequirement(unsigned int typeId, int kind);
    //  0x6F4039C0 - typeId's own +0x18 threshold, or "no cap" with none.
    int GetThreshold(unsigned int typeId);
    //  0x6F403600 - below this closure's depth; declared, not
    //  reconstructed (see ability_typedep.cpp for why `retn 8` is a
    //  reasoned guess, not a confirmed one).
    CDependencyAliasRecord* LookupFlag18Record(unsigned int typeId, int allowCreate);

    char                       m_reserved00[4];
    CDependencyAliasTableClass m_table;   // 0x04
};

typedef CDependencyAliasTableOwner CDependencyAliasTable;

#endif
