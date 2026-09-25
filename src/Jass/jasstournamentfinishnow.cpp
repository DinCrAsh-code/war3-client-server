//============================================================================
//  0x6F3C1ED0 / 0x6F3C1F00 - GetTournamentFinishNowRule / Player.  Both
//  live for the single id 0x80302, both read the current event data's own
//  +0x20 word (see jassevents.h) - Rule as a plain int, Player as a player
//  index into the world player table.  Own translation unit, right after
//  the dialog module and calling nothing else in this batch.
//============================================================================
#include "jassevents.h"
#include "itemhandlemain.h"
#include "agentregistry.h"

//  0x6F0418A0 - IsAgentHandleLive (Agent/agenthandlelive.cpp).  Declared
//  locally the way gameuithunks.h's own copy is, rather than pulling in
//  GameUI/'s whole header for one free function.
int __fastcall IsAgentHandleLive(const void* agent);

//  0x6F3A1650 - IndexedArrayHolder_6F3A1650::At, already reconstructed and
//  EXACT (Misc/misc_field_getters.cpp).  Declared locally, the way every
//  other caller of it in this repo does - see gameuiplayername.cpp's own
//  header comment for why a second naked redirect would be the wrong move.
struct IndexedArrayHolder_6F3A1650
{
    char pad[0x58];
    int  m_data[1];
    int  At(unsigned int index);
};

//----------------------------------------------------------------------------
//  0x6F3C1ED0 - the rule id itself, a plain field read.
//----------------------------------------------------------------------------
int GetTournamentFinishNowRule()
{
    if ((unsigned int)(GetCurrentEventId() + kEventIdBias) != 0x80302)
        return 0;

    if (!GetCurrentScriptEvent())
        return 0;

    CScriptEventData* data = GetCurrentScriptEventData();
    return (int)data->m_ownerRef.m_handle;
}

//----------------------------------------------------------------------------
//  0x6F3C1F00 - the +0x24 half of the same field, read as a player index
//  into the world player table (IndexedArrayHolder_6F3A1650::At, the same
//  member misc_field_getters.cpp already reconstructs), validated live
//  through IsAgentHandleLive before being registered as an agent.
//----------------------------------------------------------------------------
void* GetTournamentFinishNowPlayer()
{
    if ((unsigned int)(GetCurrentEventId() + kEventIdBias) != 0x80302)
        return 0;

    if (!GetCurrentScriptEvent())
        return 0;

    CScriptEventData* data = GetCurrentScriptEventData();
    unsigned int playerIndex = (unsigned int)data->m_ownerRef.m_typeTag;

    void* player =
        (void*)((IndexedArrayHolder_6F3A1650*)g_unk6FAB65F4)->At(playerIndex);

    if (!player || !IsAgentHandleLive(player))
        return 0;

    CAgentRegistry* registry = (CAgentRegistry*)g_pItemSlotHost->GetSlotTable();
    return (void*)registry->Register((CUnitAgent*)player, 0);
}
