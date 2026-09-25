//============================================================================
//  0x6F29B700 - CUnit's vtable slot 34 (+0x88), SetModelByHandle.  See
//  unit.h.
//
//  Do what CWar3Image does (0x6F26B9E0, widget_setmodelhandle.cpp - a
//  direct, non-virtual base call, `call sub_6F26B9E0` with `this`
//  unchanged), then tell the unit's own model bookkeeping about it with
//  the "no particular index" sentinel -1.
//============================================================================
#include "unit.h"

void CUnit::SetModelByHandle(unsigned int modelHandle, int notify)
{
    CWar3Image::SetModelByHandle(modelHandle, notify);
    NotifyModelChanged(-1);
}
