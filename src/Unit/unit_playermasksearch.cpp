//============================================================================
//  0x6F285650 / 0x6F2856C0 - CUnit's vtable slots 99 (+0x18C) and 114
//  (+0x1C8).  Both naked redirects in unit_slotthunks.cpp until now.
//
//  One question asked twice: **is this unit a member of any player who can
//  see the local player?**  Walk all twelve player records, keep the ones
//  whose relation agent's +0x38 mask has the local player's bit set, and
//  ask that record's own membership set whether it holds this unit.  First
//  yes wins.
//
//  Slot 0x1C8 is slot 0x18C plus one exclusion: it skips the local
//  player's own record.  It establishes that by fetching it again -
//  `At(m_index)` a second time, inside the loop, rather than hoisting it -
//  and comparing pointers, which is the shipped code and is why the second
//  call is written inside the `for` here.
//
//  Twelve, not the array's own length: the bound is a literal `cmp edi, 0Ch`
//  in both bodies.
//
//  The mask read is `1 << localSlot` with the shift count in cl - the
//  variable shift, not a table - and the local slot is a *word* at the
//  holder's +0x28, which is what the `movzx` says.
//
//  Own translation unit: neither calls the other, and every one of their
//  four callees has to stay a real call.
//============================================================================
#include "unit.h"
#include "jassrelationagent.h"
#include "unitmembership.h"

extern void* g_unk6FAB65F4;

//  Re-declared to match unit_playertable.cpp / misc_field_getters.cpp
//  exactly - same class name, same member, so the calls mangle alike and
//  reach the one real body.
struct IndexedArrayHolder_6F3A1650
{
    char pad[0x28];
    unsigned short m_index;   // +0x28 - the local player's slot
    char pad2[0x2E];
    int  m_data[1];
    int  At(unsigned int index);
};

//  The record At() hands back.  Its +0x38 relation slot is
//  jassrelationagent.h's; +0x34 is the membership set, which is the field
//  CUnit::Method_0x188 (unit_playertable.cpp) already reaches.
struct SPlayerMembershipRecord
{
    char                m_reserved00[0x34];
    SUnitMembershipSet* m_pMembers;    // +0x34
};

enum { kPlayerRecordCount = 12 };

int CUnit::Method_0x18C()
{
    IndexedArrayHolder_6F3A1650* holder =
        (IndexedArrayHolder_6F3A1650*)g_unk6FAB65F4;
    unsigned int localSlot = holder->m_index;

    for (unsigned int i = 0; i < kPlayerRecordCount; i++)
    {
        SPlayerMembershipRecord* record =
            (SPlayerMembershipRecord*)holder->At(i);
        if (record == 0)
            continue;

        //  The agent in a named local, not chained into the field read:
        //  chained, MSVC forms the field's address with `mov`/`add` where
        //  the shipped code has one `lea` - the same effect
        //  jassgamestatemasks.cpp's DetectedMask records.
        SJassRelationAgent* agent =
            ((SJassPlayerRelations*)record)->EnsureRelationAgent();
        unsigned int mask = (unsigned int)agent->m_player.QueryField78();
        //  One instruction is not reproducible from either spelling: the
        //  shipped `test` has the shifted bit first and the mask second,
        //  and writing the `&` either way round leaves MSVC emitting
        //  `test mask, bit`.  Measured both ways.
        if ((mask & (1u << localSlot)) == 0)
            continue;

        if (record->m_pMembers->Contains(this) != 0)
            return 1;
    }
    return 0;
}

int CUnit::Method_0x1C8()
{
    IndexedArrayHolder_6F3A1650* holder =
        (IndexedArrayHolder_6F3A1650*)g_unk6FAB65F4;
    unsigned int localSlot = holder->m_index;

    for (unsigned int i = 0; i < kPlayerRecordCount; i++)
    {
        SPlayerMembershipRecord* record =
            (SPlayerMembershipRecord*)holder->At(i);
        if (record == 0)
            continue;

        //  Fetched inside the loop, not hoisted: the shipped body calls
        //  At() a second time on every iteration.
        if (record == (SPlayerMembershipRecord*)holder->At(holder->m_index))
            continue;

        //  The agent in a named local, not chained into the field read:
        //  chained, MSVC forms the field's address with `mov`/`add` where
        //  the shipped code has one `lea` - the same effect
        //  jassgamestatemasks.cpp's DetectedMask records.
        SJassRelationAgent* agent =
            ((SJassPlayerRelations*)record)->EnsureRelationAgent();
        unsigned int mask = (unsigned int)agent->m_player.QueryField78();
        //  One instruction is not reproducible from either spelling: the
        //  shipped `test` has the shifted bit first and the mask second,
        //  and writing the `&` either way round leaves MSVC emitting
        //  `test mask, bit`.  Measured both ways.
        if ((mask & (1u << localSlot)) == 0)
            continue;

        if (record->m_pMembers->Contains(this) != 0)
            return 1;
    }
    return 0;
}
