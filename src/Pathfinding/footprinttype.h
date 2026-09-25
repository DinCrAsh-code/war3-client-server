//============================================================================
//  The per-type "pathing footprint" registry: a global TSHashTable<T,K>
//  instantiation (0x6F31xxxx/0x6F32xxxx/0x6F30xxxx), keyed by the same
//  AGILE_TYPE_ID FourCC as the agile type registry in agiletype.h and even
//  hashed with the same AGILE_TYPE_ID::Hash (0x6F4C8520 - the dump calls it
//  FourCC__Hash here, one function, two names in two call trees).
//
//  Everything about the container (Ptr/NewNode/Initialize/CheckGrow/Rehash/
//  UnlinkAll/SetCount/ComputeChunk/SetAlloc) is a second instantiation of the
//  exact templates agiletype.h's cluster already established - see
//  src/storm.h, src/tshash.inl, src/tsarray.inl, src/tslist.inl.  Only the
//  node type and the table singleton are new.
//
//  The node carries a vftable (same +4 shift as AGILE_TYPE_DATA - see
//  TSHashTable::HashLinkOffset()), so TSHashObject's fields sit at +4/+8/+0x18
//  instead of +0/+4/+0x14.  What lives between the key (+0x18) and the field
//  this call tree actually reads (+0xCC, a footprint shape pointer) is not
//  recoverable from this call tree and is left reserved.
//
//  0x6F32C880 registers/looks up a type's node and finishes constructing a
//  freshly-created one through a call on the *node's* own vtable slot 0,
//  passing the type id back in - AllocNode() itself (reached through the
//  table's vtable, not this call tree) evidently default-constructs the node
//  without it.  That virtual is declared pure and never defined here, same
//  as AGILE_TYPE_TABLE::AllocNode in agiletype.h.
//============================================================================
#ifndef FOOTPRINTTYPE_H
#define FOOTPRINTTYPE_H

#include "agiletype.h"
#include "CFloat.h"

//----------------------------------------------------------------------------
//  The pathing shape a footprint type points at.  Only the three fields
//  sub_6F3E0B50 reads are named; everything else in the struct is unknown.
//----------------------------------------------------------------------------
class CFootprintShape
{
public:
    //  0x6F3E0B50 - snap a position to this footprint's cell grid for the
    //  given facing.  See footprintdir.cpp.
    void ApplyDirection(CFloat* x, CFloat* y, int facing);

    //  0x6F3E0D30 - walk every cell this footprint covers at that position
    //  and direction, calling `visit(cell, context)` for each.  `retn 10h`.
    //  0x6F3DB4F0 - this footprint's bounding box there.  `retn 0Ch`.
    //
    //  Members, not the free `__fastcall` functions they used to be: the
    //  shipped call sites set ONLY ecx (`mov ecx, ebx`, the shape) and push
    //  every other argument, so a `__fastcall` free function silently moved
    //  one argument into edx - which those callees never read - and pushed
    //  4 bytes less than each cleans.  Every caller's frame was then off by
    //  4 for the rest of its body and its own `retn` went to a shifted
    //  address.  All four footprint slots are rewritten on every movement
    //  step, which is what made this a crash on move.  Declared here beside
    //  ApplyDirection, which had the same three-stack-argument shape right
    //  all along.
    void VisitCells(struct SFootprintPos* pos, int direction, void* visit,
                    void* context);
    void CellBox(void* boxOut, struct SFootprintPos* pos, int direction);

    char            m_reserved00[8];
    //  Two per-axis flag words.  Bit 0 clear means "offset this axis by half
    //  a cell"; bit 1 set means "step one half-cell along it".  Which word
    //  drives which axis depends on the facing.
    unsigned int    m_flags8;    // 0x08
    unsigned int    m_flagsC;    // 0x0C
    char            m_reserved10[0x64 - 0x10];
    //  0x64 - bit 0 picks which pathing-mask set CWidget's slot 82 builds
    //  its registration from: four masks {0xC2, 0x10, 8, 4} when set, three
    //  {0xC2, 0x10, 8} when clear.  See widget_addfootprint.cpp.
    unsigned int    m_kindFlags; // 0x64
};

class CFootprintTypeData : public TSHashObject<CFootprintTypeData, AGILE_TYPE_ID>
{
public:
    //  Slot 0 - declared before the destructor because that is where the
    //  shipped code calls it from (`mov eax,[edx]`, not `[edx+4]`).
    virtual void FinishConstruct(unsigned int id) = 0;
    virtual ~CFootprintTypeData();

    char             m_reserved1C[0x28 - 0x1C];
    //  0x28 / 0x2C - a counted array of the type's own display-name
    //  strings, named by 0x6F32DBE0 (footprinttypename.cpp), the "name"
    //  property twin of GetFootprintShape/GetFootprintTypeScale.  It is the
    //  only member of the family that takes a real second parameter, and it
    //  clamps that index into this array rather than trusting it.
    unsigned int     m_nameCount;    // 0x28
    const char**     m_names;        // 0x2C
    char             m_reserved30[0x34 - 0x30];
    //  0x34 - a plain pointer property, the type's own "modelPath" string,
    //  no null check on the caller's side beyond GetOrCreateFootprintTypeData's
    //  own shared node lookup.  Named by 0x6F32CFD0 (footprintmodelpath.cpp).
    const char*      m_modelPath;    // 0x34
    char             m_reserved38[0x48 - 0x38];
    //  0x48 - a plain pointer property, null when the type has no
    //  "uberSplat" record.  Named by 0x6F32D010 (footprintubersplat.cpp),
    //  the `mov eax,[eax+48h]` twin of GetFootprintScaleFactor/
    //  GetFootprintZOffset with a null (not 0.0f) fallback instead - the
    //  member returns a pointer, not a float.
    void*            m_uberSplat;     // 0x48
    //  0x4C - a plain pointer property, null when the type has no "shadow"
    //  record.  Named by 0x6F32D030 (footprintshadow.cpp), the same
    //  `mov eax,[eax+4Ch]` + null-check twin of m_uberSplat one dword over.
    void*            m_shadow;        // 0x4C
    //  0x50 - a plain pointer property, null when the type has no
    //  "structureShadow" record.  Named by 0x6F32D050 (footprintshadow.cpp),
    //  the same `mov eax,[eax+50h]` + null-check twin of m_shadow/
    //  m_uberSplat one dword over.
    void*            m_structureShadow; // 0x50
    //  0x54 - a plain hardware float, the type's own "scaleFactor"
    //  property.  Named by 0x6F32D070 (widget_footprintscale.cpp), the
    //  `fld [eax+54h]` twin of GetFootprintShape that CWidget's selection-
    //  circle slot scales by; it used to sit inside one 0xB0-byte reserved
    //  block with everything else between the key and m_footprint.
    float            m_scaleFactor;  // 0x54
    //  0x58 - a plain hardware float, the type's own "zOffset" property.
    //  Named by 0x6F32D090 (footprintzoffset.cpp), the `fld [eax+58h]`
    //  twin of GetFootprintScaleFactor - same shape, 0.0f fallback instead
    //  of 1.0f.
    float            m_zOffset;      // 0x58
    char             m_reserved5C[0x80 - 0x5C];
    //  0x80/0x84 - a pair of plain hardware floats, "shadowOffset" x/y.
    //  Named by 0x6F32D250 (footprintshadow.cpp): `ecx` on entry is the
    //  hidden return buffer, `edx` the type id (the same
    //  GetPlayerRelationConstant/relationconstant.cpp "return a class by
    //  value" convention), 0.0f/0.0f fallback when the type has no record.
    float            m_shadowOffsetX; // 0x80
    float            m_shadowOffsetY; // 0x84
    //  0x88/0x8C - "shadowSize" x/y, the identical pair-return shape one
    //  step over (0x6F32D2A0).
    float            m_shadowSizeX;   // 0x88
    float            m_shadowSizeY;   // 0x8C
    //  0x90 - a plain flag property, "ShadowOnWater", read back raw with no
    //  null check on the caller's side (0x6F32CBE0, footprintshadow.cpp) -
    //  the same unguarded-read shape m_isStructure/m_selCircleOnWater use,
    //  one dword before m_selCircleOnWater.
    unsigned int     m_shadowOnWater; // 0x90
    //  0x94 - a plain bool/flag property, read straight off the node with
    //  no branch of its own (`mov eax,[eax+94h]; retn` in
    //  0x6F32CC20/GetSelCircleOnWaterFlag - CUnit's own slot 104 asks for
    //  it by that literal debug name, "SelCircleOnWater").
    unsigned int     m_selCircleOnWater; // 0x94
    //  0x98 / 0x9C - "maxPitch"/"maxRoll", the same shape as m_zOffset
    //  (0.0f fallback).  See footprintelevation.cpp.  Read by sub_6F282160
    //  (CUnit's slot 110), which zeroes an output 3x3-basis block and then
    //  builds it from these two angles plus the caller's own facing.
    //  Restored here after a concurrent-session merge race dropped these
    //  two fields (see commit 665c4322ebcc, which added them originally)
    //  and broke every TU's build until the JASS VM finalizer noticed.
    float            m_maxPitch;         // 0x98
    float            m_maxRoll;          // 0x9C
    //  0xA0 - a plain int/pointer property, "elevPoints" - read as a bare
    //  word with no null check on the caller's side, unlike the float
    //  properties above (sub_6F276AD0, footprintelevation.cpp).  Named
    //  `int` rather than a pointer type: nothing in the one call tree that
    //  reads it ever dereferences it, only branches on its value.
    int              m_elevPoints;      // 0xA0
    //  0xA4 - a plain hardware float, "elevRadius", same shape as
    //  m_zOffset (0.0f fallback).  See footprintelevation.cpp.
    float            m_elevRadius;      // 0xA4
    char             m_reservedA8[0xAC - 0xA8];
    //  0xAC - a plain int property, "modelColor", -1 fallback (not 0) when
    //  the type has no record - unlike every other pair-return/scalar
    //  property here.  Named by 0x6F32D390 (footprintshadow.cpp), the same
    //  hidden-return-pointer shape as the two float pairs above, wrapping
    //  one int the way relationconstant.h's SRelationColorConstant does.
    int              m_modelColor;      // 0xAC
    //  0xB0 - a plain hardware float, the type's own "modelScale" property,
    //  1.0f fallback when the type has no record - same shape as
    //  m_scaleFactor/m_zOffset one property family over.  Named by
    //  0x6F32D3C0 (footprintmodelscale.cpp).
    float            m_modelScale;      // 0xB0
    //  0xB4 - the type's own "IsStructure" property, read back raw with no
    //  null check on the caller's side (CUnit's per-template initialiser,
    //  0x6F2A0E30) - unlike GetFootprintShape/GetFootprintScaleFactor,
    //  nothing here has ever needed the "not registered" fallback.
    unsigned int     m_isStructure;   // 0xB4
    char             m_reservedB8[0xCC - 0xB8];
    CFootprintShape* m_footprint;    // 0xCC
    char             m_reservedD0[0x16C - 0xD0];
    //  0x16C/0x170 - "dependencyOr", a plain counted array of rawcode
    //  type ids: any one of them satisfies this type's own dependency
    //  requirement.  Read by GetFootprintDependencyOrCount/
    //  GetFootprintDependencyOrEntry (footprintdependency.cpp,
    //  cability_depth5_closure_worklist.json batch 3).
    unsigned int     m_dependencyOrCount;  // 0x16C
    unsigned int*    m_dependencyOr;       // 0x170
    //  0x178/0x17C - "upgradesIds", a jagged array: m_upgradeGroupCount
    //  groups, each a 12-byte SUpgradeIdGroup naming how many rawcodes it
    //  holds and where they are.  Read by GetFootprintUpgradeIdCount/
    //  GetFootprintUpgradeId (footprintdependency.cpp).
    struct SUpgradeIdGroup
    {
        unsigned int m_field0;
        unsigned int m_idCount;
        unsigned int* m_ids;
    };
    unsigned int     m_upgradeGroupCount;  // 0x178
    SUpgradeIdGroup* m_upgradeGroups;      // 0x17C
    char             m_reserved180[0x10];
    //  0x190/0x194, 0x1A8/0x1AC, 0x1C0/0x1C4, 0x1D8/0x1DC, 0x1F0/0x1F4,
    //  0x208/0x20C - six more jagged arrays with the identical
    //  count/SUpgradeIdGroup[]-array shape m_upgradeGroupCount/
    //  m_upgradeGroups already established, one per unit-type dependency
    //  list ("buildsIds", "trainsIds", "researchesIds", "sellsUnitIds",
    //  "sellsItemIds", "makesItemIds").  Read by the Count/Group accessor
    //  pairs in footprintdependency.cpp
    //  (cunit_vtable_closure_worklist.json, slot 107 closure batch 24).
    unsigned int     m_buildGroupCount;    // 0x190
    SUpgradeIdGroup* m_buildGroups;        // 0x194
    char             m_reserved198[0x10];
    unsigned int     m_trainGroupCount;    // 0x1A8
    SUpgradeIdGroup* m_trainGroups;        // 0x1AC
    char             m_reserved1B0[0x10];
    unsigned int     m_researchGroupCount; // 0x1C0
    SUpgradeIdGroup* m_researchGroups;     // 0x1C4
    char             m_reserved1C8[0x10];
    unsigned int     m_sellUnitGroupCount; // 0x1D8
    SUpgradeIdGroup* m_sellUnitGroups;     // 0x1DC
    char             m_reserved1E0[0x10];
    unsigned int     m_sellItemGroupCount; // 0x1F0
    SUpgradeIdGroup* m_sellItemGroups;     // 0x1F4
    char             m_reserved1F8[0x10];
    unsigned int     m_makeItemGroupCount; // 0x208
    SUpgradeIdGroup* m_makeItemGroups;     // 0x20C
    char             m_reserved210[0xC];
    //  0x21C - "revive": a single rawcode, read back raw with no clamp and
    //  no null-record fallback beyond the shared 0 GetOrCreateFootprintTypeData
    //  gives every property here.  See GetFootprintReviveId
    //  (footprintdependency.cpp).
    unsigned int     m_reviveId;           // 0x21C
    char             m_reserved220[0x1C];
    //  0x23C / 0x240 - a second counted array of strings, "specialArt",
    //  the same shape as m_nameCount/m_names (0x28/0x2C) one property
    //  over.  Named by 0x6F32DC30 (footprinttypespecialart.cpp), which
    //  clamps its own `index` argument the identical way
    //  GetFootprintTypeName does.
    unsigned int     m_specialArtCount; // 0x23C
    const char**     m_specialArt;      // 0x240
    char             m_reserved244[0x54];  //  size not established beyond here
};

class CFootprintTypeTable : public TSHashTable<CFootprintTypeData, AGILE_TYPE_ID>
{
public:
    virtual ~CFootprintTypeTable();
    virtual CFootprintTypeData* AllocNode(
        TSExplicitList<CFootprintTypeData>* bucket, void* arg1, void* arg2);
};

//  The table singleton lives inline in the data segment at this address -
//  never a pointer to one allocated elsewhere. The call sites prove it:
//  `mov ecx, offset dword_6FAB58D4` loads the *address* of the symbol
//  itself as `this` for Ptr()/NewNode(), not a value loaded through it.
//  Never given a defining declaration in any .cpp here (same as
//  g_CFloatZero in CFloat.h) - it already exists, built by the game's own
//  untouched startup code; this TU only needs to name its address so the
//  injection binder can point references at it.
extern CFootprintTypeTable g_footprintTypeTable;   // dword_6FAB58D4

//----------------------------------------------------------------------------
//  0x6F32C880 - find a type's node, registering one if this is the first
//  time it has been asked about.  `debugName` is never read: same dead
//  __FILE__-style tag every Storm-backed lookup in this binary carries, kept
//  as a parameter only because the caller supplies it.
//----------------------------------------------------------------------------
//  The type id arrives as a bare FourCC in ecx and is wrapped in an
//  AGILE_TYPE_ID local inside - VC8 never passes a class type in a register,
//  so a by-value AGILE_TYPE_ID parameter would land on the stack instead.
CFootprintTypeData* __fastcall GetOrCreateFootprintTypeData(
    unsigned int id, const char* debugName);

//----------------------------------------------------------------------------
//  0x6F32D0B0 - a type's footprint shape, or null if it does not have one.
//----------------------------------------------------------------------------
CFootprintShape* __fastcall GetFootprintShape(unsigned int id);

//----------------------------------------------------------------------------
//  0x6F32CFD0 - a type's own "modelPath" property (the +0x34 pointer), or
//  null when the type has no record at all - same shape as GetFootprintShape
//  one property family over.  See footprintmodelpath.cpp.
//----------------------------------------------------------------------------
const char* __fastcall GetFootprintModelPath(unsigned int id);

//----------------------------------------------------------------------------
//  0x6F32D3C0 - a type's own "modelScale" property (the +0xB0 float), or
//  1.0f when the type has no record at all - same shape as
//  GetFootprintScaleFactor.  See footprintmodelscale.cpp.
//----------------------------------------------------------------------------
float __fastcall GetFootprintModelScale(unsigned int id);

//----------------------------------------------------------------------------
//  0x6F32DBE0 - one of a type's own display-name strings, clamped to the
//  last one it has, or the literal "Default string" when the type has no
//  record or no names at all.  The third member of the
//  GetOrCreateFootprintTypeData-plus-one-property-read family, and the only
//  one with a second real parameter: `index` arrives in edx and survives the
//  `mov edx, offset "name"` the call needs by being saved into esi first.
//----------------------------------------------------------------------------
const char* __fastcall GetFootprintTypeName(unsigned int id, unsigned int index);

//----------------------------------------------------------------------------
//  0x6F32D070 - the same shape one property over: a type's own
//  "scaleFactor", or 1.0 when the type has no record at all (note the
//  different fallback - GetFootprintShape answers null, this one answers
//  the identity scale).  See widget_footprintscale.cpp.
//----------------------------------------------------------------------------
float __fastcall GetFootprintScaleFactor(unsigned int id);

//----------------------------------------------------------------------------
//  0x6F32CC20 - a type's "SelCircleOnWater" flag (the +0x94 property).  See
//  footprintselcircleonwater.cpp.
//----------------------------------------------------------------------------
unsigned int __fastcall GetSelCircleOnWaterFlag(unsigned int id);

//----------------------------------------------------------------------------
//  0x6F32CC80 - a type's "IsStructure" flag (the +0xB4 property).  Reached
//  from CUnit's per-template initialiser (0x6F2A0E30).  See
//  footprintisstructure.cpp.
//----------------------------------------------------------------------------
unsigned int __fastcall GetFootprintIsStructureFlag(unsigned int id);

//----------------------------------------------------------------------------
//  0x6F32D090 - a type's "zOffset" property (the +0x58 float), or 0.0f when
//  the type has no record at all.  See footprintzoffset.cpp.
//----------------------------------------------------------------------------
float __fastcall GetFootprintZOffset(unsigned int id);

//----------------------------------------------------------------------------
//  0x6F32D350 - a type's "elevRadius" property (the +0xA4 float), or 0.0f
//  when the type has no record at all.  0x6F32D330 - its "elevPoints"
//  property (the +0xA0 word), or 0 with no record.  See
//  footprintelevation.cpp.
//----------------------------------------------------------------------------
float __fastcall GetFootprintElevRadius(unsigned int id);
int   __fastcall GetFootprintElevPoints(unsigned int id);

//----------------------------------------------------------------------------
//  0x6F32D2F0 - a type's "maxPitch" property (the +0x98 float).  0x6F32D310
//  - its "maxRoll" property (the +0x9C float).  0.0f fallback, same shape
//  as GetFootprintZOffset.  See footprintelevation.cpp.
//----------------------------------------------------------------------------
float __fastcall GetFootprintMaxPitch(unsigned int id);
float __fastcall GetFootprintMaxRoll(unsigned int id);

//----------------------------------------------------------------------------
//  0x6F32D030 - a type's "shadow" property (the +0x4C pointer), or null when
//  the type has no record at all.  0x6F32CBE0 - its "ShadowOnWater" flag
//  (the +0x90 word), read back raw with no null check.  See
//  footprintshadow.cpp.
//----------------------------------------------------------------------------
void*        __fastcall GetFootprintShadow(unsigned int id);
unsigned int __fastcall GetFootprintShadowOnWaterFlag(unsigned int id);

//----------------------------------------------------------------------------
//  0x6F32D050 - a type's "structureShadow" property (the +0x50 pointer), or
//  null when the type has no record at all.  Same shape as GetFootprintShadow
//  one dword over.  See footprintshadow.cpp.
//----------------------------------------------------------------------------
void* __fastcall GetFootprintStructureShadow(unsigned int id);

//----------------------------------------------------------------------------
//  0x6F32DC30 - a type's "specialArt" name at `index`, clamped to the last
//  one it has, or null when the type has no record or no special-art names
//  at all.  See footprinttypespecialart.cpp.
//----------------------------------------------------------------------------
const char* __fastcall GetFootprintSpecialArt(unsigned int id, unsigned int index);

//----------------------------------------------------------------------------
//  0x6F32D440 - a type's own "dependencyOr" count (m_dependencyOrCount),
//  or 0 with no record.  0x6F32DBA0 - one entry of that array, index
//  clamped to the last one.  See footprintdependency.cpp.
//----------------------------------------------------------------------------
unsigned int __fastcall GetFootprintDependencyOrCount(unsigned int id);
unsigned int __fastcall GetFootprintDependencyOrEntry(unsigned int id,
                                                       unsigned int index);

//----------------------------------------------------------------------------
//  0x6F32D7E0 - one "upgradesIds" group's own id count (index clamped to
//  the last group).  0x6F32DB40 - one id out of that group (both indices
//  clamped).  See footprintdependency.cpp.
//----------------------------------------------------------------------------
unsigned int __fastcall GetFootprintUpgradeIdCount(unsigned int id,
                                                    unsigned int groupIndex);
unsigned int __fastcall GetFootprintUpgradeId(unsigned int id,
                                              unsigned int groupIndex,
                                              unsigned int idIndex);

//----------------------------------------------------------------------------
//  0x6F32D660/0x6F32D6A0/0x6F32D6E0/0x6F32D720/0x6F32D760/0x6F32D7A0 - the
//  same "one group's own id count, group index clamped to the last group"
//  shape GetFootprintUpgradeIdCount already documents, one per dependency
//  list.  0x6F32D860/0x6F32D8A0/0x6F32D8E0/0x6F32D920/0x6F32D960/0x6F32D9A0/
//  0x6F32D9E0 - the sibling accessor that returns a pointer to the clamped
//  group struct itself (`&data->m_xGroups[clampedIndex]`) rather than one of
//  its fields, `&g_footprintTypeTable`'s own `unk_6FAB5930` empty-group
//  sentinel when the type has no record at all.  See footprintdependency.cpp
//  (cunit_vtable_closure_worklist.json, slot 107 closure batch 24).
//----------------------------------------------------------------------------
unsigned int __fastcall GetFootprintBuildIdCount(unsigned int id, unsigned int groupIndex);
unsigned int __fastcall GetFootprintTrainIdCount(unsigned int id, unsigned int groupIndex);
unsigned int __fastcall GetFootprintResearchIdCount(unsigned int id, unsigned int groupIndex);
unsigned int __fastcall GetFootprintSellUnitIdCount(unsigned int id, unsigned int groupIndex);
unsigned int __fastcall GetFootprintSellItemIdCount(unsigned int id, unsigned int groupIndex);
unsigned int __fastcall GetFootprintMakeItemIdCount(unsigned int id, unsigned int groupIndex);

CFootprintTypeData::SUpgradeIdGroup* __fastcall GetFootprintBuildGroup(unsigned int id, unsigned int groupIndex);
CFootprintTypeData::SUpgradeIdGroup* __fastcall GetFootprintResearchGroup(unsigned int id, unsigned int groupIndex);
CFootprintTypeData::SUpgradeIdGroup* __fastcall GetFootprintMakeItemGroup(unsigned int id, unsigned int groupIndex);
CFootprintTypeData::SUpgradeIdGroup* __fastcall GetFootprintSellItemGroup(unsigned int id, unsigned int groupIndex);
CFootprintTypeData::SUpgradeIdGroup* __fastcall GetFootprintSellUnitGroup(unsigned int id, unsigned int groupIndex);
CFootprintTypeData::SUpgradeIdGroup* __fastcall GetFootprintTrainGroup(unsigned int id, unsigned int groupIndex);
CFootprintTypeData::SUpgradeIdGroup* __fastcall GetFootprintUpgradeGroup(unsigned int id, unsigned int groupIndex);

//----------------------------------------------------------------------------
//  0x6F32DA20 - a type's own "revive" rawcode (the +0x21C dword), read back
//  raw with no clamp - same no-check shape as GetFootprintIsStructureFlag.
//  See footprintdependency.cpp.
//----------------------------------------------------------------------------
unsigned int __fastcall GetFootprintReviveId(unsigned int id);

#endif
