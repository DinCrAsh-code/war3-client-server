//============================================================================
//  The unit-type registry probes defined in unittypequeries.cpp.  Declared
//  in a header, and included by that file itself, so that a caller in
//  another translation unit and the definition here cannot drift into two
//  mangled names - cause 1 of docs/notes/verifier-gate-link-failures.md.
//============================================================================
#ifndef UNITTYPEQUERIES_H
#define UNITTYPEQUERIES_H

#include "CFloat.h"

int     __fastcall UnitTypeFlag_6F29C2A0(unsigned int fourCC);        // 0x6F29C2A0
CFloat* __fastcall UnitTypeRadius_6F29C110(CFloat* out, unsigned int fourCC);
CFloat* __fastcall UnitTypeStateValue_6F29C710(CFloat* out, unsigned int fourCC);
CFloat* __fastcall UnitTypeValue_6F29CBE0(CFloat* out, unsigned int fourCC);

//  +0x210/+0x214 - CUnit slot 70's own closure (unit_abilityoverridefloat.cpp).
CFloat* __fastcall UnitTypeField210_6F29C830(CFloat* out, unsigned int fourCC);
CFloat* __fastcall UnitTypeField214_6F29C870(CFloat* out, unsigned int fourCC);

//  +0x5C - the food a unit type uses, +0x60 - the food it makes.
int     __fastcall UnitTypeField5C_6F29BAF0(unsigned int fourCC);     // 0x6F29BAF0
int     __fastcall UnitTypeField60_6F29BB20(unsigned int fourCC);     // 0x6F29BB20

//  +0x1D0 - the type's point value.  No dump heading of its own; see the
//  definition's comment.
int     __fastcall UnitTypePointValue_6F29C4D0(unsigned int fourCC);  // 0x6F29C4D0

//  +0x54 - one more of the same probe family, and the existence-only
//  probe (cunit_vtable_closure_worklist.json slot 107 closure batch 24).
int     __fastcall UnitTypeField54_6F29BA90(unsigned int fourCC);     // 0x6F29BA90
int     __fastcall UnitTypeIsRegistered_6F29D2F0(unsigned int fourCC); // 0x6F29D2F0


//  createunit-G1's batch - plain-int one-shot probes.
int     __fastcall UnitTypeField9C_6F29CB80(unsigned int fourCC);
int     __fastcall UnitTypeField170_6F29BBB0(unsigned int fourCC);
int     __fastcall UnitTypeField174_6F29BBE0(unsigned int fourCC);
int     __fastcall UnitTypeField178_6F29BC10(unsigned int fourCC);
int     __fastcall UnitTypeField17C_6F29BC40(unsigned int fourCC);
int     __fastcall UnitTypeField1C4_6F29C1E0(unsigned int fourCC);
int     __fastcall UnitTypeField1C8_6F29C210(unsigned int fourCC);
int     __fastcall UnitTypeField1E8_6F29C640(unsigned int fourCC);
int     __fastcall UnitTypeField1EC_6F29C670(unsigned int fourCC);
int     __fastcall UnitTypeField1F4_6F29C6E0(unsigned int fourCC);
int     __fastcall UnitTypeField224_6F29CA30(unsigned int fourCC);
int     __fastcall UnitTypeField22C_6F29CA90(unsigned int fourCC);
int     __fastcall UnitTypeField270_6F29CAF0(unsigned int fourCC);
int     __fastcall UnitTypeField98_6F29BFE0(unsigned int fourCC);

//  CFloat-out-pointer one-shot probes.
CFloat* __fastcall UnitTypeField194_6F29C090(CFloat* out, unsigned int fourCC);
CFloat* __fastcall UnitTypeField198_6F29C0D0(CFloat* out, unsigned int fourCC);
CFloat* __fastcall UnitTypeField90_6F29BF60(CFloat* out, unsigned int fourCC);
CFloat* __fastcall UnitTypeField1FC_6F29C750(CFloat* out, unsigned int fourCC);
CFloat* __fastcall UnitTypeField180_6F29BC70(CFloat* out, unsigned int fourCC);
CFloat* __fastcall UnitTypeField188_6F29BCF0(CFloat* out, unsigned int fourCC);
CFloat* __fastcall UnitTypeField184_6F29BCB0(CFloat* out, unsigned int fourCC);
CFloat* __fastcall UnitTypeField18C_6F29C010(CFloat* out, unsigned int fourCC);
CFloat* __fastcall UnitTypeField88_6F29BEE0(CFloat* out, unsigned int fourCC);
CFloat* __fastcall UnitTypeField1D4_6F29C500(CFloat* out, unsigned int fourCC);
CFloat* __fastcall UnitTypeField1F0_6F29C6A0(CFloat* out, unsigned int fourCC);
CFloat* __fastcall UnitTypeField84_6F29BE60(CFloat* out, unsigned int fourCC);
CFloat* __fastcall UnitTypeField1E0_6F29C5C0(CFloat* out, unsigned int fourCC);
CFloat* __fastcall UnitTypeField1E4_6F29C600(CFloat* out, unsigned int fourCC);

//  Real hardware-float probes (fld/fldz).
float   __fastcall UnitTypeFloatField220_6F29C910(unsigned int fourCC);
float   __fastcall UnitTypeFloatField21C_6F29C8E0(unsigned int fourCC);
float   __fastcall UnitTypeFloatField218_6F29C8B0(unsigned int fourCC);

//  Embedded-SHandleHolder probes.
unsigned int __fastcall UnitTypeHandle280_6F29CB50(unsigned int fourCC);
unsigned int __fastcall UnitTypeHandle274_6F29CB20(unsigned int fourCC);

//  Three-float-vector probe.
unsigned int* __fastcall UnitTypeVec200_6F29C790(unsigned int* out, unsigned int fourCC);

//  Per-slot (two-element array) plain-int probes.
int     __fastcall UnitTypeSlotFieldA0_6F29CC20(unsigned int fourCC, int slot);
int     __fastcall UnitTypeSlotFieldB0_6F29CCA0(unsigned int fourCC, int slot);
int     __fastcall UnitTypeSlotFieldC8_6F29CD60(unsigned int fourCC, int slot);
int     __fastcall UnitTypeSlotFieldE8_6F29CE90(unsigned int fourCC, int slot);
int     __fastcall UnitTypeSlotFieldF8_6F29CF10(unsigned int fourCC, int slot);
int     __fastcall UnitTypeSlotField128_6F29D0E0(unsigned int fourCC, int slot);
int     __fastcall UnitTypeSlotFieldC0_6F29CD20(unsigned int fourCC, int slot);
int     __fastcall UnitTypeSlotFieldF0_6F29CED0(unsigned int fourCC, int slot);
int     __fastcall UnitTypeSlotFieldB8_6F29CCE0(unsigned int fourCC, int slot);
int     __fastcall UnitTypeSlotField168_6F29D2B0(unsigned int fourCC, int slot);

//  Per-slot CFloat-out-pointer probes.
CFloat* __fastcall UnitTypeSlotField100_6F29CF50(CFloat* out, unsigned int fourCC, int slot);
CFloat* __fastcall UnitTypeSlotField108_6F29CFA0(CFloat* out, unsigned int fourCC, int slot);
CFloat* __fastcall UnitTypeSlotField110_6F29CFF0(CFloat* out, unsigned int fourCC, int slot);
CFloat* __fastcall UnitTypeSlotField120_6F29D090(CFloat* out, unsigned int fourCC, int slot);
CFloat* __fastcall UnitTypeSlotField130_6F29D120(CFloat* out, unsigned int fourCC, int slot);
CFloat* __fastcall UnitTypeSlotField138_6F29D170(CFloat* out, unsigned int fourCC, int slot);
CFloat* __fastcall UnitTypeSlotField140_6F29D1C0(CFloat* out, unsigned int fourCC, int slot);
CFloat* __fastcall UnitTypeSlotField148_6F29D210(CFloat* out, unsigned int fourCC, int slot);
CFloat* __fastcall UnitTypeSlotField150_6F29D260(CFloat* out, unsigned int fourCC, int slot);
CFloat* __fastcall UnitTypeSlotFieldD0_6F29CDA0(CFloat* out, unsigned int fourCC, int slot);
CFloat* __fastcall UnitTypeSlotFieldD8_6F29CDF0(CFloat* out, unsigned int fourCC, int slot);
CFloat* __fastcall UnitTypeSlotFieldE0_6F29CE40(CFloat* out, unsigned int fourCC, int slot);
CFloat* __fastcall UnitTypeSlotField118_6F29D040(CFloat* out, unsigned int fourCC, int slot);

//  0x6F29BEA0 - see the definition's own comment.
struct SConfigFloatFieldHolder;
void*   __fastcall ResolveConfigFloatById(SConfigFloatFieldHolder* out, unsigned int fourCC);

#endif
