//============================================================================
//  NotifyPlayerControllerChanged (0x6F416700) - re-evaluate a player's own
//  +0x20 flags bit 2 whenever its controller changes (SetPlayerController,
//  jassnatives_player.cpp).
//
//  The player IS its own SJassPlayerRelations view (jassrelationagent.h -
//  the same object, a different set of named fields), and the ally-relation
//  agent it caches at +0x38 (SCheckedAllySlot::m_value,
//  Agent/agenttypedslots.h) is read raw here rather than lazily created:
//  this call site trusts it already exists.
//============================================================================
#include "jassnatives.h"
#include "jassrelationagent.h"

//----------------------------------------------------------------------------
//  sub_6F3A1650 - see Misc/misc_field_getters.cpp's own comment; redeclared
//  locally, as every other user of it does.
//----------------------------------------------------------------------------
struct IndexedArrayHolder_6F3A1650
{
    char pad[0x58];
    int  m_data[1];
    int  At(unsigned int index);
};

//  misc_handle_lookups.cpp - the +0xC8-offset sibling of QueryField78,
//  reached only through sub_6F41B420's own (still-unidentified-by-class)
//  return value, exactly as it is here.
int __fastcall QueryField78At0xC8(void*);

//----------------------------------------------------------------------------
//  0x6F416700 - `retn 0`, `this` (the player) in ecx.
//
//  The relation agent's own +0x88 and +0xC8 handle-ref fields (the second
//  reached through QueryField78At0xC8's own fixed adjustor) are OR'd into
//  one mask; every player slot that mask selects has to have a non-zero
//  controller for the flag to end up set - the loop is written as MSVC's
//  own "stay true until one player fails the test, or the players run out"
//  shape, `edi` starting true so a completely empty mask sets the flag too.
//----------------------------------------------------------------------------
void __fastcall NotifyPlayerControllerChanged(SJassPlayer* player)
{
    void* gameState = g_unk6FAB65F4;
    SJassPlayerRelations* record = (SJassPlayerRelations*)player;
    SJassRelationAgent* agent = (SJassRelationAgent*)record->m_allySlot.m_value;

    int mask = agent->m_reserved88.QueryField78() | QueryField78At0xC8(agent);

    int allControlled = 1;
    int i = 0;

    while (allControlled)
    {
        if (mask & (1 << i))
        {
            SJassPlayer* other = (SJassPlayer*)
                ((IndexedArrayHolder_6F3A1650*)gameState)->At(i);
            allControlled = (other->m_controller != 0);
        }

        if (++i >= 0x0C)
            break;
    }

    if (allControlled)
        player->m_field20 |= 2;
    else
        player->m_field20 &= ~2;
}
