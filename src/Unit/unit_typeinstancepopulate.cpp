//============================================================================
//  0x6F29F3E0 - populate a per-slot unit-type instance record from the
//  type's own registry row.  `ecx` = a four-character unit-type id
//  ("fourCC"), `edx` = a slot index, one stack argument = the destination
//  record; `retn 4`.  Returns 0 immediately if the id is not a registered
//  member of the table `sub_6F29D2F0` checks (a second, smaller registry
//  than `g_unitTypeTable` - `dword_6FAB445C`, not otherwise identified),
//  otherwise copies roughly forty fields out of one-shot accessors plus a
//  further twenty-three fields out of a two-iteration loop (`edx` = slot
//  0 then 1) of per-slot accessors, and returns 1.
//
//  None of the ~40 individual field accessors below are reconstructed here:
//  each is a one- or two-register-argument leaf with no stack cleanup
//  (every call site pushes nothing beyond the loop's own index, so `retn 0`
//  - or `retn 4` for the five-argument shape carrying the slot index on the
//  stack too - is not a guess, it is what the caller's own zero extra
//  pushes already require for the real game to run at all). Three are
//  already reconstructed elsewhere in this exact module
//  (unittypequeries.cpp's `UnitTypeField210_6F29C830`,
//  `UnitTypeIntField1A8_6F29C240`, `UnitTypeStateValue_6F29C710`) and are
//  called for real rather than re-thunked; `sub_6F6EEE20` is
//  `CFloat::operator*` (softfloat.cpp) and `sub_6F4C5CF0` is
//  `STStringField::Assign` (rcstringassigncstr.cpp), both already
//  reconstructed and called for real too.
//
//  Four call shapes recur throughout, matched to the family
//  unittypequeries.cpp already establishes:
//    P1 - `int/void* __fastcall Fn(fourCC)`            (one ecx arg)
//    P2 - `T* __fastcall Fn(T* out, fourCC)`            (ecx=out, edx=fourCC;
//                                                         caller derefs eax)
//    P3 - `FVec3* __fastcall Fn(FVec3* out, fourCC)`    (three-dword out)
//    P4 - `int/void* __fastcall Fn(fourCC, slotIndex)`  (ecx=fourCC, edx=idx)
//    P5 - `T* __fastcall Fn(T* out, fourCC, slotIndex)` (ecx=out, edx=fourCC,
//                                                         one stack arg=idx,
//                                                         `retn 4`)
//  plus two FPU-return variants (`fstp` instead of an `eax`/out-pointer)
//  for the two `float`-typed fields, matching
//  UnitTypeFloatField1A0_6F29C150's own shape.
//============================================================================
#include "CFloat.h"
#include "jassnatives.h"     // STStringField
#include "unittypequeries.h" // createunit-G1's own batch, see that file's header

extern CFloat __fastcall operator*(const CFloat&, const CFloat&);
extern const CFloat g_CFloatDegToRad;   // dword_6FAAE5E8

//  0x6F29C830 / 0x6F29C240 / 0x6F29C710 - already reconstructed
//  (unittypequeries.cpp).  Re-declared to bind onto those bodies rather
//  than invent second symbols.
extern CFloat* __fastcall UnitTypeField210_6F29C830(CFloat* out, unsigned int fourCC);
extern int     __fastcall UnitTypeIntField1A8_6F29C240(unsigned int fourCC);
extern CFloat* __fastcall UnitTypeStateValue_6F29C710(CFloat* out, unsigned int fourCC);
extern int     __fastcall UnitTypeIsRegistered_6F29D2F0(unsigned int fourCC);

//  The destination record.  Only the offsets this function itself writes
//  are named; everything else is a reserved gap.  The 23 fields written
//  inside the two-iteration loop are two-element arrays at the loop's own
//  stride (4 bytes/element, the loop index selects which).
struct SUnitTypeInstanceRecord
{
    char         m_reserved00[0x10];
    float        m_float10;          // +0x10
    unsigned int m_field14;          // +0x14
    char         m_reserved18[0x1C - 0x18];
    float        m_float1C;          // +0x1C
    char         m_reserved20[0x28 - 0x20];
    unsigned int m_slotIndex28;      // +0x28 - the function's own `edx` arg
    char         m_reserved2C[0x38 - 0x2C];   // STStringField at +0x2C
    unsigned int m_field38;          // +0x38
    unsigned int m_field3C;          // +0x3C - same value as +0x38
    unsigned int m_field40;          // +0x40
    unsigned int m_field44;          // +0x44 - a CFloat, *DegToRad already applied
    unsigned int m_field48;          // +0x48
    unsigned int m_field4C;          // +0x4C
    unsigned int m_field50;          // +0x50
    unsigned int m_field54;          // +0x54
    unsigned int m_slotIndex58;      // +0x58 - same `edx` arg again
    unsigned int m_field5C;          // +0x5C
    unsigned int m_field60;          // +0x60
    unsigned int m_field64;          // +0x64
    unsigned int m_field68;          // +0x68
    unsigned int m_field6C;          // +0x6C
    unsigned int m_field70;          // +0x70
    unsigned int m_field74;          // +0x74
    unsigned int m_field78;          // +0x78
    unsigned int m_vec7C[3];         // +0x7C/+0x80/+0x84
    unsigned int m_field88;          // +0x88
    unsigned int m_field8C;          // +0x8C
    unsigned int m_field90;          // +0x90
    unsigned int m_field94;          // +0x94
    unsigned int m_arr98[2];         // +0x98/+0x9C
    unsigned int m_arrA0[2];         // +0xA0/+0xA4
    unsigned int m_arrA8[2];         // +0xA8/+0xAC
    unsigned int m_arrB0[2];         // +0xB0/+0xB4
    unsigned int m_arrB8[2];         // +0xB8/+0xBC
    unsigned int m_arrC0[2];         // +0xC0/+0xC4
    unsigned int m_arrC8[2];         // +0xC8/+0xCC
    unsigned int m_arrD0[2];         // +0xD0/+0xD4
    unsigned int m_arrD8[2];         // +0xD8/+0xDC
    unsigned int m_arrE0[2];         // +0xE0/+0xE4
    unsigned int m_arrE8[2];         // +0xE8/+0xEC
    unsigned int m_arrF0[2];         // +0xF0/+0xF4
    unsigned int m_arrF8[2];         // +0xF8/+0xFC
    unsigned int m_arr100[2];        // +0x100/+0x104
    unsigned int m_arr108[2];        // +0x108/+0x10C
    unsigned int m_arr110[2];        // +0x110/+0x114
    unsigned int m_arr118[2];        // +0x118/+0x11C
    unsigned int m_arr120[2];        // +0x120/+0x124
    unsigned int m_arr128[2];        // +0x128/+0x12C
    unsigned int m_arr130[2];        // +0x130/+0x134
    unsigned int m_arr138[2];        // +0x138/+0x13C
    unsigned int m_arr140[2];        // +0x140/+0x144
    unsigned int m_arr148[2];        // +0x148/+0x14C
    unsigned int m_field150;         // +0x150
    unsigned int m_field154;         // +0x154
    unsigned int m_field158;         // +0x158
    unsigned int m_field15C;         // +0x15C
    unsigned int m_field160;         // +0x160
    unsigned int m_field164;         // +0x164
    unsigned int m_field168;         // +0x168
    unsigned int m_field16C;         // +0x16C
    unsigned int m_field170;         // +0x170
    unsigned int m_field174;         // +0x174
    unsigned int m_field178;         // +0x178
    unsigned int m_field17C;         // +0x17C
    unsigned int m_field180;         // +0x180
    float        m_float184;         // +0x184
    float        m_float188;         // +0x188
    unsigned int m_field18C;         // +0x18C
    unsigned int m_field190;         // +0x190
    unsigned int m_field194;         // +0x194
    unsigned int m_field198;         // +0x198
    unsigned int m_field19C;         // +0x19C
};

//  Re-declared to match Misc/misc_field_getters.cpp exactly, so the call
//  binds to that definition rather than inventing a second symbol
//  (tools/link_check.py's first defect class).
struct IndexedArrayHolder_6F3A1650
{
    char pad[0x58];
    int  m_data[1];
    int  At(unsigned int index);
};

//  dword_6FAB65F4 - the world-object singleton (Unit/unit_publishposition.cpp
//  and friends).  Declared, never defined: the real game's global.
extern void* g_unk6FAB65F4;

int __fastcall PopulateUnitTypeInstance(unsigned int fourCC, int slotIndex,
                                         SUnitTypeInstanceRecord* out);

//----------------------------------------------------------------------------
//  ~40 one-shot field accessors (P1/P2/P3) plus one registry membership
//  test - not reconstructed, see the file header.
//----------------------------------------------------------------------------
#define UTQ_THUNK(addr)          \
    {                            \
        __asm { mov eax, addr }  \
        __asm { jmp eax }        \
    }

//  Everything below is out of createunit-G1's own address list (a different
//  module - 0x6F29DE70/0x6F32CFD0/0x6F32D390/0x6F32D3C0/0x6F29C2D0/0x6F29CA60/
//  0x6F29CAC0 - and still genuinely unreconstructed), so it stays a naked
//  redirect thunk.  Every other UTQ_ address this file used to declare here
//  is now a real accessor in Unit/unittypequeries.cpp (createunit-G1's own
//  batch) or Math/cunit_agent3_scaledroundedfloat.cpp, declared in
//  unittypequeries.h and called by name below instead.
extern "C"
{
void*        __fastcall UTQ_6F29DE70(void* out, void* slotRecord) UTQ_THUNK(0x6F29DE70);
void*        __fastcall UTQ_6F29C2D0(unsigned int fourCC) UTQ_THUNK(0x6F29C2D0);
void*        __fastcall UTQ_6F29CA60(unsigned int fourCC) UTQ_THUNK(0x6F29CA60);
void*        __fastcall UTQ_6F29CAC0(unsigned int fourCC) UTQ_THUNK(0x6F29CAC0);
const char*  __fastcall UTQ_6F32CFD0(unsigned int fourCC) UTQ_THUNK(0x6F32CFD0);
float        __fastcall UTQ_6F32D3C0(unsigned int fourCC) UTQ_THUNK(0x6F32D3C0);
void*        __fastcall UTQ_6F32D390(void* out, unsigned int fourCC) UTQ_THUNK(0x6F32D390);
}

int __fastcall PopulateUnitTypeInstance(unsigned int fourCC, int slotIndex,
                                         SUnitTypeInstanceRecord* out)
{
    void* slotRecord = (void*)((IndexedArrayHolder_6F3A1650*)g_unk6FAB65F4)->At(slotIndex);

    if (!UnitTypeIsRegistered_6F29D2F0(fourCC))
        return 0;

    unsigned int scratch;
    unsigned int scratchVec[3];

    void* p = UTQ_6F29DE70(&scratch, slotRecord);
    out->m_field3C = *(unsigned int*)p;
    out->m_field38 = *(unsigned int*)p;

    p = (void*)UnitTypeField1E0_6F29C5C0((CFloat*)&scratch, fourCC);
    out->m_field40 = *(unsigned int*)p;

    CFloat* angle = UnitTypeField1E4_6F29C600((CFloat*)&scratch, fourCC);
    CFloat rad = operator*(*angle, g_CFloatDegToRad);
    out->m_field44 = *(unsigned int*)&rad;

    out->m_field48 = (unsigned int)UnitTypeField1E8_6F29C640(fourCC);
    out->m_field4C = (unsigned int)UnitTypeField1EC_6F29C670(fourCC);

    p = (void*)UnitTypeField1F0_6F29C6A0((CFloat*)&scratch, fourCC);
    out->m_field50 = *(unsigned int*)p;

    out->m_field54 = (unsigned int)UnitTypeField1F4_6F29C6E0(fourCC);
    out->m_slotIndex58 = (unsigned int)slotIndex;

    out->m_field19C = (unsigned int)UnitTypeField1C4_6F29C1E0(fourCC);
    out->m_field5C = (unsigned int)UnitTypeField1C8_6F29C210(fourCC);

    p = (void*)UnitTypeField1D4_6F29C500((CFloat*)&scratch, fourCC);
    out->m_field68 = *(unsigned int*)p;

    out->m_field78 = (unsigned int)UTQ_6F29C2D0(fourCC);

    p = (void*)UnitTypeField88_6F29BEE0((CFloat*)&scratch, fourCC);
    out->m_field60 = *(unsigned int*)p;

    p = (void*)UnitTypeField84_6F29BE60((CFloat*)&scratch, fourCC);
    out->m_field64 = *(unsigned int*)p;

    p = (void*)UnitTypeField194_6F29C090((CFloat*)&scratch, fourCC);
    out->m_field8C = *(unsigned int*)p;

    p = (void*)UnitTypeField198_6F29C0D0((CFloat*)&scratch, fourCC);
    out->m_field90 = *(unsigned int*)p;

    unsigned int* vec = UnitTypeVec200_6F29C790((unsigned int*)&scratchVec, fourCC);
    out->m_vec7C[0] = vec[0];
    out->m_vec7C[1] = vec[1];
    out->m_vec7C[2] = vec[2];

    CFloat field210Buf;
    UnitTypeField210_6F29C830(&field210Buf, fourCC);
    out->m_field88 = *(unsigned int*)&field210Buf;

    p = (void*)UnitTypeField90_6F29BF60((CFloat*)&scratch, fourCC);
    out->m_field150 = *(unsigned int*)p;

    out->m_field154 = (unsigned int)UnitTypeField98_6F29BFE0(fourCC);
    out->m_field6C = (unsigned int)UnitTypeIntField1A8_6F29C240(fourCC);

    CFloat stateValueBuf;
    UnitTypeStateValue_6F29C710(&stateValueBuf, fourCC);
    out->m_field70 = *(unsigned int*)&stateValueBuf;

    p = (void*)UnitTypeField1FC_6F29C750((CFloat*)&scratch, fourCC);
    out->m_field74 = *(unsigned int*)p;

    out->m_field158 = (unsigned int)UnitTypeField170_6F29BBB0(fourCC);
    out->m_field15C = (unsigned int)UnitTypeField178_6F29BC10(fourCC);
    out->m_field160 = (unsigned int)UnitTypeField174_6F29BBE0(fourCC);

    p = (void*)UnitTypeField180_6F29BC70((CFloat*)&scratch, fourCC);
    out->m_field168 = *(unsigned int*)p;

    p = (void*)UnitTypeField188_6F29BCF0((CFloat*)&scratch, fourCC);
    out->m_field16C = *(unsigned int*)p;

    p = (void*)UnitTypeField184_6F29BCB0((CFloat*)&scratch, fourCC);
    out->m_field170 = *(unsigned int*)p;

    out->m_field164 = (unsigned int)UnitTypeField17C_6F29BC40(fourCC);

    p = (void*)UnitTypeField18C_6F29C010((CFloat*)&scratch, fourCC);
    out->m_field180 = *(unsigned int*)p;

    out->m_float184 = UnitTypeFloatField21C_6F29C8E0(fourCC);
    out->m_float188 = UnitTypeFloatField220_6F29C910(fourCC);
    out->m_field18C = (unsigned int)UnitTypeField224_6F29CA30(fourCC);
    out->m_field190 = (unsigned int)UTQ_6F29CA60(fourCC);
    out->m_field194 = (unsigned int)UnitTypeField22C_6F29CA90(fourCC);
    out->m_field198 = (unsigned int)UTQ_6F29CAC0(fourCC);

    ((STStringField*)((char*)out + 0x2C))->Assign(UTQ_6F32CFD0(fourCC));

    out->m_float10 = UTQ_6F32D3C0(fourCC);

    p = UTQ_6F32D390(&scratch, fourCC);
    out->m_field14 = *(unsigned int*)p;

    out->m_float1C = UnitTypeFloatField218_6F29C8B0(fourCC);
    out->m_slotIndex28 = (unsigned int)slotIndex;
    out->m_field94 = (unsigned int)UnitTypeField9C_6F29CB80(fourCC);

    for (unsigned int i = 0; i < 2; i++)
    {
        out->m_arr98[i] = (unsigned int)UnitTypeSlotFieldA0_6F29CC20(fourCC, i);

        p = (void*)UnitTypeSlotField100_6F29CF50((CFloat*)&scratch, fourCC, i);
        out->m_arrA0[i] = *(unsigned int*)p;

        p = (void*)UnitTypeSlotField108_6F29CFA0((CFloat*)&scratch, fourCC, i);
        out->m_arrA8[i] = *(unsigned int*)p;

        p = (void*)UnitTypeSlotField110_6F29CFF0((CFloat*)&scratch, fourCC, i);
        out->m_arr100[i] = *(unsigned int*)p;

        p = (void*)UnitTypeSlotField118_6F29D040((CFloat*)&scratch, fourCC, i);
        out->m_arr108[i] = *(unsigned int*)p;

        p = (void*)UnitTypeSlotField120_6F29D090((CFloat*)&scratch, fourCC, i);
        out->m_arr110[i] = *(unsigned int*)p;

        out->m_arr118[i] = (unsigned int)UnitTypeSlotFieldB0_6F29CCA0(fourCC, i);
        out->m_arr120[i] = (unsigned int)UnitTypeSlotFieldB8_6F29CCE0(fourCC, i);
        out->m_arr128[i] = (unsigned int)UnitTypeSlotFieldC0_6F29CD20(fourCC, i);
        out->m_arr140[i] = (unsigned int)UnitTypeSlotFieldE8_6F29CE90(fourCC, i);
        out->m_arr130[i] = (unsigned int)UnitTypeSlotFieldF8_6F29CF10(fourCC, i);
        out->m_arr138[i] = (unsigned int)UnitTypeSlotFieldF0_6F29CED0(fourCC, i);
        out->m_arrB0[i]  = (unsigned int)UnitTypeSlotField128_6F29D0E0(fourCC, i);

        p = (void*)UnitTypeSlotField130_6F29D120((CFloat*)&scratch, fourCC, i);
        out->m_arrB8[i] = *(unsigned int*)p;

        p = (void*)UnitTypeSlotField138_6F29D170((CFloat*)&scratch, fourCC, i);
        out->m_arrC0[i] = *(unsigned int*)p;

        p = (void*)UnitTypeSlotField140_6F29D1C0((CFloat*)&scratch, fourCC, i);
        out->m_arrC8[i] = *(unsigned int*)p;

        p = (void*)UnitTypeSlotField148_6F29D210((CFloat*)&scratch, fourCC, i);
        out->m_arrD0[i] = *(unsigned int*)p;

        p = (void*)UnitTypeSlotField150_6F29D260((CFloat*)&scratch, fourCC, i);
        out->m_arrD8[i] = *(unsigned int*)p;

        out->m_arrE0[i] = (unsigned int)UnitTypeSlotFieldC8_6F29CD60(fourCC, i);

        p = (void*)UnitTypeSlotFieldD0_6F29CDA0((CFloat*)&scratch, fourCC, i);
        out->m_arrE8[i] = *(unsigned int*)p;

        p = (void*)UnitTypeSlotFieldD8_6F29CDF0((CFloat*)&scratch, fourCC, i);
        out->m_arrF0[i] = *(unsigned int*)p;

        p = (void*)UnitTypeSlotFieldE0_6F29CE40((CFloat*)&scratch, fourCC, i);
        out->m_arrF8[i] = *(unsigned int*)p;

        out->m_arr148[i] = (unsigned int)UnitTypeSlotField168_6F29D2B0(fourCC, i);
    }

    out->m_field174 = (unsigned int)UnitTypeField270_6F29CAF0(fourCC);
    out->m_field178 = UnitTypeHandle274_6F29CB20(fourCC);
    out->m_field17C = UnitTypeHandle280_6F29CB50(fourCC);

    return 1;
}
