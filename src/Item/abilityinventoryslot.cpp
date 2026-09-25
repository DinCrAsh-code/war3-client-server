//============================================================================
//  0x6F0F0F00, 0x6F0E4050 and 0x6F0E3DB0 - CAbilityInventory's slot
//  reader, the slot count it is bounded by, and a second slot walk that
//  counts how many still resolve to a live handle.
//
//  `GetItemInSlot` is CUnit::GetItemInSlot's tail-jump target
//  (unititeminslot.cpp) and the function JASS_UnitItemInSlot actually
//  asks the question of.  Both of its refusals - index at or past the
//  count, and an empty slot - land on the same `xor eax,eax` / `retn 4`,
//  which is why the empty-slot test jumps *backwards* to the bounds
//  check's own return rather than forwards to one of its own.
//
//  `CountValidHandles` is CUnit::CountValidInventoryHandles's own
//  tail-jump target (unit_inventoryvalidcount.cpp), same reason as
//  GetItemInSlot's own split from CUnit::GetItemInSlot below.
//
//  Own translation unit, separate from unititeminslot.cpp: the shipped
//  CUnit::GetItemInSlot ends in `jmp sub_6F0F0F00`, so the two cannot be
//  in a position to be inlined into each other.  GetSlotCount is here
//  because GetItemInSlot calls it for real (`call sub_6F0E4050`), which
//  it could not if MSVC had folded it in - it is only reached from this
//  one place, so it takes an out-of-line call to keep it out of line, and
//  the shipped build has one.
//============================================================================
#include "abilityinventoryslot.h"
#include "CFloat.h"

//  0x6F02E9E0 - CAbilityInventory::GetLevelField, the per-level field read
//  GetSlotCount's count comes out of.  Its own module (0x6F02Exxx, the
//  ability *type* record) and a real call, so its body is in
//  abilitylevelfield.cpp rather than here.

//  0x6F0E4050 - CAbilityInventory::GetSlotCount, the bound below.  A real
//  out-of-line call in the shipped code, so it is in its own TU
//  (abilityslotcount.cpp) - see its header comment.

//----------------------------------------------------------------------------
//  0x6F0F0F00.
//
//  The occupancy test is `(m_typeTag & m_handle) == -1`, not two separate
//  comparisons: an unused slot holds {-1,-1} and the shipped code folds
//  both words into one `and` before the single `cmp`.  Written as the
//  `and` rather than as `m_handle == -1 && m_typeTag == -1` because that
//  is what the shipped instruction stream contains, and because the two
//  are not the same test - the `and` also rejects any pair whose bits
//  happen to cover each other.
//
//  The slot's *address* is what goes to QueryHandleField0x54, and the
//  same two words are read out of it a second time for the occupancy
//  test; the shipped code really does index the array twice (0x6F0F0F19
//  and 0x6F0F0F1D) and then `lea` the same base a third time.
//----------------------------------------------------------------------------
CAgent* CAbilityInventory::GetItemInSlot(int index)
{
    if ((unsigned int)index >= GetSlotCount())
        return 0;

    SInventorySlot* slot = &m_slots[index];
    if ((slot->m_ref.m_typeTag & (int)slot->m_ref.m_handle) == -1)
        return 0;

    return (CAgent*)QueryHandleField0x54(&slot->m_ref);
}

//----------------------------------------------------------------------------
//  0x6F0E3DB0 - CAbilityInventory::CountValidHandles.  How many of
//  m_slots[0..m_maxSlots) are both occupied (the same `(handle & typeTag)
//  != -1` sentinel test GetItemInSlot uses) *and* still resolve through
//  QueryHandleField0x54 - a slot can be occupied by a handle the global
//  table no longer honours, and this does not count those.
//
//  CUnit::CountValidInventoryHandles (unit_inventoryvalidcount.cpp) tail
//  jumps here after its own null check, so this stays in its own
//  translation unit, same reason as GetSlotCount's split from
//  GetInventorySize.
//----------------------------------------------------------------------------
unsigned int CAbilityInventory::CountValidHandles()
{
    unsigned int count = 0;

    for (unsigned int i = 0; i < m_maxSlots; ++i)
    {
        SInventorySlot* slot = &m_slots[i];
        if ((int)(slot->m_ref.m_handle & slot->m_ref.m_typeTag) != -1)
        {
            if (QueryHandleField0x54(&slot->m_ref))
                count++;
        }
    }

    return count;
}
