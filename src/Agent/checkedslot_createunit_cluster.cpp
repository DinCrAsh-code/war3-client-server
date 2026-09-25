//============================================================================
//  A small cluster of `SCheckedSlot_<getter>`-shaped functions sitting right
//  next to CreateUnitForPlayer (0x6F24F1F0) - part of the CreateUnit closure
//  BFS, group createunit-G3.  Same family, same naming rule,
//  checkedslot_6F0D91A0_assign.cpp / checkedslot_batch2526.h already
//  establish: an anonymous checked-slot class is named after its own
//  fixed-type-id getter's address when nothing in any dump ties it to a
//  real class name.
//
//  Two rawcode getters here decode to real FourCCs (misc_rawcode_getters.cpp
//  convention, MSB-first): 0x6F24F580 is '+ccp' (0x2B636370) and 0x6F2CFBD0
//  is 'AIcs' (0x41496373).  Neither is tied to a known class or ability by
//  anything in this closure slice, so - same rule that file's own header
//  states for its own unplaced FourCCs - they stay literal rather than
//  guessed names.  A sibling getter this cluster calls but does not own,
//  0x6F24F510 ('+ccu', 0x2B636375), is declared extern; its own body is
//  outside this group's worklist slice.
//============================================================================
#include "agent.h"
#include "agiletype.h"
#include "game.h"
#include "widgetagentquery.h"

//  0x6F24F580 - '+ccp' MSB-first, 0x6F2CFBD0 - 'AIcs' MSB-first.  Both
//  defined in Misc/misc_rawcode_getters.cpp (its own established home for
//  this exact `mov eax,<imm32>/retn` leaf shape) rather than here: the
//  shipped code reaches each through a real `call`, and keeping the
//  one-instruction body next to its one caller in the same TU would let
//  this build's /Ob2 inline it away instead.
unsigned int GetFourCC_6F24F580();
unsigned int GetFourCC_6F2CFBD0();

//  0x6F24F510 - '+ccu' MSB-first (misc_rawcode_getters.cpp naming rule) -
//  not in this group's own worklist slice.
unsigned int GetFourCC_6F24F510();

//----------------------------------------------------------------------------
//  0x6F24F600 - SCheckedSlot_6F24F510::Assign.  Same body shape as
//  SCheckedSlot_6F0D91A0::Assign (checkedslot_6F0D91A0_assign.cpp):
//  candidate's own GetAgileTypeId() checked against GetFourCC_6F24F510()
//  via AgileTypeIsDerivedFrom, then the usual release-old/addref-new swap.
//----------------------------------------------------------------------------
struct SCheckedSlot_6F24F510
{
    SCheckedSlot_6F24F510* Assign(CAgent* candidate);          //  0x6F24F600
    SCheckedSlot_6F24F510* Construct(CAgent* candidate);       //  0x6F24FC70
    SCheckedSlot_6F24F510* MakeAndAssign(int, int, int mode);  //  0x6F250CF0

    //  The manually-inlined release every SCheckedSlot_<addr> local this
    //  cluster holds gets at scope exit - the same
    //  `if(m_value){if(--m_value->m_refcount==0)m_value->ReleaseSelf();}`
    //  shape CAgentPtr's own destructor (agent.h) uses, and the same
    //  `6F24F680`-family funclet the SEH-wrapped members of this file carry
    //  appended to their own dump.  Not itself claimed at a separate
    //  address - inlined at every caller, per that funclet.
    void Release()
    {
        if (m_value)
        {
            if (--m_value->m_refcount == 0)
                m_value->ReleaseSelf();
        }
    }
    ~SCheckedSlot_6F24F510() { Release(); }

    CAgent* m_value;
};

SCheckedSlot_6F24F510* SCheckedSlot_6F24F510::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate
        && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(),
                                  GetFourCC_6F24F510()))
        checked = candidate;
    else
        checked = 0;

    CAgent* old = m_value;
    if (old != checked)
    {
        if (old)
        {
            if (--old->m_refcount == 0)
                old->ReleaseSelf();
        }
        if (checked)
        {
            if (candidate)
                candidate->m_refcount++;
        }
        m_value = checked;
    }

    return this;
}

//----------------------------------------------------------------------------
//  0x6F24FC70 - SCheckedSlot_6F24F510::Construct: `m_value = 0;
//  Assign(candidate); return this;` - the plain, generic checked-slot
//  constructor, same shape cunit_agent_seh_abilityfactories.cpp's own
//  0x6F280F60 (SCheckedAbilitySlot_Aque::SCheckedAbilitySlot_Aque) is, and
//  it carries the identical push -1/SEH-offset/fs:0 cookie
//  __except_handler4 frame despite having no destructible local of its
//  own - the frame protects partially-constructed `this` across the call
//  to Assign, which the compiler cannot prove won't throw.  This build's
//  fixed /GS- /EHs-c- cannot reach that frame shape (docs/msvc-vc8-idioms
//  .md), so this is capped well short of EXACT by frame bytes alone, same
//  as that address.
//----------------------------------------------------------------------------
SCheckedSlot_6F24F510* SCheckedSlot_6F24F510::Construct(CAgent* candidate)
{
    m_value = 0;
    Assign(candidate);
    return this;
}

//----------------------------------------------------------------------------
//  0x6F250CF0 - SCheckedSlot_6F24F510::MakeAndAssign.  Same call sequence
//  as the MAKE_ABILITY_AGENT_FACTORY family (cunit_agent_seh_abilityfactories
//  .cpp) - GetFourCC/AGILE_TYPE_ID::Hash/TSHashTable::Ptr/
//  InitWidgetAgentQuery/SubmitWidgetAgentQuery/+0x54 holder read, in the
//  shipped order - except the rawcode getter is called once and its result
//  reused (not called twice), there is no destructible local so no SEH
//  frame wraps it, and the query's own +0x24 field (widgetagentquery.h's
//  `m_reserved24`, "normally -1") is patched to -1 or -2 depending on the
//  third argument before the submit - nothing in this closure names what
//  that selects, so it stays a raw-offset write rather than a guessed
//  field name.  Returns `this` (not the made CAgent*), through this slot's
//  own Assign (0x6F24F600).  The first two stack arguments arrive (three
//  pushes at the one call site this session's dump has, 0x6F250FD0: 0, 0,
//  2) but neither is read anywhere in this body.
//----------------------------------------------------------------------------
namespace {
struct SMadeAgentHolder3
{
    char    m_reserved00[0x54];
    CAgent* m_pAgent;   // +0x54
};
}

SCheckedSlot_6F24F510* SCheckedSlot_6F24F510::MakeAndAssign(int, int, int mode)
{
    CGameData* game = g_pGameData;
    unsigned int typeId = GetFourCC_6F24F510();
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, typeId, pool);
    *(int*)((char*)&query + 0x24) = -1 - (mode == 2);

    Assign(((SMadeAgentHolder3*)SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);
    return this;
}

//----------------------------------------------------------------------------
//  0x6F24F6F0 - SCheckedSlot_6F2CFBD0::Assign.  Identical shape, checked
//  against 'AIcs' instead.
//----------------------------------------------------------------------------
struct SCheckedSlot_6F2CFBD0
{
    SCheckedSlot_6F2CFBD0* Assign(CAgent* candidate);          //  0x6F24F6F0

    CAgent* m_value;
};

SCheckedSlot_6F2CFBD0* SCheckedSlot_6F2CFBD0::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate
        && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(),
                                  GetFourCC_6F2CFBD0()))
        checked = candidate;
    else
        checked = 0;

    CAgent* old = m_value;
    if (old != checked)
    {
        if (old)
        {
            if (--old->m_refcount == 0)
                old->ReleaseSelf();
        }
        if (checked)
        {
            if (candidate)
                candidate->m_refcount++;
        }
        m_value = checked;
    }

    return this;
}

//----------------------------------------------------------------------------
//  0x6F24F760 - SCheckedSlot_6F24F580::Assign.  Identical shape again,
//  checked against '+ccp'.  0x6F24FEF0 is this slot's own SEH constructor,
//  same shape as SCheckedSlot_6F24F510::Construct above.
//----------------------------------------------------------------------------
struct SCheckedSlot_6F24F580
{
    SCheckedSlot_6F24F580* Assign(CAgent* candidate);          //  0x6F24F760
    SCheckedSlot_6F24F580* Construct(CAgent* candidate);       //  0x6F24FEF0

    void Release()
    {
        if (m_value)
        {
            if (--m_value->m_refcount == 0)
                m_value->ReleaseSelf();
        }
    }
    ~SCheckedSlot_6F24F580() { Release(); }

    CAgent* m_value;
};

SCheckedSlot_6F24F580* SCheckedSlot_6F24F580::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate
        && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(),
                                  GetFourCC_6F24F580()))
        checked = candidate;
    else
        checked = 0;

    CAgent* old = m_value;
    if (old != checked)
    {
        if (old)
        {
            if (--old->m_refcount == 0)
                old->ReleaseSelf();
        }
        if (checked)
        {
            if (candidate)
                candidate->m_refcount++;
        }
        m_value = checked;
    }

    return this;
}

SCheckedSlot_6F24F580* SCheckedSlot_6F24F580::Construct(CAgent* candidate)
{
    m_value = 0;
    Assign(candidate);
    return this;
}

//----------------------------------------------------------------------------
//  0x6F24FBA0 - a bare field accessor: `mov eax,[ecx+20h] / retn`.  A
//  __fastcall free function rather than a member for the same reason
//  QueryHandleField0x54 (itemhandleresolve.h) is one - nothing in this
//  closure names the owning class, so this stays a raw-offset accessor.
//  Defined in Misc/misc_field_getters.cpp (its own established home for
//  this exact one-field-accessor shape), not here: Register_6F250FD0 below
//  reaches it through a real `call`, and a same-TU definition would let
//  this build's /Ob2 inline the one-instruction body away instead.
//----------------------------------------------------------------------------
void* __fastcall GetField0x20(void* obj);

//----------------------------------------------------------------------------
//  0x6F250D70 - a growable array of SCheckedSlot_6F24F580 elements (each
//  4 bytes - a bare `CAgent* m_value`, checkedslot_createunit_cluster.cpp
//  above), and this is its "append one fresh, unassigned slot" member:
//  grow if the array is full (chunk-round the desired size the same way
//  Containers/computechunk*.cpp's own family does, then hand the rounded
//  size to a rehash callee), place the new element at [count], bump the
//  count, and default-construct it (candidate=0) through
//  SCheckedSlot_6F24F580::Construct (0x6F24FEF0) so it comes back with
//  m_value already null rather than garbage.  Two callees here
//  (0x6F24F8E0 - the chunk-size helper, called only once the cache at
//  +0x0C is empty; 0x6F250340 - the actual rehash) are outside this
//  group's own worklist slice and have no dump; both are naked redirects
//  to their real, unhooked shipped bodies rather than reconstructions.
//  Nothing in any dump here names the owning class, so it stays
//  address-derived like its sibling anonymous checked-slot types.
//----------------------------------------------------------------------------
struct SCheckedSlotArray_6F250D70
{
    unsigned int            m_capacity;           // +0x00
    unsigned int            m_count;              // +0x04
    SCheckedSlot_6F24F580*  m_data;               // +0x08
    unsigned int            m_growthChunkCache;   // +0x0C

    unsigned int ComputeGrowthChunk(unsigned int desired);  //  0x6F24F8E0
    void Rehash(unsigned int newCapacity);                  //  0x6F250340

    SCheckedSlot_6F24F580* AppendNew();                     //  this file
};

SCheckedSlot_6F24F580* SCheckedSlotArray_6F250D70::AppendNew()
{
    unsigned int want = m_count + 1;
    if (want > m_capacity)
    {
        unsigned int chunk = m_growthChunkCache;
        if (!chunk)
            chunk = ComputeGrowthChunk(want);

        unsigned int rounded = want;
        unsigned int rem = want % chunk;
        if (rem)
            rounded = want + (chunk - rem);

        Rehash(rounded);
    }

    SCheckedSlot_6F24F580* slot =
        (SCheckedSlot_6F24F580*)((char*)m_data + m_count * 4);
    m_count = m_count + 1;

    if (slot)
        slot->Construct(0);

    return slot;
}

//  0x6F24F8E0/0x6F250340 - out of this group's own worklist slice; naked
//  redirects to the real, unhooked shipped bodies rather than
//  reconstructions of them.
__declspec(naked) unsigned int SCheckedSlotArray_6F250D70::ComputeGrowthChunk(unsigned int)
{
    __asm { mov eax, 06F24F8E0h }
    __asm { jmp eax }
}

__declspec(naked) void SCheckedSlotArray_6F250D70::Rehash(unsigned int)
{
    __asm { mov eax, 06F250340h }
    __asm { jmp eax }
}

//----------------------------------------------------------------------------
//  0x6F250FD0 - the cluster's own top-level entry point.  `candidate`
//  arrives in ecx (no stack args, bare `retn`).  dword_6FAB3C04 is a
//  module-global compound object this whole cluster shares, laid out (from
//  what this function itself reads and writes) as:
//
//      +0x00  SCheckedSlot_6F24F510   m_primary     (this file)
//      +0x04  char                    m_reserved04[0x1C]  (untouched here)
//      +0x20  SCheckedSlotArray_6F250D70 m_related  (0x6F250D70, this file)
//
//  Bails immediately unless the world singleton (g_unk6FAB65F4) exists and
//  its own +0x3E0 flag is set - the same two-step gate
//  GameUI/gameuihighlightupdate.cpp's UpdatePauseHighlights already reads
//  off the identical global.  Lazily makes `m_primary` the first time
//  through (MakeAndAssign with a literal 2 for the "mode" argument
//  MakeAndAssign's own header already flags as unplaced).  Then walks
//  `m_related` for an entry whose own agent reads `candidate` back out of
//  its own +0x20 field (GetField0x20) and, if none matches, makes a fresh
//  '+ccp'-tagged agent (this cluster's own GetFourCC_6F24F580), tells it
//  about `candidate` through a vtable slot this closure does not otherwise
//  name (slot 24 / +0x60 - past the last slot agent.h declares, so a raw
//  dispatch rather than a new CAgent virtual), and appends it to
//  `m_related`.  Every local holder here is a real SCheckedSlot_<addr>
//  whose destructor (Release() above) fires at the closing brace of the
//  `if` that declared it - the shipped code inlines that exactly where
//  each brace below is, the same `6F24F680`/`6F24F7E0`-family funclet its
//  own dump carries.
//
//  No asm/ dump exists for this address (agent_worktrees raw_asm only);
//  the unreproducible push -1/SEH-offset/fs:0 __except_handler4 frame this
//  build's fixed /GS- /EHs-c- cannot reach caps this well short of EXACT
//  regardless, same as every other SEH-wrapped member of this file.
//----------------------------------------------------------------------------
struct SGlobalRegistry_6FAB3C04
{
    SCheckedSlot_6F24F510      m_primary;         // +0x00
    char                       m_reserved04[0x1C]; // +0x04
    SCheckedSlotArray_6F250D70 m_related;          // +0x20
};

extern SGlobalRegistry_6FAB3C04 g_registry_6FAB3C04;   // dword_6FAB3C04

//  dword_6FAB65F4 - the world-object singleton (Unit/unit_publishposition.cpp
//  and friends).  Declared, never defined: the real game's global.
extern void* g_unk6FAB65F4;

typedef void (__thiscall *NotifyCandidateFn)(void* self, CAgent* candidate);

void __fastcall Register_6F250FD0(CAgent* candidate)
{
    void* world = g_unk6FAB65F4;
    if (!world)
        return;
    if (*(int*)((char*)world + 0x3E0) == 0)
        return;

    if (g_registry_6FAB3C04.m_primary.m_value == 0)
    {
        SCheckedSlot_6F24F510 held;
        held.Construct(0);
        held.MakeAndAssign(0, 0, 2);
        g_registry_6FAB3C04.m_primary.Assign(held.m_value);
    }

    SCheckedSlotArray_6F250D70* related = &g_registry_6FAB3C04.m_related;
    unsigned int count = related->m_count;
    unsigned int i = 0;
    if (count > 0)
    {
        SCheckedSlot_6F24F580* data = related->m_data;
        for (i = 0; i < count; i++)
        {
            if (GetField0x20(data[i].m_value) == candidate)
                break;
        }
    }

    if (i != count)
        return;

    {
        SCheckedSlot_6F24F580 held2;
        held2.Construct(0);

        unsigned int typeId = GetFourCC_6F24F580();
        CGameData* game = g_pGameData;
        AGILE_TYPE_ID id(typeId);
        unsigned int hash = id.Hash();
        AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
        void* pool = type->m_allocator;

        SWidgetAgentQuery query;
        InitWidgetAgentQuery(&query, typeId, pool);
        *(int*)((char*)&query + 0x24) = -2;

        held2.Assign(((SMadeAgentHolder3*)SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

        (*(NotifyCandidateFn**)held2.m_value)[0x60 / 4](held2.m_value, candidate);

        SCheckedSlot_6F24F580* newSlot = related->AppendNew();
        newSlot->Assign(held2.m_value);
    }
}
