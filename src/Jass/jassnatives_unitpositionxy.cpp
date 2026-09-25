//============================================================================
//  0x6F3C5A50 - the JASS native `SetUnitPosition`, "(Hunit;RR)V".
//
//  Stop whatever the unit is doing, then reposition it through CUnit's own
//  slot 96 (+0x180, unit.h) with the two reals copied into locals and their
//  addresses handed over.
//
//  The eight constants after the two positions are the slot's remaining
//  arguments, pushed in the shipped order: (1, 1, -1, 0, 0, 0, -1, 0).  All
//  ten pushes happen before the vtable load, which is MSVC's right-to-left
//  argument evaluation and not a scheduling accident.
//
//  Own translation unit, split from `SetUnitPositionLoc`
//  (jassnatives_unitpositionloc.cpp) which calls it for real.
//============================================================================
#include "jassnatives.h"
#include "unit.h"
#include "CFloat.h"

void __cdecl JASS_SetUnitPosition(int hUnit, const int* x, const int* y)
{
    CUnit* unit = (CUnit*)ResolveUnitHandle(hUnit);

    if (!unit)
        return;

    unit->IssueStopOrder(1);

    //  Both reals are read out before either is stored, which is the
    //  shipped load order (`mov ecx,[eax]` for x, then `mov eax,[edx]` for
    //  y, then the two stores in the other order).
    CFloat px(*x);
    CFloat py(*y);

    unit->Reposition(&px, &py, 1, 1, -1, 0, 0, 0, -1, 0);
}
