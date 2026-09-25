//============================================================================
//  0x6F3CF490 - SCheckedRectSlot::MakeRect: build a fresh JASS `rect` agent
//  and park it in this slot.
//
//  The same five-line opening JASS_Location has (jassnatives_locationmake.cpp)
//  and CWidget::ClearPlayerMaskBits before it: the type id into the game-wide
//  agile type registry, that record's own allocator as the pool, the pool and
//  the type into the 0x2C-byte request block, and the CAgent* the returned
//  holder carries at +0x54.  The type-id leaf is called *twice*, once for the
//  pool lookup and once to fill the block in, which is the shipped code.
//
//  The one thing this does that the location factory does not is derive the
//  block's +0x24 word from its argument: -1 normally, -2 when `kind` is 2.
//  The other two arguments are never read - both call sites pass zero.
//
//  Own translation unit in the 0x6F3CFxxx module: it issues a genuine call
//  to SCheckedRectSlot::Assign (recttypedassign.cpp).
//============================================================================
#include "agenttypedslots.h"
#include "agiletype.h"
#include "widgetagentquery.h"
#include "game.h"

//  0x6F3DB8A0 - '+rct' (jassagiletypeids.cpp).
unsigned int GetRectAgileTypeId();

//  What the factory hands back; only the CAgent* at +0x54 is reached.
struct SMadeRectAgent
{
    char    m_reserved00[0x54];
    CAgent* m_pAgent;   // +0x54
};

SCheckedRectSlot* SCheckedRectSlot::MakeRect(int, int, int kind)
{
    AGILE_TYPE_ID id(GetRectAgileTypeId());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetRectAgileTypeId(), type->m_allocator);
    query.m_reserved24 = 0xFFFFFFFF - (unsigned int)(kind == 2);

    //  Assign already answers `this`, but the shipped code still issues its
    //  own `mov eax,esi` afterwards, so the return is spelled separately.
    Assign(((SMadeRectAgent*)
            SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);
    return this;
}
