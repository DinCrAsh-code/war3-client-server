//============================================================================
//  0x6F37CB20 - TSGrowableArray<SCheckedSelectableSlot>::New().  Grow by one
//  slot and converting-construct it through SCheckedSelectableSlot's own
//  constructor (0x6F2C9670, GameUI/gameuiselectableslot.cpp), the same
//  member tsarray.inl's generic New() calls for every other instantiation.
//
//  Own translation unit: this is the one member of the array that carries
//  an SEH prologue/epilogue of its own, because the element constructor it
//  calls opens a __try/__finally frame (agenttypedslots.h's own note on the
//  family) and the placement `new` here needs to unwind through that call
//  the same way every other caller of a JASS-slot-family constructor does.
//============================================================================
#include "agenttypedslots.h"
#include "tsarray.inl"

//  Declared, not defined, here: Containers/selectableptrarray.cpp carries
//  the real ComputeChunk/SetAlloc specialisations.  Without these
//  declarations ahead of the call below, this TU would see only the
//  generic primary template for SetAlloc and implicitly instantiate its
//  own (wrong - no addref) copy alongside it.
template unsigned int TSGrowableArray<SCheckedSelectableSlot>::ComputeChunk(unsigned int);
template <>
void TSGrowableArray<SCheckedSelectableSlot>::SetAlloc(unsigned int);

template <>
SCheckedSelectableSlot* TSGrowableArray<SCheckedSelectableSlot>::New()
{
    unsigned int count = m_count + 1;
    if (count > m_alloc)
    {
        unsigned int chunk = m_chunk;
        if (chunk == 0)
            chunk = ComputeChunk(count);

        unsigned int alloc = count;
        unsigned int over = count % chunk;
        if (over != 0)
            alloc = count + (chunk - over);

        SetAlloc(alloc);
    }

    SCheckedSelectableSlot* slot = &m_data[m_count];
    m_count++;

    return new (slot) SCheckedSelectableSlot(0);
}
