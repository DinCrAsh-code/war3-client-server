//============================================================================
//  createunit-G3's second cluster: the global creep-spot registry
//  CUnit::NotifyAbilityHostChanged (unit_abilityhostnotify.cpp) reaches
//  through 0x6F2E51D0 for a neutral-hostile (player index 0x0C) unit.
//  dword_6FAB4EC0 is a `TSGrowableArray<CAgentPtr<CCreepSpot>>` - not a
//  guess: 0x6F2E2740 hands Storm the RTTI descriptor name
//  ".?AV?$CAgentPtr@VCCreepSpot@@@@" (0x6FA7546C) as its allocation tag,
//  the same way unitagentptrarray.cpp's own header explains for
//  TSGrowableArray<SCheckedUnitSlot>.  This repo's own name for
//  `CAgentPtr<CCreepSpot>` is SCheckedSlot_6F24F580 (Agent/checkedslot_
//  createunit_cluster.cpp - Construct/Assign at 0x6F24FEF0/0x6F24F760,
//  checked against the '+ccp' agile type id 0x6F24F580 hands back), so
//  '+ccp' is very likely CCreepSpot's own agile type id - a synthetic
//  '+'-prefixed tag, same family as CAgent/CWidget/CUnit's own
//  ('+aga'/'+w3w'/'+w3u', misc_rawcode_getters.cpp), not a WC3 object
//  rawcode.  This TU only needs SCheckedSlot_6F24F580's own layout (one
//  CAgent* at +0x00), not its Assign/Construct, so it gets a minimal
//  redeclaration here - the same "match the real definition's shape,
//  nothing more" rule this repo's other cross-TU struct redeclarations
//  already follow (e.g. unit_abilityhostnotify.cpp's SUnitAbilityHost).
//============================================================================
#include "agent.h"

struct SCheckedSlot_6F24F580
{
    CAgent* m_value;   // +0x00
};

#define TSARRAY_TYPENAME ".?AV?$CAgentPtr@VCCreepSpot@@@@"
#include "tsarray.inl"

//----------------------------------------------------------------------------
//  0x6F2DCB10 - TSGrowableArray<SCheckedSlot_6F24F580>::ComputeChunk, this
//  instantiation's own compiled copy (this image is linked without
//  /OPT:ICF, agentdefaults.cpp, so every instantiation gets its own real
//  body).  256/sizeof(SCheckedSlot_6F24F580) = 0x40, matching the shipped
//  cap exactly.
//----------------------------------------------------------------------------
template unsigned int TSGrowableArray<SCheckedSlot_6F24F580>::ComputeChunk(unsigned int);

//----------------------------------------------------------------------------
//  0x6F2E2740 - SetAlloc, an explicit specialisation for the same reason
//  unitagentptrarray.cpp's own TSGrowableArray<SCheckedUnitSlot>::SetAlloc
//  is one: SCheckedSlot_6F24F580's copy needs an addref MSVC's own
//  generated copy constructor would not give it, and declaring one here
//  would collide with the converting-constructor name Construct
//  (0x6F24FEF0) already owns.
//----------------------------------------------------------------------------
template <>
void TSGrowableArray<SCheckedSlot_6F24F580>::SetAlloc(unsigned int alloc)
{
    SCheckedSlot_6F24F580* olddata = m_data;

    if (alloc < m_count)
    {
        for (unsigned int i = alloc; i < m_count; i++)
        {
            CAgent* held = olddata[i].m_value;
            if (held)
            {
                if (--held->m_refcount == 0)
                    held->ReleaseSelf();
            }
        }
    }

    m_alloc = alloc;
    m_data = (SCheckedSlot_6F24F580*)SMemReAlloc(olddata, alloc * sizeof(SCheckedSlot_6F24F580),
                                                 s_typeName, -2, 0x10);
    if (m_data == 0)
    {
        m_data = (SCheckedSlot_6F24F580*)SMemAlloc(alloc * sizeof(SCheckedSlot_6F24F580),
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

                CAgent* dropped = olddata[i].m_value;
                if (dropped)
                {
                    if (--dropped->m_refcount == 0)
                        dropped->ReleaseSelf();
                }
            }
            SMemFree(olddata, s_typeName, -2, 0);
        }
    }
}

//----------------------------------------------------------------------------
//  0x6F2E4B00 - "make sure element `index` exists", not a plain SetCount:
//  a no-op whenever `index` is already below the array's own m_count (no
//  growth, no clear, no count store at all - the shipped code's own first
//  branch skips straight to the epilogue), otherwise grow to `index+1`
//  (the same chunk-rounded SetAlloc call SetCount's own grow makes, just
//  computed inline rather than through a shared helper) and, when
//  `clear` is set, `memset` the newly-uncovered span to zero before
//  running the usual placement-new-equivalent null-fill loop over
//  whatever `memset` did not already reach.  Not declared on
//  TSGrowableArray itself (storm.h's own member list is shared by every
//  instantiation in this repo; this shape is not one of its established
//  members), so it stays a free function taking the array by its own
//  `this` in ecx, matching the shipped `__thiscall`-shaped `retn 8`.
//----------------------------------------------------------------------------
extern "C" void* __cdecl memset(void* dst, int val, unsigned int size);

//  A non-virtual, no-added-data derived view - same size, same layout,
//  same address as the real TSGrowableArray<SCheckedSlot_6F24F580> - so
//  this can add a genuine `this`-taking (`__thiscall`) member without
//  touching storm.h's own shared template, which every other
//  instantiation in this repo also compiles against.
struct CreepSpotArray : public TSGrowableArray<SCheckedSlot_6F24F580>
{
    void EnsureIndex(unsigned int index, int clear);
};

void CreepSpotArray::EnsureIndex(unsigned int index, int clear)
{
    if (index < m_count)
        return;

    unsigned int newCount = index + 1;
    if (newCount > m_alloc)
    {
        unsigned int chunk = m_chunk;
        if (!chunk)
            chunk = ComputeChunk(newCount);

        unsigned int alloc = newCount;
        unsigned int over = newCount % chunk;
        if (over)
            alloc = newCount + (chunk - over);

        SetAlloc(alloc);
    }

    if (clear)
    {
        unsigned int oldCount = m_count;
        memset(&m_data[oldCount], 0, (index - oldCount) * 4 + 4);
    }

    for (unsigned int i = m_count; i <= index; i++)
    {
        SCheckedSlot_6F24F580* slot = &m_data[i];
        if (slot)
            slot->m_value = 0;
    }

    m_count = newCount;
}
