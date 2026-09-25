//============================================================================
//  SWorldVisibilityMaskState::RefreshPlayerWorldVisibilityMasks
//  (0x6F408070) - one of SetPlayerAlliance's own closure
//  (Jass/jasssetplayeralliance.cpp), called after every alliance change and
//  unconditionally by CPlayerWar3FinishLeaveNotify's own recompute
//  (Player/playerwar3_handleplayerleftnotify.cpp - the "sub_6F408070" this
//  batch's own worklist names as one of that function's still-thunked
//  callees).  `this` is the single world-global object; the *local player's*
//  own per-player record (a second, different object, jassrelationagent.h's
//  SJassPlayerRelations) is what m_fieldF0's QueryField78 below is actually
//  read off.
//============================================================================
#include "jassrelationagent.h"

//----------------------------------------------------------------------------
//  sub_6F3A1650 - see Misc/misc_field_getters.cpp's own comment; redeclared
//  locally, as every other user of it does.
//----------------------------------------------------------------------------
struct IndexedArrayHolder_6F3A1650
{
    char           pad[0x28];
    unsigned short m_index;   // +0x28 - the local player's slot
    char           pad2[0x2E];
    int            m_data[1];
    int  At(unsigned int index);
};

//  Same record CUnit::Method_0x18C and Unit/unit_ownerworldmask.cpp already
//  name; redeclared locally for the one field this function reads.
struct SPlayerWorldMaskRecord
{
    char           m_reserved000[0x2E0];
    unsigned short m_mask2E0;   // +0x2E0
};

//----------------------------------------------------------------------------
//  0x6F408070 - `retn 0`, `this` in ecx.
//----------------------------------------------------------------------------
void SWorldVisibilityMaskState::RefreshPlayerWorldVisibilityMasks()
{
    IndexedArrayHolder_6F3A1650* holder = (IndexedArrayHolder_6F3A1650*)g_unk6FAB65F4;

    void* localRecord = (void*)holder->At(holder->m_index);

    if (m_field24 & 1)
    {
        m_field3C = 0xFFFF;
    }
    else if (((SJassPlayerRelations*)localRecord)->m_fieldF0.QueryField78())
    {
        m_field3C = 0xFFF;
    }
    else
    {
        m_field3C = (unsigned short)(1 << ((SJassPlayer*)localRecord)->m_playerIndex);
    }

    unsigned int mask = ((SPlayerWorldMaskRecord*)localRecord)->m_mask2E0;

    m_field3E = (unsigned short)mask;
    m_field40 = 0;

    for (unsigned int i = 0; mask; ++i, mask >>= 1)
    {
        if (mask & 1)
        {
            SJassPlayer* other = (SJassPlayer*)holder->At(i);
            m_field40 |= ((SPlayerWorldMaskRecord*)other)->m_mask2E0;
        }
    }
}
