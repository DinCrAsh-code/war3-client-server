//============================================================================
//  0x6F29C2A0 / 0x6F29C110 / 0x6F29C710 / 0x6F29CBE0 - the four unit-type
//  registry probes CUnit's placement and bound-value slots make
//  (unit_placement.cpp, unit_boundvalue2.cpp).  All four were naked
//  redirects in unreconstructed_thunks.cpp.
//
//  One shape, four field offsets:
//
//      0x6F29C2A0  +0x1B0  an int flag, 0 when the type is unknown
//      0x6F29C110  +0x19C  a CFloat, g_CFloatZero when the type is unknown
//      0x6F29C710  +0x1F8  ditto
//      0x6F29CBE0  +0x2B8  ditto
//
//  **The nested `Ptr(id.Hash(), id)` spelling, not the staged one.**  Every
//  one of these pushes `&id` *before* calling Hash(), which is exactly what
//  agent_releaseself.cpp's own note says the nested form does and the
//  staged form (a named `hash` local) does not.  That file needs the staged
//  one because its shipped code calls Hash first; these four need this one.
//
//  The three CFloat probes return their own out buffer - the shipped
//  `mov eax, esi` on both arms - so they are declared to return `CFloat*`
//  even though both call sites drop it.  Declared `void` the `mov` has
//  nowhere to come from.
//
//  Own translation unit: the four do not call each other, and all four have
//  to reach 0x6F29A8C0 (unittypetable.cpp) with a real call.
//============================================================================
#include "unittypetable.h"
#include "unittypequeries.h"
#include "handleobject.h"

//  dword_6FAAE4D0 / dword_6FAAE4F4 - CFloat.h's own shared constants (0.5
//  and encoded 1.0 respectively), two of the miss-arm defaults
//  createunit-G1's own batch below falls back to.
extern const CFloat g_CFloatHalf;       // dword_6FAAE4D0
extern const CFloat g_slopeThreshold1;  // dword_6FAAE4F4

int __fastcall UnitTypeFlag_6F29C2A0(unsigned int fourCC)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    //  The found arm first here too, and as a conditional expression rather
    //  than a guard: `if (type == 0) return 0;` puts the zero return in the
    //  middle of the body with a backwards branch into it, where the
    //  shipped code has one forward `jz` onto a trailing `xor eax, eax`.
    return type != 0 ? type->m_flag1B0 : 0;
}

CFloat* __fastcall UnitTypeRadius_6F29C110(CFloat* out, unsigned int fourCC)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    //  The *found* arm first: the shipped `jz` jumps forward to the
    //  g_CFloatZero store, so the field read is the fall-through.  Written
    //  the other way round the two swap and both arms miss.
    if (type != 0)
        *out = type->m_radius;
    else
        *out = g_CFloatZero;
    return out;
}

CFloat* __fastcall UnitTypeStateValue_6F29C710(CFloat* out, unsigned int fourCC)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    if (type != 0)
        *out = type->m_stateValue;
    else
        *out = g_CFloatZero;
    return out;
}

CFloat* __fastcall UnitTypeValue_6F29CBE0(CFloat* out, unsigned int fourCC)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    if (type != 0)
        *out = type->m_value2B8;
    else
        *out = g_CFloatZero;
    return out;
}

//----------------------------------------------------------------------------
//  0x6F29BAF0 / 0x6F29BB20 - two more of the same probe, both called
//  directly by the unit factory 0x6F29F990 (jasscreateunit.h) rather than
//  by a CUnit slot.  Same nested `Ptr(id.Hash(), id)`, same found-arm-first
//  conditional, two adjacent int fields at +0x5C and +0x60.
//
//  What those two are is now known, though the names below are left alone
//  because every existing call site reads them: the JASS registration table
//  binds 0x6F3B2E10 to "GetFoodUsed" and 0x6F3B2E00 to "GetFoodMade", and
//  each of those two natives is `mov ecx,[esp+4]` / `jmp` straight into one
//  of these - 0x6F29BAF0 (+0x5C) is the food a unit type *uses*, 0x6F29BB20
//  (+0x60) is the food it *makes*.
//
//  They live here rather than in a file of their own for the reason the
//  header above gives: 0x6F29BAF0 and 0x6F29C2A0 are the same module, none
//  of the six calls another, and every one of them has to reach 0x6F29A8C0
//  with a real call.
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//  0x6F29C4D0 - the same probe again, reading the type's point value at
//  +0x1D0.  The JASS native "GetUnitPointValueByType" (0x6F3B2E20) is two
//  instructions into this one, which is what names the field.
//
//  It has no `### ... @ 0x...` heading anywhere in asm/ and so gets no
//  funcmap.py row and no score: IDA models 0x6F29C4D0 as a *tail chunk* of
//  the three functions that `jmp` into it (0x6F3B2E20, 0x6F3BA9A3 and
//  0x6F3C5E80) rather than as a function of its own - the shape
//  docs/msvc-vc8-idioms.md calls "a shared tail chunk is not a score you
//  can chase".  It is written out because the .mix needs something for
//  JASS_GetUnitPointValueByType to jump to.
//----------------------------------------------------------------------------
int __fastcall UnitTypePointValue_6F29C4D0(unsigned int fourCC)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    return type != 0 ? type->m_pointValue : 0;
}

int __fastcall UnitTypeField5C_6F29BAF0(unsigned int fourCC)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    return type != 0 ? type->m_field5C : 0;
}

int __fastcall UnitTypeField60_6F29BB20(unsigned int fourCC)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    return type != 0 ? type->m_field60 : 0;
}

//----------------------------------------------------------------------------
//  0x6F29BDB0 / 0x6F29BDF0 - two more of the same probe family (CUnit
//  batch-12, slot 3 closure - reached from 0x6F29D570, not slot 107).
//  +0x74 is the CFloat-out-pointer shape (UnitTypeRadius_6F29C110's own);
//  +0x78 is the plain-`int`-return shape (UnitTypeFlag_6F29C2A0's own) -
//  same nested `Ptr(id.Hash(), id)`, same found-arm-first structure.
//----------------------------------------------------------------------------
CFloat* __fastcall UnitTypeField74_6F29BDB0(CFloat* out, unsigned int fourCC)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    if (type != 0)
        *out = type->m_field74;
    else
        *out = g_CFloatZero;
    return out;
}

int __fastcall UnitTypeField78_6F29BDF0(unsigned int fourCC)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    return type != 0 ? type->m_field78 : 0;
}

//----------------------------------------------------------------------------
//  0x6F29BE20 / 0x6F29BF20 / 0x6F29C7F0 / 0x6F29C870 - four more CFloat
//  out-pointer probes (+0x7C, +0x8C, +0x20C, +0x214), called out of
//  CUnit's still-thunked slot 107 (0x6F2A0E30, docs/targets/CUnit__vtable.md).
//  Same shape as UnitTypeRadius_6F29C110 above: `Ptr(id.Hash(), id)`, found
//  arm first, default is a straight 4-byte copy of g_CFloatZero.
//----------------------------------------------------------------------------
CFloat* __fastcall UnitTypeField7C_6F29BE20(CFloat* out, unsigned int fourCC)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    if (type != 0)
        *out = type->m_field7C;
    else
        *out = g_CFloatZero;
    return out;
}

CFloat* __fastcall UnitTypeField8C_6F29BF20(CFloat* out, unsigned int fourCC)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    if (type != 0)
        *out = type->m_field8C;
    else
        *out = g_CFloatZero;
    return out;
}

CFloat* __fastcall UnitTypeField20C_6F29C7F0(CFloat* out, unsigned int fourCC)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    if (type != 0)
        *out = type->m_field20C;
    else
        *out = g_CFloatZero;
    return out;
}

//----------------------------------------------------------------------------
//  0x6F29C830 - CUnit slot 70 (0x6F29E570)'s own closure, not slot 107's:
//  the +0x210 sibling immediately below +0x214, same
//  `Ptr(id.Hash(), id)` / found-arm-first / g_CFloatZero-default shape as
//  UnitTypeField214_6F29C870 right below it.
//----------------------------------------------------------------------------
CFloat* __fastcall UnitTypeField210_6F29C830(CFloat* out, unsigned int fourCC)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    if (type != 0)
        *out = type->m_field210;
    else
        *out = g_CFloatZero;
    return out;
}

CFloat* __fastcall UnitTypeField214_6F29C870(CFloat* out, unsigned int fourCC)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    if (type != 0)
        *out = type->m_field214;
    else
        *out = g_CFloatZero;
    return out;
}

//----------------------------------------------------------------------------
//  0x6F29C150 / 0x6F29C180 / 0x6F29C240 - three plain-return probes, also
//  from slot 107's closure.  +0x1A4 and +0x1A8 are `int`s returned in eax,
//  the same shape as UnitTypeFlag_6F29C2A0 above.  +0x1A0 is the one
//  exception in this whole family: it comes back through `fld`/`fldz`, a
//  real hardware `float` rather than a CFloat out-pointer, so the record
//  field is typed `float`, not `CFloat`.
//----------------------------------------------------------------------------
float __fastcall UnitTypeFloatField1A0_6F29C150(unsigned int fourCC)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    //  Two separate `return`s, not a ternary: the ternary's common-type merge
    //  forces MSVC to round-trip the found value through a stack temp
    //  (fstp/fld) before the shared epilogue, which the shipped code does
    //  not do - it has two independent `fld`/`fldz` + `retn` tails instead.
    if (type != 0)
        return type->m_field1A0;
    return 0.0f;
}

int __fastcall UnitTypeIntField1A4_6F29C180(unsigned int fourCC)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    return type != 0 ? type->m_field1A4 : 0;
}

int __fastcall UnitTypeIntField1A8_6F29C240(unsigned int fourCC)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    return type != 0 ? type->m_field1A8 : 0;
}

//----------------------------------------------------------------------------
//  0x6F29CA00 - cunit_agent7 batch (CUnit closure BFS depth 1-2).  Same
//  `Ptr(id.Hash(), id)` / found-arm-first / plain-int-return shape as
//  UnitTypeIntField1A8_6F29C240 above, one field further on at +0x26C.
//----------------------------------------------------------------------------
int __fastcall UnitTypeIntField26C_6F29CA00(unsigned int fourCC)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    return type != 0 ? type->m_field26C : 0;
}

//----------------------------------------------------------------------------
//  0x6F29C1B0 - a fifth `int` probe, this time reached from
//  CAbility::GetPlacementCursorTexture (ability_slots.cpp, worklist batch
//  E) rather than from a CUnit slot: it looks up the ability's resolved
//  owner's own type FourCC and reads +0x1C0.  Same nested `Ptr(id.Hash(),
//  id)` / found-arm-first shape as UnitTypeFlag_6F29C2A0 above.
//----------------------------------------------------------------------------
int __fastcall UnitTypeField_6F29C1B0(unsigned int fourCC)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    return type != 0 ? type->m_field1C0 : 0;
}

//----------------------------------------------------------------------------
//  0x6F29D2F0 - CAbility depth<=5 closure (via 0x6F2A0E30's own callee
//  closure, the same CUnit slot-107 neighbourhood unittypetable.h's own
//  header note names).  Same nested `Ptr(id.Hash(), id)` shape as every
//  probe above, but a plain existence test rather than a field read -
//  `neg eax / sbb eax,eax / neg eax` is this build's bool-ize of a
//  pointer, the same idiom `UnitTypeFlag_6F29C2A0`'s own family uses a
//  ternary for.  No field of the record is ever read.
//----------------------------------------------------------------------------
int __fastcall UnitTypeIsRegistered_6F29D2F0(unsigned int fourCC)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    return type != 0;
}

//----------------------------------------------------------------------------
//  0x6F29BA90 - one more field read off +0x54, the dword just below the
//  first named one (+0x5C).  Same nested `Ptr(id.Hash(), id)` shape;
//  cunit_vtable_closure_worklist.json, slot 107 closure batch 24.
//----------------------------------------------------------------------------
int __fastcall UnitTypeField54_6F29BA90(unsigned int fourCC)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    return type != 0 ? type->m_field54 : 0;
}
//----------------------------------------------------------------------------
//  createunit-G1's own batch: the CreateUnit closure's remaining probes in
//  this same 0x6F29Bxxx-0x6F29Dxxx module - one-shot plain-int reads.
//----------------------------------------------------------------------------
int __fastcall UnitTypeField9C_6F29CB80(unsigned int fourCC)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    return type != 0 ? type->m_field9C : 0;
}

int __fastcall UnitTypeField170_6F29BBB0(unsigned int fourCC)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    return type != 0 ? type->m_field170 : 0;
}

int __fastcall UnitTypeField174_6F29BBE0(unsigned int fourCC)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    return type != 0 ? type->m_field174 : 0;
}

int __fastcall UnitTypeField178_6F29BC10(unsigned int fourCC)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    return type != 0 ? type->m_field178 : 0;
}

int __fastcall UnitTypeField17C_6F29BC40(unsigned int fourCC)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    return type != 0 ? type->m_field17C : 0;
}

int __fastcall UnitTypeField1C4_6F29C1E0(unsigned int fourCC)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    return type != 0 ? type->m_field1C4 : 0;
}

int __fastcall UnitTypeField1C8_6F29C210(unsigned int fourCC)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    return type != 0 ? type->m_field1C8 : 0;
}

int __fastcall UnitTypeField1E8_6F29C640(unsigned int fourCC)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    return type != 0 ? type->m_field1E8 : 0;
}

int __fastcall UnitTypeField1EC_6F29C670(unsigned int fourCC)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    return type != 0 ? type->m_field1EC : 0;
}

int __fastcall UnitTypeField1F4_6F29C6E0(unsigned int fourCC)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    return type != 0 ? type->m_field1F4 : 0;
}

int __fastcall UnitTypeField224_6F29CA30(unsigned int fourCC)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    return type != 0 ? type->m_field224 : 0;
}

int __fastcall UnitTypeField22C_6F29CA90(unsigned int fourCC)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    return type != 0 ? type->m_field22C : 0;
}

int __fastcall UnitTypeField270_6F29CAF0(unsigned int fourCC)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    return type != 0 ? type->m_field270 : 0;
}

int __fastcall UnitTypeField98_6F29BFE0(unsigned int fourCC)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    return type != 0 ? type->m_field98 : 0;
}

//----------------------------------------------------------------------------
//  One-shot CFloat-out-pointer reads, miss arm = g_CFloatZero.
//----------------------------------------------------------------------------
CFloat* __fastcall UnitTypeField194_6F29C090(CFloat* out, unsigned int fourCC)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    if (type != 0)
        *out = type->m_field194;
    else
        *out = g_CFloatZero;
    return out;
}

CFloat* __fastcall UnitTypeField198_6F29C0D0(CFloat* out, unsigned int fourCC)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    if (type != 0)
        *out = type->m_field198;
    else
        *out = g_CFloatZero;
    return out;
}

CFloat* __fastcall UnitTypeField90_6F29BF60(CFloat* out, unsigned int fourCC)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    if (type != 0)
        *out = type->m_field90;
    else
        *out = g_CFloatZero;
    return out;
}

CFloat* __fastcall UnitTypeField1FC_6F29C750(CFloat* out, unsigned int fourCC)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    if (type != 0)
        *out = type->m_field1FC;
    else
        *out = g_CFloatZero;
    return out;
}

CFloat* __fastcall UnitTypeField180_6F29BC70(CFloat* out, unsigned int fourCC)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    if (type != 0)
        *out = type->m_field180;
    else
        *out = g_CFloatZero;
    return out;
}

CFloat* __fastcall UnitTypeField188_6F29BCF0(CFloat* out, unsigned int fourCC)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    if (type != 0)
        *out = type->m_field188;
    else
        *out = g_CFloatZero;
    return out;
}

CFloat* __fastcall UnitTypeField184_6F29BCB0(CFloat* out, unsigned int fourCC)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    if (type != 0)
        *out = type->m_field184;
    else
        *out = g_CFloatZero;
    return out;
}

CFloat* __fastcall UnitTypeField18C_6F29C010(CFloat* out, unsigned int fourCC)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    if (type != 0)
        *out = type->m_field18C;
    else
        *out = g_CFloatZero;
    return out;
}

CFloat* __fastcall UnitTypeField88_6F29BEE0(CFloat* out, unsigned int fourCC)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    if (type != 0)
        *out = type->m_field88;
    else
        *out = g_CFloatZero;
    return out;
}

CFloat* __fastcall UnitTypeField1D4_6F29C500(CFloat* out, unsigned int fourCC)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    if (type != 0)
        *out = type->m_field1D4;
    else
        *out = g_CFloatZero;
    return out;
}

CFloat* __fastcall UnitTypeField1F0_6F29C6A0(CFloat* out, unsigned int fourCC)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    if (type != 0)
        *out = type->m_field1F0;
    else
        *out = g_CFloatZero;
    return out;
}

CFloat* __fastcall UnitTypeField84_6F29BE60(CFloat* out, unsigned int fourCC)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    if (type != 0)
        *out = type->m_field84;
    else
        *out = g_CFloatZero;
    return out;
}

//----------------------------------------------------------------------------
//  Same shape, miss arm = g_CFloatHalf (encoded 0.5) instead of zero.
//----------------------------------------------------------------------------
CFloat* __fastcall UnitTypeField1E0_6F29C5C0(CFloat* out, unsigned int fourCC)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    if (type != 0)
        *out = type->m_field1E0;
    else
        *out = g_CFloatHalf;
    return out;
}

CFloat* __fastcall UnitTypeField1E4_6F29C600(CFloat* out, unsigned int fourCC)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    if (type != 0)
        *out = type->m_field1E4;
    else
        *out = g_CFloatHalf;
    return out;
}

//----------------------------------------------------------------------------
//  Real hardware-float reads (fld/fldz), same shape as
//  UnitTypeFloatField1A0_6F29C150 above.
//----------------------------------------------------------------------------
float __fastcall UnitTypeFloatField220_6F29C910(unsigned int fourCC)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    if (type != 0)
        return type->m_field220;
    return 0.0f;
}

float __fastcall UnitTypeFloatField21C_6F29C8E0(unsigned int fourCC)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    if (type != 0)
        return type->m_field21C;
    return 0.0f;
}

float __fastcall UnitTypeFloatField218_6F29C8B0(unsigned int fourCC)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    if (type != 0)
        return type->m_field218;
    return 0.0f;
}

//----------------------------------------------------------------------------
//  Embedded-SHandleHolder reads: the found arm calls through to
//  GetHandleOrZeroAlias (Agent/handleobject.cpp/.h) rather than reading the
//  field directly.
//----------------------------------------------------------------------------
unsigned int __fastcall UnitTypeHandle280_6F29CB50(unsigned int fourCC)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    return type != 0 ? GetHandleOrZeroAlias((const SHandleHolder*)&type->m_handle280) : 0;
}

unsigned int __fastcall UnitTypeHandle274_6F29CB20(unsigned int fourCC)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    return type != 0 ? GetHandleOrZeroAlias((const SHandleHolder*)&type->m_handle274) : 0;
}

//----------------------------------------------------------------------------
//  0x6F29C790 - the one three-float-vector probe, out pointer to three
//  consecutive dwords, all three zeroed on a miss.
//----------------------------------------------------------------------------
unsigned int* __fastcall UnitTypeVec200_6F29C790(unsigned int* out, unsigned int fourCC)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    if (type != 0)
    {
        out[0] = ((unsigned int*)type->m_vec200)[0];
        out[1] = ((unsigned int*)type->m_vec200)[1];
        out[2] = ((unsigned int*)type->m_vec200)[2];
    }
    else
    {
        //  Reverse store order on the miss arm ([esi+8] then [esi+4] then
        //  [esi]) - not a guess, the shipped code really writes the third
        //  word first.  Written the other way round the whole three-store
        //  block is still correct C++ but the compiler emits it forwards
        //  and the diff shows a 3-instruction reorder for nothing.
        out[2] = 0;
        out[1] = 0;
        out[0] = 0;
    }
    return out;
}

//----------------------------------------------------------------------------
//  Unit/unit_typeinstancepopulate.cpp's own per-slot loop: two-element
//  `Ptr(id.Hash(), id)` arrays, plain-int shape, miss = 0.
//----------------------------------------------------------------------------
int __fastcall UnitTypeSlotFieldA0_6F29CC20(unsigned int fourCC, int slot)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    return type != 0 ? type->m_arrA0[slot] : 0;
}

int __fastcall UnitTypeSlotFieldB0_6F29CCA0(unsigned int fourCC, int slot)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    return type != 0 ? type->m_arrB0[slot] : 0;
}

int __fastcall UnitTypeSlotFieldC8_6F29CD60(unsigned int fourCC, int slot)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    return type != 0 ? type->m_arrC8[slot] : 0;
}

int __fastcall UnitTypeSlotFieldE8_6F29CE90(unsigned int fourCC, int slot)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    return type != 0 ? type->m_arrE8[slot] : 0;
}

int __fastcall UnitTypeSlotFieldF8_6F29CF10(unsigned int fourCC, int slot)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    return type != 0 ? type->m_arrF8[slot] : 0;
}

int __fastcall UnitTypeSlotField128_6F29D0E0(unsigned int fourCC, int slot)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    return type != 0 ? type->m_arr128[slot] : 0;
}

int __fastcall UnitTypeSlotFieldC0_6F29CD20(unsigned int fourCC, int slot)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    return type != 0 ? type->m_arrC0[slot] : 0;
}

int __fastcall UnitTypeSlotFieldF0_6F29CED0(unsigned int fourCC, int slot)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    return type != 0 ? type->m_arrF0[slot] : 0;
}

int __fastcall UnitTypeSlotFieldB8_6F29CCE0(unsigned int fourCC, int slot)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    return type != 0 ? type->m_arrB8[slot] : 0;
}

//----------------------------------------------------------------------------
//  Same per-slot shape, but the miss arm returns 1, not 0.
//----------------------------------------------------------------------------
int __fastcall UnitTypeSlotField168_6F29D2B0(unsigned int fourCC, int slot)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    return type != 0 ? type->m_arr168[slot] : 1;
}

//----------------------------------------------------------------------------
//  Same per-slot loop, CFloat-out-pointer shape, miss = g_CFloatZero.
//----------------------------------------------------------------------------
CFloat* __fastcall UnitTypeSlotField100_6F29CF50(CFloat* out, unsigned int fourCC, int slot)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    if (type != 0)
        *out = type->m_arr100[slot];
    else
        *out = g_CFloatZero;
    return out;
}

CFloat* __fastcall UnitTypeSlotField108_6F29CFA0(CFloat* out, unsigned int fourCC, int slot)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    if (type != 0)
        *out = type->m_arr108[slot];
    else
        *out = g_CFloatZero;
    return out;
}

CFloat* __fastcall UnitTypeSlotField110_6F29CFF0(CFloat* out, unsigned int fourCC, int slot)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    if (type != 0)
        *out = type->m_arr110[slot];
    else
        *out = g_CFloatZero;
    return out;
}

CFloat* __fastcall UnitTypeSlotField120_6F29D090(CFloat* out, unsigned int fourCC, int slot)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    if (type != 0)
        *out = type->m_arr120[slot];
    else
        *out = g_CFloatZero;
    return out;
}

CFloat* __fastcall UnitTypeSlotField130_6F29D120(CFloat* out, unsigned int fourCC, int slot)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    if (type != 0)
        *out = type->m_arr130[slot];
    else
        *out = g_CFloatZero;
    return out;
}

CFloat* __fastcall UnitTypeSlotField138_6F29D170(CFloat* out, unsigned int fourCC, int slot)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    if (type != 0)
        *out = type->m_arr138[slot];
    else
        *out = g_CFloatZero;
    return out;
}

CFloat* __fastcall UnitTypeSlotField140_6F29D1C0(CFloat* out, unsigned int fourCC, int slot)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    if (type != 0)
        *out = type->m_arr140[slot];
    else
        *out = g_CFloatZero;
    return out;
}

CFloat* __fastcall UnitTypeSlotField148_6F29D210(CFloat* out, unsigned int fourCC, int slot)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    if (type != 0)
        *out = type->m_arr148[slot];
    else
        *out = g_CFloatZero;
    return out;
}

CFloat* __fastcall UnitTypeSlotField150_6F29D260(CFloat* out, unsigned int fourCC, int slot)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    if (type != 0)
        *out = type->m_arr150[slot];
    else
        *out = g_CFloatZero;
    return out;
}

CFloat* __fastcall UnitTypeSlotFieldD0_6F29CDA0(CFloat* out, unsigned int fourCC, int slot)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    if (type != 0)
        *out = type->m_arrD0[slot];
    else
        *out = g_CFloatZero;
    return out;
}

CFloat* __fastcall UnitTypeSlotFieldD8_6F29CDF0(CFloat* out, unsigned int fourCC, int slot)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    if (type != 0)
        *out = type->m_arrD8[slot];
    else
        *out = g_CFloatZero;
    return out;
}

CFloat* __fastcall UnitTypeSlotFieldE0_6F29CE40(CFloat* out, unsigned int fourCC, int slot)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    if (type != 0)
        *out = type->m_arrE0[slot];
    else
        *out = g_CFloatZero;
    return out;
}

//----------------------------------------------------------------------------
//  Same per-slot CFloat shape, miss = g_slopeThreshold1 (encoded 1.0)
//  instead of zero - the loop's own outlier.
//----------------------------------------------------------------------------
CFloat* __fastcall UnitTypeSlotField118_6F29D040(CFloat* out, unsigned int fourCC, int slot)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    if (type != 0)
        *out = type->m_arr118[slot];
    else
        *out = g_slopeThreshold1;
    return out;
}

//----------------------------------------------------------------------------
//  0x6F29BEA0 - ResolveConfigFloatById.  Same P2 CFloat-out-pointer shape as
//  above, but declared with Math/cunit_agent3_scaledroundedfloat.cpp's own
//  SConfigFloatFieldHolder parameter type (re-declared here to match it
//  exactly) so that TU's forward declaration binds onto this definition -
//  tools/link_check.py's first defect class.  The address is this table's
//  own module (0x6F29Bxxx-0x6F29Dxxx), not that caller's, so the body lives
//  here rather than there.
//----------------------------------------------------------------------------
struct SConfigFloatFieldHolder
{
    CFloat m_value;   // +0x00
};

void* __fastcall ResolveConfigFloatById(SConfigFloatFieldHolder* out, unsigned int fourCC)
{
    AGILE_TYPE_ID id(fourCC);
    SUnitTypeRecord* type = g_unitTypeTable.Ptr(id.Hash(), id);
    if (type != 0)
        out->m_value = type->m_field80;
    else
        out->m_value = g_CFloatZero;
    return out;
}
