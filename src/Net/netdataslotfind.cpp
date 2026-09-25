//============================================================================
//  0x6F53E050 - CNetData::SLOTTABLE::Find.
//
//  Twelve callers in the same module (0x6F53E080 through 0x6F54FD50) hand it
//  a slot byte and take back a participant record.  The byte's top bit is a
//  namespace selector, not part of the index: set picks the referee/observer
//  array at +0x3C/+0x40 and the remaining seven bits index it, clear picks
//  the player array at +0x2C/+0x30 and the whole byte indexes that.
//
//  `xor eax,eax` sits at the top and both "no such slot" answers fall into
//  one shared `retn`, so the null is one initialisation and not two returns;
//  the referee arm's own hit gets its own `retn` because that is the branch
//  the shipped code lays out first.
//
//  Own translation unit: every one of those twelve callers issues a real
//  `call`.
//============================================================================
#include "netdata.h"

CNetData::SLOT* CNetData::SLOTTABLE::Find(unsigned char slot)
{
    SLOT* found = 0;

    //  The masking is done *on the byte* and only then widened - `and dl,7Fh`
    //  / `movzx edx,dl` - which is what keeps the slot in dl across the sign
    //  test; naming a widened `unsigned int index` first costs the `movzx`
    //  its place at the top of the function.
    if ((signed char)slot < 0)
    {
        slot &= 0x7F;
        if (slot < m_refereeCount)
            return m_referees[slot];
    }
    else
    {
        if (slot < m_playerCount)
            found = m_players[slot];
    }

    return found;
}
