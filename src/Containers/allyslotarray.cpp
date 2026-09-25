//============================================================================
//  TSGrowableArray<AllySlot> - the growable list a start location keeps its
//  ally priorities in (jassnatives.h).
//
//      0x6F3A02B0  ComputeChunk    0x6F3A0310  SetAlloc
//      0x6F3A3C60  SetCount
//
//  The element type's name is not a guess: SetAlloc hands Storm the RTTI
//  descriptor name ".?AUAllySlot@@" as the allocation tag, which is IDA's
//  `aAuallyslot`.  Eight bytes an element, so ComputeChunk's cap is
//  256 / 8 = 0x20 - the `cmp eax, 20h` at the top of 0x6F3A02B0.
//
//  Own translation unit, exactly like tsarray.cpp's instantiation over the
//  relationship table's bucket type: SetCount issues genuine calls to
//  ComputeChunk and SetAlloc, and CJassGameState::SetStartLocPrioCount
//  (jassgamestate_startlocation.cpp) issues a genuine call to SetCount.
//
//  Neither the destroy loop SetCount's shrink arm has nor the one SetAlloc
//  opens with survives: AllySlot's destructor is trivial and both optimise
//  away, which is why the shipped bodies have no such loop.  The `if (eax)`
//  guard around each element store is the placement-new null check MSVC
//  emits for a type that *does* have a constructor.
//============================================================================
#include "jassnatives.h"

#define TSARRAY_TYPENAME ".?AUAllySlot@@"
#include "tsarray.inl"

template unsigned int TSGrowableArray<AllySlot>::ComputeChunk(unsigned int);
template void TSGrowableArray<AllySlot>::SetAlloc(unsigned int);
template void TSGrowableArray<AllySlot>::SetCount(unsigned int);
