//============================================================================
//  0x6F282A10 - is this unit's owning player's bit set in the world-record
//  table's own +0x2E0 word?  `this` (a CUnit) in ecx, no stack arguments,
//  `retn 0`.
//
//  Bit 30 (0x40000000) of the unit's own +0x5C flags word gates the whole
//  question - clear it and the answer is a flat false, no lookup at all.
//  Otherwise: index the world-object singleton's per-player array
//  (g_unk6FAB65F4, IndexedArrayHolder_6F3A1650::At - the same array
//  CUnit::Method_0x188, Unit/unit_playertable.cpp, and CUnit::Method_0x18C,
//  Unit/unit_playermasksearch.cpp, both already index) by the *local*
//  player's own acting slot (the holder's own +0x28 word), take that
//  record's +0x2E0 word (gameui.h's own "player record's word at +0x2E0"
//  note - a different table from the two already-named ones at +0x1F8/
//  +0x214), and test whether this unit's own owning-player bit (CUnit's
//  vtable slot 59, GetOwningPlayerIndex) is set in it.  What the mask
//  itself represents past "some per-player relationship to the local
//  player" is not established by anything this dump reaches, so the field
//  keeps its offset rather than a guessed name.
//============================================================================
#include "unit.h"

struct IndexedArrayHolder_6F3A1650
{
    char           pad[0x28];
    unsigned short m_index;   // +0x28 - the local player's slot
    char           pad2[0x2E];
    int            m_data[1];
    int            At(unsigned int index);
};

//  Same record CUnit::Method_0x18C's own SPlayerMembershipRecord names,
//  one more field.
struct SPlayerWorldMaskRecord
{
    char           m_reserved000[0x2E0];
    unsigned short m_mask2E0;   // +0x2E0
};

extern void* g_unk6FAB65F4;

int CUnit::IsOwningPlayerInWorldMask2E0()
{
    if (!(m_flags5C & 0x40000000))
        return 0;

    IndexedArrayHolder_6F3A1650* holder = (IndexedArrayHolder_6F3A1650*)g_unk6FAB65F4;
    SPlayerWorldMaskRecord* record = (SPlayerWorldMaskRecord*)holder->At(holder->m_index);
    unsigned short mask = record->m_mask2E0;

    return ((1 << GetOwningPlayerIndex()) & mask) != 0;
}
