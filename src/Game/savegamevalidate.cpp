//============================================================================
//  0x6F5BE710 and 0x6F5BE740 - the two tests CSaveGame::ReadHeader ends in,
//  one per half of what it just read.  Both are "decode, then say whether
//  what came out is usable"; the save is only marked valid when both agree.
//
//  The slot test is the interesting one: every slot that says it is a player
//  must name a player, must not carry the +0x03 marker, and must not repeat
//  a player id an *earlier* slot already used.  It walks the table backwards
//  and rescans the prefix for each slot it reaches, which is why the outer
//  index counts down and the inner one up.
//============================================================================
#include "savegame.h"
#include "cgamedescstore.h"

//  0x6F655C40 - Game/gamesetupslotrecord.cpp.
int __fastcall GameSetupSlotsDecode(Net::GAMESETUP_SLOTS* out,
                                    const Net::GAMESETUP_SLOTRECORD* in);

//----------------------------------------------------------------------------
//  0x6F5BE710
//----------------------------------------------------------------------------
int __fastcall SaveGameHeaderUsable(Net::GAMEDESC* desc, const SSaveGameHeaderBlock* block)
{
    if (!GameDescDecode(desc, block))
        return 0;

    if (desc->m_playerCount > 2)
        return 0;

    //  A zero-seeded result, not an `&&`: the shipped tail is
    //  `xor eax,eax` / `cmp [..],al` / `je` / `cmp [..],al` / `setne al`,
    //  which is one branch and a setcc rather than two branches.
    int ok = 0;
    if (desc->m_mapPath[0] != 0)
        ok = desc->m_creator[0] != 0;
    return ok;
}

//----------------------------------------------------------------------------
//  0x6F5BE740
//----------------------------------------------------------------------------
int __fastcall SaveGameSlotsUsable(Net::GAMESETUP_SLOTS* slots,
                                   const Net::GAMESETUP_SLOTRECORD* record)
{
    if (!GameSetupSlotsDecode(slots, record))
        return 0;

    unsigned int count = slots->m_array.m_count;
    if (count == 0 || slots->m_field11 > count)
        return 0;

    Net::GAMESETUP_SLOT* table = slots->m_array.m_data;

    //  The outer walk decrements *first* and tests the counter at the
    //  bottom, which is what leaves the count itself as the loop variable
    //  and the slot pointer as a second one stepping back by nine.  Written
    //  as `while (i-- != 0)` the counter has to be copied before the
    //  decrement and the tail costs an extra instruction.
    unsigned int i = count;
    do
    {
        i--;
        if (table[i].m_state == 2)
        {
            unsigned char id = table[i].m_playerId;
            if (id == 0)
            {
                if (table[i].m_byte3 == 0)
                    return 0;
            }
            else
            {
                if (table[i].m_byte3 != 0)
                    return 0;

                for (unsigned int j = 0; j < i; j++)
                {
                    if (table[j].m_state == 2 && id == table[j].m_playerId)
                        return 0;
                }
            }
        }
    }
    while (i != 0);

    return 1;
}
