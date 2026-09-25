//============================================================================
//  0x6F294B30 - MakeOrderPointAgent: build one COrderPoint and hand it back.
//
//  The point twin of MakeOrderAgent (unitordermake.cpp), and the same eight
//  steps in the same order: look 'ord.' up in the game-wide agile type
//  registry, take that type record's own allocator, hand pool and type to
//  the global agent factory through the same 0x2C-byte request block, park
//  the CAgent* the returned holder carries at +0x54 in a type-checked
//  scoped slot, fill the new order in through COrder::Init, and return it.
//
//  __fastcall, not a member: the order id arrives in ecx and the owning
//  player in edx, and its one call site (0x6F3B3090) has both in registers
//  the JASS native put there.
//
//  The scoped slot puts the whole body inside the same unreproducible
//  __except_handler4-shaped frame every destructible local in this family
//  carries; see docs/targets/JASS_Location.md, "The ceiling".
//
//  Own translation unit: five real calls out of it.
//============================================================================
#include "agent.h"
#include "agenttypedslots.h"
#include "agiletype.h"
#include "widgetagentquery.h"
#include "unitorder.h"
#include "game.h"

//  0x6F271330 - 'ord.' (misc_rawcode_getters.cpp).
unsigned int GetOrderPointAgileTypeId();

//  What the factory hands back: only the CAgent* at +0x54 is reached, the
//  same holder shape unitordermake.cpp names for its own made object.
struct SMadeOrderPointAgent
{
    char    m_reserved00[0x54];
    CAgent* m_pAgent;   // +0x54
};

CAgent* __fastcall MakeOrderPointAgent(int orderId, int player,
                                       const void* sourceRef, const void* x,
                                       const void* y, void* owner)
{
    //  Constructed empty and assigned in one step, which is what the
    //  0x6F03C110 constructor is (checkedorderpointslot.cpp).
    SCheckedOrderPointSlot held(0);

    unsigned int typeId = GetOrderPointAgileTypeId();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, typeId, type->m_allocator);
    //  Stored again after Init has already stored it: the shipped code has
    //  both stores, and dropping this one loses an instruction.
    query.m_reserved24 = 0xFFFFFFFF;

    held.Assign(((SMadeOrderPointAgent*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    ((COrder*)held.m_value)->Init(orderId, player, sourceRef, x, y,
                                  (CAgent*)owner);

    return held.m_value;
}
