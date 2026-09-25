//============================================================================
//  0x6F495140 - NIpse::CPoPosCl::Slot21 (vtable slot 21, own override) and
//  its two new small leaves: 0x6F488F20 (borrow a scratch related-agent
//  list off CTimeSync's own fixed 8-slot pool) and 0x6F486950
//  (SGridRegistrationPtrArray::RemoveRange - the same shape
//  SBhPoTickRelaySlotArray::RemoveRange, Missile/bhpoprojectile_slots.cpp,
//  already documents, a genuinely separate compiled twin, see
//  Pathfinding/spatialgrid.h's own header comment on
//  SGridRegistrationPtrArray for why it is declared there and defined
//  here).  The release half of the borrow (0x6F481410, a plain decrement)
//  is Misc/misc_trivial_getters.cpp's own `SField230Owner::DecrementField230`
//  - paired with `SField238Owner::GetField238` there since both are bare
//  CTimeSync field accessors from the same neighbourhood.
//
//  This is the "genuinely new CPoSeparate allocator chain and ~130-
//  instruction related-agent list rebuild" `poposcl.h`'s own header
//  comment and docs/targets/NTempestNIpse_clusterH_remainder.md both left
//  `TODO` across the earlier cluster-H-remainder sessions - traced fully
//  this follow-up session via `Pathfinding/spatialgrid.h`'s own
//  `CGridRegistration`/`CCellGrid` shapes (RegisterCell's own `flag`
//  argument - 0 for the "old box" layer, 0x1000000 for the "new box" one,
//  0x2000000 reserved for `RegisterAStarCell`'s own head records - is what
//  makes sense of the two collection loops below).
//
//  Overall shape: given a new world position, floor+truncate it to a
//  cell coordinate; if that cell hasn't changed from the one already
//  cached at +0xD0/+0xD4 (`m_pclX`/`m_pclY`), do nothing at all - not even
//  touch the output. Otherwise borrow one of CTimeSync's eight scratch
//  `SGridRegistrationPtrArray` instances, collect every OTHER agent
//  registration still live in the OLD cell (layer 1 only -
//  `CCellGrid::CollectRelated`, Pathfinding/spatialgrid.cpp) into it, then
//  collect the NEW cell's on top of that (append, no clear in between),
//  de-duplicate the newly-appended entries against the old ones (an agent
//  present in both cells is noise, not two relations), notify each
//  surviving OLD-cell registration's own node record through its own
//  vtable slot 8 with one tag buffer, then re-notify each surviving
//  NEW-cell one through the same slot with a second, near-identical tag
//  buffer (one dword differs - see `STagBuffer` below), release the
//  scratch list, and finally stamp the new cell coordinate into
//  `m_pclX`/`m_pclY`.
//============================================================================
#include "poposcl.h"
#include "spatialgrid.h"
#include "pathfind.h"
#include "game.h"
#include "CFloat.h"

extern "C" void* __cdecl memmove(void* dst, const void* src, unsigned int size);

//  0x6F481430/0x6F481410 - Misc/misc_trivial_getters.cpp.  Same minimal
//  local redeclaration convention Agent/popos_apply_slot4.cpp's own copy
//  already uses (g_pTimeSync itself is declared in game.h, included
//  above).
struct SField238Owner { int __thiscall GetField238(); };
struct SField230Owner { void __thiscall DecrementField230(); };

//  0x6F488F20 - borrow the next scratch related-agent list off CTimeSync's
//  own fixed 8-slot pool. Own translation unit
//  (Agent/scratchpoolowner.cpp) - Slot21 below is its only caller, and
//  keeping the two together let `/Ob2` inline this one straight into
//  Slot21, losing the shipped `call sub_6F488F20` entirely (the same
//  same-TU-single-caller inlining trap `poposcl_relatedagentslot4_teardown.cpp`'s
//  own header comment already documents for an analogous pair in this
//  same class family).  See that file for the full body comment.
struct SScratchPoolOwner
{
    SGridRegistrationPtrArray* __thiscall BorrowScratchRelatedList();
};

//----------------------------------------------------------------------------
//  0x6F486950 - see spatialgrid.h's own header comment on
//  SGridRegistrationPtrArray for why this is declared there and defined
//  here: byte-identical shape to `SBhPoTickRelaySlotArray::RemoveRange`
//  (0x6F4AB0D0), a separate compiled twin.  `this` in ecx, `retn 8`.
//----------------------------------------------------------------------------
int SGridRegistrationPtrArray::RemoveRange(unsigned int start, unsigned int count)
{
    if (start >= m_count)
        return 0;

    unsigned int clampedCount = count;
    if (start + clampedCount > m_count)
        clampedCount = m_count - start;

    //  Same 0.943 (33/35) as `SBhPoTickRelaySlotArray::RemoveRange`
    //  (0x6F4AB0D0) already documents - the two-instruction tail
    //  subtraction and one lea's base/index registers come out scheduled
    //  in the opposite (commutative) order from this exact source shape
    //  no matter which way the two operands are written here; register
    //  allocation only, pending angr IDENTICAL verification.
    unsigned int tail = m_count - start - clampedCount;
    if (tail != 0)
    {
        void** data = (void**)m_storage.m_data;
        memmove(&data[start], &data[start + clampedCount], tail * 4);
    }

    m_count -= clampedCount;
    return 1;
}

namespace NIpse {

//  Called through the surviving related registration's own +0x30
//  `m_nodeRecord` object, vtable slot 8 (`[vtbl + 0x20]`) - the same
//  "unidentified receiver, addressed purely through a shared vtable-slot
//  convention" shape CLAUDE.md's own examples document, and
//  `Agent/poposbh_relatedagents.cpp`'s own `GenericSlot4Fn` already uses
//  for an analogous unresolved dispatch in this same class family.  One
//  stack argument (the tag buffer below), `this` in ecx.
typedef void (__thiscall *NotifyRelatedFn)(void*, void*);

//  The 11-dword buffer the shipped code builds on its own stack and
//  passes to the notify dispatch above by address.  Fields +0x0/+0x4 are
//  fixed constants (0x5E70726F/0x60706375) in both loops below; +0x8
//  differs between them (0x6370266C for the OLD-cell loop, 0x63702665 for
//  the NEW-cell one) - none of the three decode as a printable string in
//  either byte order this session tried, so they are reproduced verbatim
//  as the literal dwords the shipped `mov [mem], imm32` stores rather than
//  guessed at as text; +0xC is `this` (CPoPosCl*), +0x10..+0x20 are zeroed,
//  and +0x24/+0x28 are -1.
struct STagBuffer
{
    unsigned int m_tag0;
    unsigned int m_tag1;
    unsigned int m_tag2;
    CPoPosCl*    m_owner;
    unsigned int m_zero1;
    unsigned int m_zero2;
    unsigned int m_zero3;
    unsigned int m_zero4;
    unsigned int m_zero5;
    unsigned int m_minusOneA;
    unsigned int m_minusOneB;
};

//----------------------------------------------------------------------------
//  0x6F495140 - vtable slot 21, own override.  See this file's own header
//  comment for the full shape.  `this` in ecx, `retn 4` - one stack
//  argument, the new world position.
//----------------------------------------------------------------------------
void CPoPosCl::Slot21(const CGridVec2* pos)
{
    if (m_relatedAgent == 0)
        return;

    //  CTimeSync's own +0x238 field, reinterpreted as the second grid
    //  context - the SAME "reinterpret an int field as a CCellGrid*"
    //  idiom CPoPos::Apply's own header comment documents for the
    //  m_syncChannelB registration (Agent/popos_apply_slot4.cpp).
    int tick = ((SField238Owner*)g_pTimeSync)->GetField238();
    CCellGrid* grid = (CCellGrid*)tick;

    CFloat floorY = CFloatFloor(pos->m_y);
    int newY = CFloatToInt(floorY);
    CFloat floorX = CFloatFloor(pos->m_x);
    int newX = CFloatToInt(floorX);

    if ((unsigned int)newX == m_pclX && (unsigned int)newY == m_pclY)
        return;

    SGridRegistrationPtrArray* related =
        ((SScratchPoolOwner*)g_pTimeSync)->BorrowScratchRelatedList();
    if (related == 0)
        return;

    SCellPoint oldCell((int)m_pclX, (int)m_pclY);
    grid->CollectRelated(&oldCell, related);
    unsigned int oldCount = related->m_count;

    SCellPoint newCell(newX, newY);
    grid->CollectRelated(&newCell, related);
    unsigned int newCount = related->m_count;

    void** entries = (void**)related->m_storage.m_data;

    //  An agent collected out of both cells is not a new relation - null
    //  both copies. No early exit on the first match: the shipped loop
    //  keeps scanning every earlier entry regardless (harmless, since a
    //  live agent is only ever registered once per cell).
    if (oldCount < newCount)
    {
        for (unsigned int outer = oldCount; outer < newCount; ++outer)
        {
            void* candidate = entries[outer];
            for (unsigned int i = 0; i < oldCount; ++i)
            {
                if (entries[i] == candidate)
                {
                    entries[i] = 0;
                    entries[outer] = 0;
                }
            }
        }
    }

    STagBuffer tag;
    tag.m_tag0 = 0x5E70726F;
    tag.m_tag1 = 0x60706375;
    tag.m_tag2 = 0x6370266C;
    tag.m_owner = this;
    tag.m_zero1 = 0;
    tag.m_zero2 = 0;
    tag.m_zero3 = 0;
    tag.m_zero4 = 0;
    tag.m_zero5 = 0;
    tag.m_minusOneA = 0xFFFFFFFF;
    tag.m_minusOneB = 0xFFFFFFFF;

    for (unsigned int i = 0; i < oldCount; ++i)
    {
        CGridRegistration* entry = (CGridRegistration*)entries[i];
        if (entry == 0)
            continue;
        if (entry->m_visitStamp == -1)
            continue;
        void* node = entry->m_nodeRecord;
        if (node == 0)
            continue;

        NotifyRelatedFn fn = (NotifyRelatedFn)(*(void***)node)[8];
        fn(node, &tag);
    }

    tag.m_tag2 = 0x63702665;

    for (unsigned int i = oldCount; i < newCount; ++i)
    {
        CGridRegistration* entry = (CGridRegistration*)entries[i];
        if (entry == 0)
            continue;
        if (entry->m_visitStamp == -1)
            continue;
        void* node = entry->m_nodeRecord;
        if (node == 0)
            continue;

        NotifyRelatedFn fn = (NotifyRelatedFn)(*(void***)node)[8];
        fn(node, &tag);
    }

    ((SField230Owner*)g_pTimeSync)->DecrementField230();

    m_pclX = (unsigned int)newX;
    m_pclY = (unsigned int)newY;
}

}  // namespace NIpse
