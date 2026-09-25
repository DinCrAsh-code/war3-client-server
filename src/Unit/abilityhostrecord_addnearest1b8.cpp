//============================================================================
//  0x6F2DF280 - SAbilityHostRecord::AddNearestTo1B8Set (abilityhostrecord.h).
//  FindNearestReachableIndex(unit, 0) then AddUniqueTo1B8Set(that index).
//============================================================================
#include "abilityhostrecord.h"
#include "unit.h"
#include "indexedrecordarray.h"

//  0x6F2DF170 - FindNearestReachableIndex, already reconstructed
//  (abilityhostrecord_findnearestslot.cpp).
int __stdcall FindNearestReachableIndex(void* candidate, const void* fallback);

void SAbilityHostRecord::AddNearestTo1B8Set(CUnit* unit)
{
    AddUniqueTo1B8Set((unsigned int)FindNearestReachableIndex(unit, 0));
}
