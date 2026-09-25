//============================================================================
//  CUnit batch-16 (slot-101 closure).
//
//  0x6F3A3530 - CGameWorld::TestVisibleRelationBit.  The exact same shape
//  as CGameWorld::TestRelationBit (gameuiplayerenum.cpp), against
//  SJassRelationAgent::m_visible (+0x98) rather than m_player (+0x38).
//
//  0x6F3A35C0 - CGameWorld::TestDetectionRelationBit.  True if the
//  player's "detected" mask (+0xA8) has the bit, or the "visible" mask
//  does *and* the world's own +0x30 object flags a discovery bit
//  (0x10000 at its own +0x38).
//
//  Own translation unit, one module over from TestRelationBit itself, for
//  the same reason TestRelationBitAgainstOwner/TestRelationBitClear are
//  kept out of gameuiplayerenum.cpp: a call compiled in the same TU as
//  TestRelationBit's own definition gets inlined by /Ob2, and the shipped
//  stream's own `call sub_6F3A1650`/`call sub_6F41B420` here says that did
//  not happen in the real build either.
//============================================================================
#include "gameui.h"
#include "jassrelationagent.h"

struct IndexedArrayHolder_6F3A1650
{
    char m_reserved00[0x28];
    int At(unsigned int index);
};

int CGameWorld::TestVisibleRelationBit(unsigned int player, int bit)
{
    int record = ((IndexedArrayHolder_6F3A1650*)this)->At(player);

    SJassRelationAgent* agent =
        ((SJassPlayerRelations*)record)->EnsureRelationAgent();

    unsigned int mask = (unsigned int)agent->m_visible.QueryField78();

    return (mask & (1u << bit)) != 0;
}

int CGameWorld::TestDetectionRelationBit(unsigned int player, int bit)
{
    int record = ((IndexedArrayHolder_6F3A1650*)this)->At(player);
    SJassRelationAgent* agent =
        ((SJassPlayerRelations*)record)->EnsureRelationAgent();
    unsigned int wanted = 1u << bit;

    if (!((unsigned int)agent->m_detected.QueryField78() & wanted))
    {
        agent = ((SJassPlayerRelations*)record)->EnsureRelationAgent();
        if ((unsigned int)agent->m_visible.QueryField78() & wanted)
        {
            if (!((m_pField30->m_flags38 & 0x10000) > 0))
                return 0;
        }
        else
        {
            return 0;
        }
    }
    return 1;
}
