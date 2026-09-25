//============================================================================
//  0x6F285740 - CUnit::WantsSubgroupRefreshNotify.
//
//  CAbilityInventory::DetachAndNotify (abilityinventorydispatch.cpp) reads
//  this once, early, and only acts on it after its own detach loop is
//  done: skip the "tell the local player their subgroup selection changed"
//  notification whenever this comes back false.
//
//  CWidget's own m_footprintType (+0x30, widget.h) is read one byte at a
//  time here: `[this+0x33]` is its most significant byte, i.e. the
//  FourCC's leading character when the tag reads as ASCII text.  Neither
//  that check nor +0x5C bit 30 (already CUnit::m_flags5C, used the same
//  way by IsOwningPlayerInWorldMask2E0) is otherwise identified by this
//  call tree - both keep their raw shape rather than a guessed name.
//
//  The two refusal-avoiding branches share one tail (a live-handle count),
//  which is why they are written as two separate `if`s into the same
//  return rather than folded into one boolean expression: MSVC schedules
//  the out-of-range case straight into the count check and only tests the
//  flag bit for the in-range case, and collapsing the two into
//  `if (inRange && !flag) return 1; return count() != 0;` changes which
//  instructions come out.
//============================================================================
#include "unit.h"
#include "widget.h"

int CUnit::WantsSubgroupRefreshNotify()
{
    unsigned char leadChar = ((unsigned char*)&m_footprintType)[3];

    if ((unsigned int)(leadChar - 'A') > 25)
        return CountValidInventoryHandles() != 0;

    if (!(m_flags5C & 0x40000000))
        return 1;

    return CountValidInventoryHandles() != 0;
}
