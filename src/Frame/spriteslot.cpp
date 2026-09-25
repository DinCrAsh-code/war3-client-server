//============================================================================
//  0x6F60FC50 - SSpriteSlot::Init.  Eleven instructions, no calls: the one
//  argument goes to +0x00, everything else is cleared and +0x10 starts at
//  0x40.  Not a constructor in the shipped build - a plain `retn 4` member
//  the CSpriteFrame constructor calls after its base is up - which is why it
//  is spelled as one here.
//============================================================================
#include "frame.h"
#include "framethunks.h"


SSpriteSlot* SSpriteSlot::Init(int owner)
{
    m_owner = owner;
    m_04 = 0;
    m_08 = 0;
    m_0C = 0;
    m_14 = 0;
    m_18 = 0;
    m_10 = 0x40;
    return this;
}
