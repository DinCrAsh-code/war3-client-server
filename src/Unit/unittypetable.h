//============================================================================
//  The unit-type registry at dword_6FAB445C - a fourth instantiation of
//  Storm's TSHashTable, keyed by the type's own four-character id.
//
//  It is the same template the agile-type registry uses (agiletype.h,
//  tshash.inl), and its lookup 0x6F29A8C0 is that template's `Ptr` again:
//  `[ecx+24h]` is m_mask, `[ecx+1Ch]` is m_buckets.m_data, the walk ends on
//  a *signed* `<= 0`, and the match is `m_hashval == hashval &&
//  m_key == key` with the key at +0x14 - which is TSHashObject's own
//  layout for a node with no vftable.
//
//  Only the record fields this repo reads are named.  Everything
//  between them is reserved; the record is at least 0x2BC bytes and
//  nothing here claims to know what else is in it.
//
//  session 2026-08-27b (CUnit slot 107 closure, still a thunk itself - see
//  docs/targets/CUnit__vtable.md) added +0x7C, +0x8C, +0x1A0, +0x1A4,
//  +0x1A8, +0x20C and +0x214 - seven more probes with the identical
//  `Ptr(id.Hash(), id)` shape, called out of 0x6F2A0E30's own callee
//  closure.  +0x1A0 is the one exception:
//  it comes back through `fld`/`fldz`, a real hardware `float`, not the
//  CFloat-out-pointer shape its neighbours at +0x19C/+0x1F8/+0x2B8/+0x7C/
//  +0x8C/+0x20C/+0x214 use.
//============================================================================
#ifndef UNITTYPETABLE_H
#define UNITTYPETABLE_H

#include "agiletype.h"
#include "CFloat.h"

struct SUnitTypeRecord
    : public TSHashObject<SUnitTypeRecord, AGILE_TYPE_ID>
{
    char   m_reserved18[0x20 - 0x18];
    int    m_field20;       // 0x20  - UnitTypeCost20_6F29B800
    int    m_field24;       // 0x24  - UnitTypeCost24_6F29B830
    char   m_reserved28[0x54 - 0x28];
    //  0x54 - one more of the same one-shot probe family, one dword below
    //  m_field5C.  See UnitTypeField54_6F29BA90 (unittypequeries.cpp,
    //  cunit_vtable_closure_worklist.json slot 107 closure batch 24).
    int    m_field54;       // 0x54
    char   m_reserved58[0x5C - 0x58];
    int    m_field5C;       // 0x5C  - UnitTypeField5C_6F29BAF0
    int    m_field60;       // 0x60  - UnitTypeField60_6F29BB20
    char   m_reserved64[0x74 - 0x64];
    CFloat m_field74;       // 0x74  - UnitTypeField74_6F29BDB0
    int    m_field78;       // 0x78  - UnitTypeField78_6F29BDF0
    CFloat m_field7C;       // 0x7C  - UnitTypeField7C_6F29BE20
    //  0x80/0x84/0x88/0x8C/0x90 - five more CFloat-out-pointer probes,
    //  packed at 4-byte stride right after +0x7C.  +0x80 is
    //  ResolveConfigFloatById's own field (Math/cunit_agent3_scaledroundedfloat
    //  .cpp's caller; that file forward-declares it and this module defines
    //  it, since the address is this table's, not that TU's own module).
    CFloat m_field80;       // 0x80  - ResolveConfigFloatById (0x6F29BEA0)
    CFloat m_field84;       // 0x84  - UnitTypeField84_6F29BE60
    CFloat m_field88;       // 0x88  - UnitTypeField88_6F29BEE0
    CFloat m_field8C;       // 0x8C  - UnitTypeField8C_6F29BF20
    CFloat m_field90;       // 0x90  - UnitTypeField90_6F29BF60
    char   m_reserved94[0x98 - 0x94];
    int    m_field98;       // 0x98  - UnitTypeField98_6F29BFE0
    int    m_field9C;       // 0x9C  - UnitTypeField9C_6F29CB80
    //  0xA0 through 0x158 - PopulateUnitTypeInstance's (unit_typeinstance
    //  populate.cpp) own two-iteration loop reads all of these, each a
    //  two-element per-slot array (4 bytes/element, `Ptr(id.Hash(), id)`
    //  re-run with the same fourCC, `edx`/stack = the loop's own 0/1 slot
    //  index).  Two element shapes recur, matching the two already-named
    //  singular shapes above: a plain `int` pair (no out pointer, retn 0,
    //  miss = 0 - UnitTypeField9C_6F29CB80's own shape) and a `CFloat` pair
    //  (out pointer, retn 4, miss = g_CFloatZero - UnitTypeRadius_6F29C110's
    //  own shape, but g_slopeThreshold1 at +0x118 instead).
    int    m_arrA0[2];      // 0xA0/0xA4  - UnitTypeSlotFieldA0_6F29CC20
    char   m_reservedA8[0xB0 - 0xA8];
    int    m_arrB0[2];      // 0xB0/0xB4  - UnitTypeSlotFieldB0_6F29CCA0
    int    m_arrB8[2];      // 0xB8/0xBC  - UnitTypeSlotFieldB8_6F29CCE0
    int    m_arrC0[2];      // 0xC0/0xC4  - UnitTypeSlotFieldC0_6F29CD20
    int    m_arrC8[2];      // 0xC8/0xCC  - UnitTypeSlotFieldC8_6F29CD60
    CFloat m_arrD0[2];      // 0xD0/0xD4  - UnitTypeSlotFieldD0_6F29CDA0
    CFloat m_arrD8[2];      // 0xD8/0xDC  - UnitTypeSlotFieldD8_6F29CDF0
    CFloat m_arrE0[2];      // 0xE0/0xE4  - UnitTypeSlotFieldE0_6F29CE40
    int    m_arrE8[2];      // 0xE8/0xEC  - UnitTypeSlotFieldE8_6F29CE90
    int    m_arrF0[2];      // 0xF0/0xF4  - UnitTypeSlotFieldF0_6F29CED0
    int    m_arrF8[2];      // 0xF8/0xFC  - UnitTypeSlotFieldF8_6F29CF10
    CFloat m_arr100[2];     // 0x100/0x104 - UnitTypeSlotField100_6F29CF50
    CFloat m_arr108[2];     // 0x108/0x10C - UnitTypeSlotField108_6F29CFA0
    CFloat m_arr110[2];     // 0x110/0x114 - UnitTypeSlotField110_6F29CFF0
    //  +0x118's own miss-arm default is g_slopeThreshold1 (encoded 1.0),
    //  not g_CFloatZero like every other CFloat probe in this table.
    CFloat m_arr118[2];     // 0x118/0x11C - UnitTypeSlotField118_6F29D040
    CFloat m_arr120[2];     // 0x120/0x124 - UnitTypeSlotField120_6F29D090
    int    m_arr128[2];     // 0x128/0x12C - UnitTypeSlotField128_6F29D0E0
    CFloat m_arr130[2];     // 0x130/0x134 - UnitTypeSlotField130_6F29D120
    CFloat m_arr138[2];     // 0x138/0x13C - UnitTypeSlotField138_6F29D170
    CFloat m_arr140[2];     // 0x140/0x144 - UnitTypeSlotField140_6F29D1C0
    CFloat m_arr148[2];     // 0x148/0x14C - UnitTypeSlotField148_6F29D210
    CFloat m_arr150[2];     // 0x150/0x154 - UnitTypeSlotField150_6F29D260
    char   m_reserved158[0x168 - 0x158];
    //  0x168 - the loop's one outlier: same two-element int-pair shape as
    //  +0x9C/+0xA0/etc, but the miss arm returns 1, not 0.
    int    m_arr168[2];     // 0x168/0x16C - UnitTypeSlotField168_6F29D2B0
    //  0x170 through 0x18C - back to one-shot (non-indexed) probes, packed
    //  at 4-byte stride: four plain ints (miss = 0, UnitTypeField5C_6F29BAF0's
    //  shape) then four CFloats (miss = g_CFloatZero, UnitTypeRadius_6F29C110's
    //  shape).
    int    m_field170;      // 0x170  - UnitTypeField170_6F29BBB0
    int    m_field174;      // 0x174  - UnitTypeField174_6F29BBE0
    int    m_field178;      // 0x178  - UnitTypeField178_6F29BC10
    int    m_field17C;      // 0x17C  - UnitTypeField17C_6F29BC40
    CFloat m_field180;      // 0x180  - UnitTypeField180_6F29BC70
    CFloat m_field184;      // 0x184  - UnitTypeField184_6F29BCB0
    CFloat m_field188;      // 0x188  - UnitTypeField188_6F29BCF0
    CFloat m_field18C;      // 0x18C  - UnitTypeField18C_6F29C010
    //  0x190 - cunit_agent1_worklist.md's own probe
    //  (UnitTypeField190_6F29F020, Unit/cunit_agent1_unittypefield190.cpp),
    //  same CFloat-out-pointer shape as +0x74/+0x7C/+0x8C.
    CFloat m_field190;      // 0x190
    CFloat m_field194;      // 0x194  - UnitTypeField194_6F29C090
    CFloat m_field198;      // 0x198  - UnitTypeField198_6F29C0D0
    CFloat m_radius;        // 0x19C - UnitTypeRadius_6F29C110
    float  m_field1A0;      // 0x1A0 - UnitTypeFloatField1A0_6F29C150 (real
                             //         hardware float - returned via `fld`,
                             //         not a CFloat out-parameter like its
                             //         neighbours)
    int    m_field1A4;      // 0x1A4 - UnitTypeIntField1A4_6F29C180
    int    m_field1A8;      // 0x1A8 - UnitTypeIntField1A8_6F29C240
    char   m_reserved1AC[0x1B0 - 0x1AC];
    int    m_flag1B0;       // 0x1B0 - UnitTypeFlag_6F29C2A0
    char   m_reserved1B4[0x1C0 - 0x1B4];
    //  0x1C0 - CAbility::GetPlacementCursorTexture's own owner-race probe
    //  (UnitTypeField_6F29C1B0, ability_slots.cpp), reached through a
    //  hash-and-Ptr lookup keyed by a FourCC read off the ability's
    //  resolved owner object rather than the ability's own m_typeId.
    int    m_field1C0;      // 0x1C0
    //  0x1C4/0x1C8 - two more plain-int one-shot probes, the same
    //  UnitTypeField5C_6F29BAF0 shape, right after +0x1C0.
    int    m_field1C4;      // 0x1C4  - UnitTypeField1C4_6F29C1E0
    int    m_field1C8;      // 0x1C8  - UnitTypeField1C8_6F29C210
    char   m_reserved1CC[0x1D0 - 0x1CC];
    //  0x1D0 - UnitTypePointValue_6F29C4D0, which the JASS native
    //  "GetUnitPointValueByType" (0x6F3B2E20) is two instructions into.
    //  That registration is what names this word; nothing else in the call
    //  tree does.
    int    m_pointValue;    // 0x1D0
    //  0x1D4 through 0x1F4 - seven more probes packed at 4-byte stride,
    //  alternating the CFloat-out-pointer and plain-int shapes exactly as
    //  the disassembly has them.  +0x1E0/+0x1E4's own miss-arm default is
    //  g_CFloatHalf, not g_CFloatZero - the only two fields in the whole
    //  table that fall back to 0.5 instead of 0.0.
    CFloat m_field1D4;      // 0x1D4  - UnitTypeField1D4_6F29C500
    char   m_reserved1D8[0x1E0 - 0x1D8];
    CFloat m_field1E0;      // 0x1E0  - UnitTypeField1E0_6F29C5C0 (miss = g_CFloatHalf)
    CFloat m_field1E4;      // 0x1E4  - UnitTypeField1E4_6F29C600 (miss = g_CFloatHalf)
    int    m_field1E8;      // 0x1E8  - UnitTypeField1E8_6F29C640
    int    m_field1EC;      // 0x1EC  - UnitTypeField1EC_6F29C670
    CFloat m_field1F0;      // 0x1F0  - UnitTypeField1F0_6F29C6A0
    int    m_field1F4;      // 0x1F4  - UnitTypeField1F4_6F29C6E0
    CFloat m_stateValue;    // 0x1F8 - UnitTypeStateValue_6F29C710
    //  0x1FC/0x200-0x208 - one more CFloat one-shot probe immediately
    //  followed by a three-float vector probe (UnitTypeVec200_6F29C790,
    //  P3 shape: out pointer to three consecutive dwords, all three
    //  zeroed on a miss rather than loaded from a shared constant).
    CFloat m_field1FC;      // 0x1FC  - UnitTypeField1FC_6F29C750
    float  m_vec200[3];     // 0x200/0x204/0x208 - UnitTypeVec200_6F29C790
    CFloat m_field20C;      // 0x20C - UnitTypeField20C_6F29C7F0
    CFloat m_field210;      // 0x210 - UnitTypeField210_6F29C830
    CFloat m_field214;      // 0x214 - UnitTypeField214_6F29C870
    //  0x218/0x21C/0x220 - three real hardware-`float` probes (fld/fldz,
    //  not a CFloat out-pointer), the same shape as m_field1A0 above.
    float  m_field218;      // 0x218  - UnitTypeFloatField218_6F29C8B0
    float  m_field21C;      // 0x21C  - UnitTypeFloatField21C_6F29C8E0
    float  m_field220;      // 0x220  - UnitTypeFloatField220_6F29C910
    int    m_field224;      // 0x224  - UnitTypeField224_6F29CA30
    //  0x228 - LookupSecondRegistryField0x228_6F29CA60 (cunit_agent8_batch1
    //  .cpp): CUnit's own vtable BFS closure reached this record through a
    //  different lookup than the one-shot probes above, but it is the same
    //  g_unitTypeTable/Ptr(id.Hash(), id) shape, just returning this field
    //  as a raw pointer rather than an int/CFloat.  Not otherwise named.
    void*  m_field228;      // 0x228
    //  0x22C - one more plain-int probe, packed right after +0x228.
    int    m_field22C;      // 0x22C  - UnitTypeField22C_6F29CA90
    //  0x230 - cunit_agent1_worklist.md's own probe
    //  (UnitTypeField230_6F29CAC0, Unit/cunit_agent1_unittypefield230.cpp),
    //  same one-shot `Ptr(id.Hash(), id)` shape as every other member here.
    int    m_field230;      // 0x230
    char   m_reserved234[0x26C - 0x234];
    //  0x26C - cunit_agent7 batch: UnitTypeIntField26C_6F29CA00
    //  (unittypequeries.cpp), same `Ptr(id.Hash(), id)` / found-arm-first /
    //  plain-int-return shape as UnitTypeIntField1A8_6F29C240 right above.
    int    m_field26C;      // 0x26C
    //  0x270 - one more plain-int probe.
    int    m_field270;      // 0x270  - UnitTypeField270_6F29CAF0
    //  0x274/0x280 - two embedded SHandleHolder-shaped fields (Agent/
    //  handleobject.h), each read through GetHandleOrZeroAlias(0x6F4C4660)
    //  rather than a direct field load - `lea ecx, [type+OFFSET]` then a
    //  real call, not an inline dereference, matching every other call site
    //  of that alias in this repo.  Declared as raw bytes here (the layout
    //  itself is handleobject.h's, not this table's) and reinterpreted at
    //  the call site.
    char   m_handle274[0x280 - 0x274];  // UnitTypeHandle274_6F29CB20
    char   m_handle280[0x28C - 0x280];  // UnitTypeHandle280_6F29CB50
    char   m_reserved28C[0x2B8 - 0x28C];
    CFloat m_value2B8;      // 0x2B8 - UnitTypeValue_6F29CBE0
};

//  The concrete table.  Its two virtuals are declared only so the slots
//  exist; nothing here constructs one or calls one, so no vtable of it is
//  emitted - the same shape AGILE_TYPE_TABLE already has.
class CUnitTypeTable : public TSHashTable<SUnitTypeRecord, AGILE_TYPE_ID>
{
public:
    virtual ~CUnitTypeTable();
    virtual SUnitTypeRecord* AllocNode(TSExplicitList<SUnitTypeRecord>* bucket,
                                       void* arg1, void* arg2);
};

//  dword_6FAB445C.  A global *object*, not a pointer: the shipped code
//  loads its address with `mov ecx, offset dword_6FAB445C`.
extern CUnitTypeTable g_unitTypeTable;

#endif
