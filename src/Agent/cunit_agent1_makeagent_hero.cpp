//============================================================================
//  MakeCheckedAgent_AHer (0x6F29B490) - the Hero instantiation of the same
//  38-address family cunit_agent1_makeagent.cpp documents in full (see that
//  file's header comment for the diff methodology and the unreproducible
//  __except_handler4-shaped SEH frame).  Its own file because it is one
//  address away from the other 37 (0x29xxxx vs 0x238000-0x24Cxxx) and uses
//  the separately-named HeroAgileTypeId rather than a fresh GetFourCC leaf.
//============================================================================
#include "widgetagentquery.h"
#include "agiletype.h"
#include "cunit_agent1_agentptr_hero_ral.h"
#include "playerheroeval.h"   // HeroAgileTypeId - 0x6F2075E0
#include "game.h"

//  What the maker hands back: only the CAgent* at +0x54 is reached (same
//  local shape as widget_slot68.cpp's own SMadeAgentHolder).
struct SMadeAgentHolder
{
    char    m_reserved00[0x54];
    CAgent* m_pAgent;   // +0x54
};

//  0x6F29B490
CAgent* MakeCheckedAgent_AHer()
{
    CAgentPtr_AHer held(0);

    AGILE_TYPE_ID id(HeroAgileTypeId());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, HeroAgileTypeId(), pool);

    held.AssignChecked(((SMadeAgentHolder*)
                        SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_ptr;
}
