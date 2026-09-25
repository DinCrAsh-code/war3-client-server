//============================================================================
//  Eleven one-line thiscall(this, value) forwarders through ISetterTarget's
//  two setter slots, each baking in a different id constant. See
//  settertarget.h for the interface-level notes, including the
//  __fastcall-vs-__thiscall bug this file's own previous version carried.
//
//  Two id/slot combinations repeat at a second address each (id 8 through
//  SetFieldA at 0x6F6F2D90 and 0x6F6F0D90; id 9 through SetFieldA at
//  0x6F6F1EB0 and 0x6F6F0DB0) - the same "same shape, two addresses, no
//  COMDAT folding" situation the five TSGrowableArray<T> destructors and
//  the two TRefCnt::Release forwarders are already in elsewhere in this
//  codebase. Named by id plus an address suffix where the id repeats.
//============================================================================
#include "settertarget.h"

//----------------------------------------------------------------------------
//  0x6F6F2CF0 - SetFieldB, id 0.
//----------------------------------------------------------------------------
void ISetterTarget::SetFieldB_Id0(int value)
{
    SetFieldB(value, 0);
}

//----------------------------------------------------------------------------
//  0x6F6F2D30 - SetFieldA, id 2.
//----------------------------------------------------------------------------
void ISetterTarget::SetFieldA_Id2(int value)
{
    SetFieldA(value, 2);
}

//----------------------------------------------------------------------------
//  0x6F6F2D50 - SetFieldB, id 3.
//----------------------------------------------------------------------------
void ISetterTarget::SetFieldB_Id3(int value)
{
    SetFieldB(value, 3);
}

//----------------------------------------------------------------------------
//  0x6F6F2D70 - SetFieldA, id 4.
//----------------------------------------------------------------------------
void ISetterTarget::SetFieldA_Id4(int value)
{
    SetFieldA(value, 4);
}

//----------------------------------------------------------------------------
//  0x6F6F2D90 - SetFieldA, id 8 (first of two addresses; see 0x6F6F0D90).
//----------------------------------------------------------------------------
void ISetterTarget::SetFieldA_Id8_6F6F2D90(int value)
{
    SetFieldA(value, 8);
}

//----------------------------------------------------------------------------
//  0x6F6F0D90 - SetFieldA, id 8 (second of two addresses; see 0x6F6F2D90).
//----------------------------------------------------------------------------
void ISetterTarget::SetFieldA_Id8_6F6F0D90(int value)
{
    SetFieldA(value, 8);
}

//----------------------------------------------------------------------------
//  0x6F6F2E30 - SetFieldA, id 0x30.
//----------------------------------------------------------------------------
void ISetterTarget::SetFieldA_Id0x30(int value)
{
    SetFieldA(value, 0x30);
}

//----------------------------------------------------------------------------
//  0x6F6F1D70 - SetFieldA, id 0xA.
//----------------------------------------------------------------------------
void ISetterTarget::SetFieldA_Id0xA(int value)
{
    SetFieldA(value, 0xA);
}

//----------------------------------------------------------------------------
//  0x6F6F1E90 - SetFieldA, id 7.
//----------------------------------------------------------------------------
void ISetterTarget::SetFieldA_Id7(int value)
{
    SetFieldA(value, 7);
}

//----------------------------------------------------------------------------
//  0x6F6F1EB0 - SetFieldA, id 9 (first of two addresses; see 0x6F6F0DB0).
//----------------------------------------------------------------------------
void ISetterTarget::SetFieldA_Id9_6F6F1EB0(int value)
{
    SetFieldA(value, 9);
}

//----------------------------------------------------------------------------
//  0x6F6F0DB0 - SetFieldA, id 9 (second of two addresses; see 0x6F6F1EB0).
//----------------------------------------------------------------------------
void ISetterTarget::SetFieldA_Id9_6F6F0DB0(int value)
{
    SetFieldA(value, 9);
}
