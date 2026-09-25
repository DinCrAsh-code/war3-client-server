//============================================================================
//  CUnit's vtable slots 78, 88, 89, 90 and 112 - the 0x6F29Fxxx module.
//  See unit.h.
//
//  All five answer a question about the unit's *type* rather than about
//  the unit: every one of them starts by reading CWidget::m_footprintType
//  (+0x30, the bare FourCC) and hands it to a type-table lookup.  Two
//  shapes: slots 88 and 89 tail-jump with the FourCC in ecx (a __fastcall
//  taking it as its one register argument), slots 90 and 112 put it in edx
//  and pass the caller's own CFloat buffer in ecx.
//
//  Slot 88's target has no `### ... @ 0x...` heading of its own - IDA has
//  0x6F29C270 attached to this slot as a tail chunk rather than defined as
//  a function - so the dump this is scored against contains the callee's
//  eleven instructions as well as this one's two.  The two are what the
//  shipped function is; see docs/targets/CUnit__vtable.md.
//============================================================================
#include "unit.h"

//  0x6F2834E0 (`retn 0`) - the unit's own player-slot lookup, declared in
//  unit_visibility.cpp's terms.
void* __fastcall UnitVisibilityMaskOwner_6F2834E0(void* self);

//  0x6F29DE70 (`retn 4`) - out buffer in ecx, player slot in edx, FourCC
//  pushed.
void __fastcall ComputeUnitTypeValue_6F29DE70(CFloat* out, void* playerSlot,
                                              unsigned int fourCC);
//  0x6F29C270 (`retn 0`) / 0x6F29C2A0 (`retn 0`) - two type-table probes
//  that take the FourCC as their only register argument.
int __fastcall UnitTypeFlag_6F29C270(unsigned int fourCC);
int __fastcall UnitTypeFlag_6F29C2A0(unsigned int fourCC);
//  0x6F29C110 (`retn 0`) / 0x6F29CBE0 (`retn 0`) - out buffer in ecx,
//  FourCC in edx.
CFloat* __fastcall UnitTypeRadius_6F29C110(CFloat* out, unsigned int fourCC);
CFloat* __fastcall UnitTypeValue_6F29CBE0(CFloat* out, unsigned int fourCC);

//----------------------------------------------------------------------------
//  0x6F29F320 - slot 78 (+0x138).
//
//  The FourCC is pushed *before* the player-slot lookup runs, which is the
//  compiler scheduling an argument it already has across an intervening
//  call, not two separate stack operations.
//----------------------------------------------------------------------------
CFloat* CUnit::Method_0x138(CFloat* out)
{
    ComputeUnitTypeValue_6F29DE70(out, UnitVisibilityMaskOwner_6F2834E0(this),
                                  m_footprintType);
    return out;
}

//----------------------------------------------------------------------------
//  0x6F29F380 / 0x6F29F390 - slots 88 and 89 (+0x160/+0x164).
//----------------------------------------------------------------------------
int CUnit::Method_0x160()
{
    return UnitTypeFlag_6F29C270(m_footprintType);
}

int CUnit::Method_0x164()
{
    return UnitTypeFlag_6F29C2A0(m_footprintType);
}

//----------------------------------------------------------------------------
//  0x6F29F3A0 / 0x6F29F3C0 - slots 90 (GetPlacementRadius) and 112.  The
//  same function twice against two different type-table columns.
//----------------------------------------------------------------------------
CFloat* CUnit::GetPlacementRadius(CFloat* out)
{
    UnitTypeRadius_6F29C110(out, m_footprintType);
    return out;
}

CFloat* CUnit::Method_0x1C0(CFloat* out)
{
    UnitTypeValue_6F29CBE0(out, m_footprintType);
    return out;
}
