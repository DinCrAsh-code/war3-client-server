//============================================================================
//  0x6F294D40 - MakeOrderTargetAgent: build one COrderTarget and hand it back.
//
//  The targeted twin of MakeOrderAgent (unitordermake.cpp), and the same eight
//  steps in the same order: look 'ordt' up in the game-wide agile type
//  registry, take that type record's own allocator, hand pool and type to
//  the global agent factory through the same 0x2C-byte request block, park
//  the CAgent* the returned holder carries at +0x54 in a type-checked
//  scoped slot, fill the new order in through COrderTarget::Init, and return it.
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

//  0x6F271430 - 'ordt' (misc_rawcode_getters.cpp).
unsigned int GetOrderTargetAgileTypeId();

//  What the factory hands back: only the CAgent* at +0x54 is reached, the
//  same holder shape unitordermake.cpp names for its own made object.
struct SMadeOrderTargetAgent
{
    char    m_reserved00[0x54];
    CAgent* m_pAgent;   // +0x54
};

CAgent* __fastcall MakeOrderTargetAgent(int orderId, int player,
                                        const void* sourceRef, void* target,
                                        const void* x, const void* y,
                                        void* owner, void* tracker)
{
    //  Constructed empty and assigned in one step, which is what the
    //  0x6F0388F0 constructor is (checkedordertargetslot.cpp).
    SCheckedOrderTargetSlot held(0);

    unsigned int typeId = GetOrderTargetAgileTypeId();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, typeId, type->m_allocator);
    //  Stored again after Init has already stored it: the shipped code has
    //  both stores, and dropping this one loses an instruction.
    query.m_reserved24 = 0xFFFFFFFF;

    held.Assign(((SMadeOrderTargetAgent*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    ((COrderTarget*)held.m_value)->Init(orderId, player, sourceRef, target,
                                        x, y, (CAgent*)owner,
                                        (CAgent*)tracker);

    return held.m_value;
}
