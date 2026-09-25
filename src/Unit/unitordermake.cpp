//============================================================================
//  0x6F294A40 - MakeOrderAgent: build one unit-order object and hand it back.
//
//  The order twin of the `Location` native's own factory
//  (0x6F3D03B0, jassnatives_locationmake.cpp): look '+ord' up in the
//  game-wide agile type registry, take that type record's own allocator,
//  hand pool and type to the global agent factory through the same
//  0x2C-byte request block, park the CAgent* the returned holder carries at
//  +0x54 in a type-checked scoped slot, fill the new object's own three
//  fields in, and return it.
//
//  Two things differ from the location factory and both are visible in the
//  stream: the type id is fetched *once* and kept in a register (the
//  location one calls its leaf twice), and the result is the agent itself
//  rather than a registry token, so nothing is registered here.
//
//  __fastcall, not a member: the order id arrives in ecx as a plain
//  immediate at the one call site in this tree (`mov ecx, 0D0004h`), which
//  no `this` ever is.
//
//  0xD0004 is the JASS order id 851972, "stop" - which is what
//  CUnit::IssueStopOrder (unitstoporder.cpp) uses it for.
//
//  The scoped slot puts the whole body inside the same unreproducible
//  __except_handler4-shaped frame every destructible local in this family
//  carries; see docs/targets/JASS_Location.md, "The ceiling".
//
//  Own translation unit: six real calls out of it.
//============================================================================
#include "agent.h"
#include "agenttypedslots.h"
#include "agiletype.h"
#include "widgetagentquery.h"
#include "itemhandleresolve.h"
#include "game.h"

//  0x6F2712B0 - '+ord' (misc_rawcode_getters.cpp).
unsigned int GetOrderAgileTypeId();

//  0x6F0419C0 - SOptionalHandleRefResolver::ResolveChained, already
//  reconstructed (handlereref_resolve.cpp); redeclared here the way
//  agentregistrysingleton.cpp redeclares it, so the mangled name is the
//  same symbol and not a second one.
struct SOptionalHandleRefResolver : SOptionalHandleRef
{
    SOptionalHandleRef& ResolveChained(const void* ref);
};

//  What the factory hands back: only the CAgent* at +0x54 is reached, the
//  same holder shape jassnatives_locationmake.cpp names for its own made
//  object.
struct SMadeOrderAgent
{
    char    m_reserved00[0x54];
    CAgent* m_pAgent;   // +0x54
};

//  The made object, in the three fields this function writes: the order id,
//  whatever the order is aimed at, and the chained handle reference the
//  resolver fills in.
struct SUnitOrder
{
    char m_reserved00[0x24];
    int  m_orderId;     // +0x24
    int  m_target;      // +0x28
    char m_reserved2C[0x0C];
    SOptionalHandleRef m_sourceRef;   // +0x38
};

CAgent* __fastcall MakeOrderAgent(int orderId, int target, const void* sourceRef)
{
    //  Constructed empty and assigned in one step, which is what the
    //  0x6F03B620 constructor is (checkedorderslot.cpp).
    SCheckedOrderSlot held(0);

    unsigned int typeId = GetOrderAgileTypeId();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, typeId, type->m_allocator);
    //  Stored again after Init has already stored it: the shipped code has
    //  both stores, and dropping this one loses an instruction.
    query.m_reserved24 = 0xFFFFFFFF;

    held.Assign(((SMadeOrderAgent*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    SUnitOrder* order = (SUnitOrder*)held.m_value;
    order->m_orderId = orderId;
    order->m_target = target;
    ((SOptionalHandleRefResolver*)&order->m_sourceRef)->ResolveChained(sourceRef);

    return held.m_value;
}
