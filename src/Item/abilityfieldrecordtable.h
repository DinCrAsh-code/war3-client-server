//============================================================================
//  g_abilityFieldRecordTable (dword_6FAB58FC) - the field-record cache
//  LookupAbilityFieldRecord (ability_typefieldrecord.cpp, 0x6F32C8E0) looks
//  a resolved field's generic-record wrapper up in.  A sibling instantiation
//  of the exact same TSHashTable<T,AGILE_TYPE_ID> cluster
//  Pathfinding/footprinttype.h already documents for CFootprintTypeData -
//  same module neighbourhood (0x6F30xxxx/0x6F31xxxx/0x6F32xxxx), same
//  vftable-carrying node shape (HashLinkOffset()==8, confirmed directly off
//  this instantiation's own Initialize: `cmp dword ptr [esi],8` against
//  family A's plain `cmp dword ptr [esi],4` for the no-vftable node), same
//  0xDDDDDDDD poison a freshly grown TSExplicitList<T> bucket is born with.
//  Forty bytes past g_footprintTypeTable's own dword_6FAB58D4, the size of
//  one TSHashTable object, so the two sit back to back in the data segment.
//
//  Node layout: TSHashObject's own m_hashval/m_key sit at +0x04/+0x18
//  rather than the usual +0x00/+0x14 because of the vtable pointer at
//  +0x00 (the same shift agiletype.h's own AGILE_TYPE_DATA documents for
//  the identical reason).  The record body from +0x1DC on (two
//  required/optional jagged-array pairs) is only read by
//  CAbility::DepCheckAlias (ability_typedep.cpp, "HasDepAbil" tag), which
//  was this call tree's only reader of the record past its header until
//  CAbilityButton's own icon/command-order accessors (Method_0x1B0/
//  Method_0x1C0/Method_0x298, abilitybutton_bigslots.cpp) were
//  reconstructed for real - they read thirteen more dwords out of what
//  used to be the single opaque m_reserved1C blob below +0x1DC, all
//  plain scalars/counted-array pairs in the same "count dword immediately
//  followed by its array pointer" shape the four SGroup pairs above
//  already have, just narrower (one flat array of pointers/ints rather
//  than an array of 12-byte SGroup structs) - CLAUDE.md's own rule is
//  that only the offsets an actual call tree touches get named, so the
//  gaps between them stay reserved.
//============================================================================
#ifndef ABILITYFIELDRECORDTABLE_H
#define ABILITYFIELDRECORDTABLE_H

#include "agiletype.h"

class SAbilityFieldRecordNode
    : public TSHashObject<SAbilityFieldRecordNode, AGILE_TYPE_ID>
{
public:
    //  Slot 0: 0x6F32C8E0's own insert path calls straight through the
    //  freshly-created node's own vtable (`mov edx,[esi]; mov eax,[edx];
    //  call eax`) with the raw typeId, the same "AllocNode leaves
    //  construction to a virtual on the node itself" shape footprinttype.h
    //  documents for CFootprintTypeData::FinishConstruct.  Its return
    //  value is never read by the caller (LookupAbilityFieldRecord
    //  overwrites eax with the node pointer right after the call), so
    //  this is an initialiser, not an accessor.
    virtual void FinishConstruct(unsigned int typeId) = 0;
    virtual ~SAbilityFieldRecordNode();

    //  A jagged-array pair: m_count groups, each a 12-byte SGroup naming
    //  how many rawcodes/flags it holds and where they are - the same
    //  shape footprinttype.h's own SUpgradeIdGroup already has.
    struct SGroup
    {
        unsigned int m_field0;
        unsigned int m_count;
        unsigned int* m_data;
    };
    char             m_reserved1C[0x38 - 0x1C];
    //  +0x38 - read by Method_0x1C0 (abilitybutton_bigslots.cpp)'s own
    //  "instance override not set" fallback path, forwarded on unread as
    //  one of nine plain ints a GameUI command-button call takes - no
    //  further meaning recovered.
    unsigned int     m_field38;         // 0x38
    char             m_reserved3C[0x50 - 0x3C];
    //  +0x50/+0x54 - a plain dword pair Method_0x1B0 reads once (both
    //  saved to locals before any of its own leaf-slot calls run) and
    //  forwards unread, same "opaque pass-through pair" shape as
    //  +0x58/+0x5C below.
    unsigned int     m_field50;         // 0x50
    unsigned int     m_field54;         // 0x54
    //  +0x58/+0x5C - Method_0x1B0's own second such pair (its "icon
    //  changed" half, alongside +0x38 above).
    unsigned int     m_field58;         // 0x58
    unsigned int     m_field5C;         // 0x5C
    char             m_reserved60[0x80 - 0x60];
    //  +0x80/+0x84 - Method_0x298's own "icon not set / not dirty" else
    //  branch's third counted-array pair (a plain int, not a pointer -
    //  fallback -1 when the count is zero), the same shape as
    //  +0x8C/+0x90 below - that pair is this same branch's own
    //  true-condition sibling, read only when the icon *is* set and
    //  dirty.
    unsigned int     m_field80;         // 0x80
    unsigned int*    m_field84;         // 0x84
    char             m_reserved88[0x8C - 0x88];
    //  +0x8C/+0x90 - a third counted-array pair (plain ints, not
    //  pointers - Method_0x298's own "no per-type override" fallback
    //  reads array[clampedIndex] straight into the value it forwards, no
    //  further dereference), the same "count dword then a pointer to
    //  count items" shape the four SGroup pairs below already have, just
    //  one flat array rather than SGroup structs.
    unsigned int     m_field8C;         // 0x8C
    unsigned int*    m_field90;         // 0x90
    char             m_reserved94[0xB0 - 0x94];
    //  +0xB0/+0xB4 - Method_0x1B0's own "has this ability got its own
    //  icon path" flag and, when set, a pointer to that path string
    //  (dereferenced once - `*m_fieldB4`, not indexed - so this is a
    //  pointer to a single `const char*`, not an array of them).
    unsigned int     m_fieldB0;         // 0xB0
    const char* const* m_fieldB4;       // 0xB4
    char             m_reservedB8[0x134 - 0xB8];
    //  +0x134/+0x138 - Method_0x298's own "icon not set / not dirty" else
    //  branch's own title array (fallback "Tool tip missing!" when the
    //  count is zero) - this branch's sibling of +0x140/+0x144 below.
    unsigned int     m_field134;        // 0x134
    const char**     m_field138;        // 0x138
    char             m_reserved13C[0x140 - 0x13C];
    //  +0x140/+0x144 - a counted array of requirement-name strings
    //  Method_0x298's own tooltip builder indexes (clamped the same way
    //  Pathfinding/footprinttype.h's own GetFootprintTypeName clamps its
    //  own last-name index), falling back to the literal "Tool tip
    //  missing!" when the count is zero.
    unsigned int     m_field140;        // 0x140
    const char**     m_field144;        // 0x144
    char             m_reserved148[0x158 - 0x148];
    //  +0x158/+0x15C - the else branch's own second value (fallback 0),
    //  this branch's sibling of +0x164/+0x168 below.
    unsigned int     m_field158;        // 0x158
    unsigned int*    m_field15C;        // 0x15C
    char             m_reserved160[0x164 - 0x160];
    //  +0x164/+0x168 - the same shape one property over: a counted array
    //  of plain ints (icon ids, forwarded unread) rather than strings,
    //  falling back to 0 rather than a string literal when the count is
    //  zero.
    unsigned int     m_field164;        // 0x164
    unsigned int*    m_field168;        // 0x168
    char             m_reserved16C[0x1DC - 0x16C];
    unsigned int     m_requiredCount;   // 0x1DC - group's own required-type-id list
    SGroup*          m_required;        // 0x1E0
    char             m_reserved1E4[0x1F4 - 0x1E4];
    unsigned int     m_optionalCount;   // 0x1F4 - group's own optional-flag list
    SGroup*          m_optional;        // 0x1F8
    char             m_reserved1FC[0x20C - 0x1FC];
    unsigned int     m_required2Count;  // 0x20C - second required-type-id list
    SGroup*          m_required2;       // 0x210
    char             m_reserved214[0x224 - 0x214];
    unsigned int     m_optional2Count;  // 0x224 - second optional-flag list
    SGroup*          m_optional2;       // 0x228

    //  0x6F330580 - CAbilityButton::Method_0x298's own tooltip builder
    //  (abilitybutton_bigslots.cpp): appends one "\n - <name>" line per
    //  entry of the required/required2 groups at `index` into `buf`
    //  (bounded by `bufSize`), naming each entry through
    //  GetFootprintTypeName(requiredTypeId, optionalFlag - 1) - the
    //  optional/optional2 groups supply that name-index, falling back to
    //  the sentinel 1 when a group/entry is missing.  See
    //  ability_typefieldrecord_text.cpp.  `retn 0Ch` - three stack args,
    //  `this` in ecx.
    void AppendRequiresText(char* buf, unsigned int bufSize, unsigned int index);
};

//----------------------------------------------------------------------------
//  The concrete table.  Both virtuals are declared only so the vtable
//  slots exist - nothing here constructs one or calls one, the same shape
//  AGILE_TYPE_TABLE and CUnitTypeTable already have (agiletype.h,
//  Unit/unittypetable.h) - so no vtable of it is emitted from this TU.
//----------------------------------------------------------------------------
class SAbilityFieldRecordTable
    : public TSHashTable<SAbilityFieldRecordNode, AGILE_TYPE_ID>
{
public:
    virtual ~SAbilityFieldRecordTable();
    virtual SAbilityFieldRecordNode* AllocNode(
        TSExplicitList<SAbilityFieldRecordNode>* bucket, void* arg1, void* arg2);
};

typedef TSExplicitList<SAbilityFieldRecordNode> SAbilityFieldRecordBucket;

//  dword_6FAB58FC - the object, not a pointer to it.
extern SAbilityFieldRecordTable g_abilityFieldRecordTable;

#endif
