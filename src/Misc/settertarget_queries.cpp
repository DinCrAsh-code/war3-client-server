//============================================================================
//  A second family of ISetterTarget callers, in the same address
//  neighbourhood as settertarget_forwards.cpp's eleven void forwarders
//  (0x6F6F2D10-0x6F6F2E90, plus one outlier at 0x6F6FA050) but reaching the
//  slots differently: keeping SetFieldA's return value instead of
//  discarding it, computing the id argument instead of baking it in, and
//  one address that reaches the still-unnamed vtable+0x14 slot for the
//  first time in this repo.  See settertarget.h for the interface-level
//  notes on why SetFieldA is declared `int`-returning for this reason.
//============================================================================
#include "settertarget.h"

//----------------------------------------------------------------------------
//  0x6F6F2D10 - SetFieldRange, mode fixed to 1, the other two arguments
//  passed straight through from this forwarder's own caller.
//----------------------------------------------------------------------------
void ISetterTarget::SetFieldRange_Mode1(int value, int a, int b)
{
    SetFieldRange(value, 1, a, b);
}

//----------------------------------------------------------------------------
//  0x6F6F0D70 - SetFieldRange, mode fixed to 7 - the same shape as
//  SetFieldRange_Mode1 above, a second address at this slot with a
//  different baked-in mode.
//----------------------------------------------------------------------------
void ISetterTarget::SetFieldRange_Mode7(int value, int a, int b)
{
    SetFieldRange(value, 7, a, b);
}

//----------------------------------------------------------------------------
//  0x6F6F2DB0 - SetFieldA, id 0x12 (18), return value kept.
//----------------------------------------------------------------------------
int ISetterTarget::QueryFieldA_Id18(int value)
{
    return SetFieldA(value, 0x12) != 0;
}

//----------------------------------------------------------------------------
//  0x6F6F2DD0 - SetFieldA, id 0x13 (19), return value kept.
//----------------------------------------------------------------------------
int ISetterTarget::QueryFieldA_Id19(int value)
{
    return SetFieldA(value, 0x13) != 0;
}

//----------------------------------------------------------------------------
//  0x6F6F2DF0 - SetFieldA, id 6, return value kept.
//----------------------------------------------------------------------------
int ISetterTarget::QueryFieldA_Id6(int value)
{
    return SetFieldA(value, 6) != 0;
}

//----------------------------------------------------------------------------
//  0x6F6F2E10 - SetFieldA, id 7, return value kept.  A second address for
//  id 7 through this same slot already exists as the void-returning
//  SetFieldA_Id7 (0x6F6F1E90, settertarget_forwards.cpp) - same id, same
//  slot, different caller shape, not a duplicate.
//----------------------------------------------------------------------------
int ISetterTarget::QueryFieldA_Id7(int value)
{
    return SetFieldA(value, 7) != 0;
}

//----------------------------------------------------------------------------
//  0x6F6F2E50 - SetFieldA, id 0x14 (20), return value kept.
//----------------------------------------------------------------------------
int ISetterTarget::QueryFieldA_Id20(int value)
{
    return SetFieldA(value, 0x14) != 0;
}

//----------------------------------------------------------------------------
//  0x6F6F2E90 - a three-way selector (0/1/2) mapped to a fixed id
//  (0x17/0x19/0x1B = 23/25/27) before calling through the same SetFieldA
//  slot everything else in this file uses; any other selector value
//  returns 0 without making the call.  The dump's own shape is a computed
//  jmp into SetFieldA's own vtable slot with this function's stack frame
//  repurposed to hold the chosen id - a true tail call, which is what
//  `return SetFieldA(...)` compiles to here.
//----------------------------------------------------------------------------
int ISetterTarget::QueryFieldA_BySelector(int value, int selector)
{
    switch (selector)
    {
    case 0:  return SetFieldA(value, 0x17);
    case 1:  return SetFieldA(value, 0x19);
    case 2:  return SetFieldA(value, 0x1B);
    default: return 0;
    }
}

//----------------------------------------------------------------------------
//  0x6F6FA050 - SetFieldA, id 5, then the 0-4 result remapped through a
//  fixed table (0->2, 1->3, 2->4, 3->5, 4->1, anything else->0) rather
//  than returned as-is - a different shape from every other caller in
//  this file, which either discard the result or pass it straight
//  through.  What the remap means is not established from this call tree.
//----------------------------------------------------------------------------
int ISetterTarget::QueryFieldA_Id5Remapped(int value)
{
    int result = SetFieldA(value, 5);
    switch (result)
    {
    case 0:  return 2;
    case 1:  return 3;
    case 2:  return 4;
    case 3:  return 5;
    case 4:  return 1;
    default: return 0;
    }
}
