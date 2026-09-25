//============================================================================
//  0x6F3D0310 - SCheckedTextAgentSlot::MakeTextAgent: build a fresh '+tex'
//  (floating-text-agent-tagged) agent and park it in this slot.
//
//  Byte-for-byte the same five-line opening SCheckedRectSlot::MakeRect
//  (jassrectmake.cpp) has - the type id into the game-wide agile type
//  registry, that record's own allocator as the pool, the pool and the type
//  into the 0x2C-byte request block, and the CAgent* the returned holder
//  carries at +0x54.  The type-id leaf (GetTextAgentTypeId) is called
//  *twice*, once for the pool lookup and once to fill the block in - the
//  shipped code, not a simplification.
//
//  Like MakeRect, the block's +0x24 word is derived from `kind`: -1
//  normally, -2 when `kind` is 2.  The other two arguments are never read;
//  ExecuteFunc's own call site (jassruntimeexecutefunc.cpp) always passes
//  all three as 0.
//
//  Own translation unit in the same style as jassrectmake.cpp: it issues a
//  genuine call to SCheckedTextAgentSlot::Assign (textagenttypedassign.cpp).
//============================================================================
#include "agenttypedslots.h"
#include "agiletype.h"
#include "widgetagentquery.h"
#include "game.h"

//  0x6F443C60 - '+tex' (GameUI/gameuiagenttypetag.cpp).
unsigned int GetTextAgentTypeId();

//  What the factory hands back; only the CAgent* at +0x54 is reached.
struct SMadeTextAgent
{
    char    m_reserved00[0x54];
    CAgent* m_pAgent;   // +0x54
};

SCheckedTextAgentSlot* SCheckedTextAgentSlot::MakeTextAgent(int, int, int kind)
{
    AGILE_TYPE_ID id(GetTextAgentTypeId());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetTextAgentTypeId(), type->m_allocator);
    query.m_reserved24 = 0xFFFFFFFF - (unsigned int)(kind == 2);

    //  Assign already answers `this`, but the shipped code still issues its
    //  own `mov eax,esi` afterwards, so the return is spelled separately.
    Assign(((SMadeTextAgent*)
            SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);
    return this;
}
