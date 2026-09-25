//============================================================================
//  TSGrowableArray<SCheckedUnitSlot> - the array CGameUI's pause highlight
//  sweep appends to (gameuihighlightsweep.cpp).
//
//  The element type is not a guess: 0x6F09F890 hands Storm the RTTI
//  descriptor name ".?AV?$CAgentPtr@VCUnit@@@@" (0x6FA53E28) as its
//  allocation tag, i.e. `class CAgentPtr<class CUnit>`, and 0x6F0A4730
//  constructs each new element through 0x6F038450, which this repo already
//  calls SCheckedUnitSlot.  So the binary's own name for the whole
//  SCheckedXxxSlot family is CAgentPtr<X>; renaming fifteen classes and
//  their thirty-odd funcmap rows is not this batch's to do, so the tag is
//  written out literally here and the established name is kept.
//
//  There is a *second*, separate instantiation of this array in the binary
//  (Agent/agentptrarray.cpp, ~TSGrowableArray at 0x6F286590) whose element
//  constructs through 0x6F02F780 rather than 0x6F038450.  Both tags say
//  CAgentPtr; taken together they say the shipped class is a template and
//  that this repo's `CAgentPtr` and its fifteen `SCheckedXxxSlot`s are the
//  same template at different arguments.  Two instantiations, two symbols,
//  and nothing to merge.
//
//  0x6F0860C0 ComputeChunk (cap 0x40 = 256/sizeof(T)), 0x6F09F890 SetAlloc,
//  0x6F0A4730 New.
//============================================================================
#include "agenttypedslots.h"

#define TSARRAY_TYPENAME ".?AV?$CAgentPtr@VCUnit@@@@"
#include "tsarray.inl"

//----------------------------------------------------------------------------
//  0x6F09F890 - SetAlloc, as an explicit specialisation rather than a plain
//  instantiation, for the same kind of reason Agent/agentptrarray.cpp gives
//  for its own destructor.
//
//  The element has a refcounting copy constructor in the shipped source -
//  the move loop copies m_value and addrefs it when non-null before
//  destroying the source - but it cannot be *declared* on SCheckedUnitSlot:
//  MSVC gives a copy constructor the same readable name as the converting
//  one that lives at 0x6F038450, the pipeline keys a generated instruction
//  stream on that name, and the two collide.  Measured: declaring it cost
//  0x6F038450 thirteen matched instructions in each of 28 dumps.  So the
//  copy is written out here, in the one body that needs it.
//
//  The only thing lost is the null test MSVC puts in front of a placement
//  `new` - one `jz` per moved element - which no spelling of an explicit
//  copy can bring back.
//----------------------------------------------------------------------------
template <>
void TSGrowableArray<SCheckedUnitSlot>::SetAlloc(unsigned int alloc)
{
    SCheckedUnitSlot* olddata = m_data;

    if (alloc < m_count)
    {
        for (unsigned int i = alloc; i < m_count; i++)
            olddata[i].Release();
    }

    m_alloc = alloc;
    m_data = (SCheckedUnitSlot*)SMemReAlloc(olddata, alloc * sizeof(SCheckedUnitSlot),
                                            s_typeName, -2, 0x10);
    if (m_data == 0)
    {
        m_data = (SCheckedUnitSlot*)SMemAlloc(alloc * sizeof(SCheckedUnitSlot),
                                              s_typeName, -2, 0);
        if (olddata != 0)
        {
            unsigned int moved = alloc < m_count ? alloc : m_count;
            for (unsigned int i = 0; i < moved; i++)
            {
                CAgent* held = olddata[i].m_value;
                m_data[i].m_value = held;
                if (held)
                    held->m_refcount++;

                olddata[i].Release();
            }
            SMemFree(olddata, s_typeName, -2, 0);
        }
    }
}

template unsigned int TSGrowableArray<SCheckedUnitSlot>::ComputeChunk(unsigned int);
template SCheckedUnitSlot* TSGrowableArray<SCheckedUnitSlot>::New();
