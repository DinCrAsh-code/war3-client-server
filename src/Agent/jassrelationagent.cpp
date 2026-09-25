//============================================================================
//  0x6F41B420 - the JASS game-state singleton's per-player relation agent,
//  made on first use and kept in the player record's own 'ally'-typed slot.
//
//  Every one of the seven owner-relationship masks in
//  jassgamestatemasks.cpp goes through here, which is why it is worth its
//  own translation unit: they call it for real, so it must not be inlined
//  into any of them.
//
//  The body is the same make-an-agent sequence CWidget::ClearPlayerMaskBits
//  (widget_slot68.cpp) and CAgent::ReleaseSelf (agent_releaseself.cpp)
//  already have, and it is written the same way for the same reasons - the
//  named AGILE_TYPE_ID lvalue whose address is taken twice, the singleton
//  read into its own local and held across Hash() in a callee-saved
//  register, the hash in a local so it is computed before the argument
//  pushes rather than between them.  See agent_releaseself.cpp for the
//  full argument; none of it is re-derived here.
//
//  Two things are this function's own:
//
//  * **`GetAllyFourCC()` is called twice**, once for the pool lookup and
//    once for the query block, rather than kept in a register.  Same as
//    slot 68's two calls to GetWidgetAgileTypeId, and the reason that one
//    is out of line at all.
//  * **there is no null check on the type record.**  The shipped code goes
//    straight from `Ptr` into `[eax+70h]`; ReleaseSelf's own "report and
//    carry on" arm has no counterpart here.
//============================================================================
#include "jassrelationagent.h"
#include "agiletype.h"
#include "agent.h"
#include "widgetagentquery.h"
#include "game.h"

//  0x6F3D9E30 - 'ally' (misc_rawcode_getters.cpp).  Declared, not defined:
//  the shipped code reaches it with two real calls.
unsigned int GetAllyFourCC();

//  What SubmitWidgetAgentQuery hands back - a holder whose +0x54 is the
//  CAgent the slot actually takes.  Same shape widget_slot68.cpp names.
struct SMadeAgentHolder
{
    char    m_reserved00[0x54];
    CAgent* m_pAgent;   // +0x54
};

SJassRelationAgent* SJassPlayerRelations::EnsureRelationAgent()
{
    if (m_allySlot.m_value == 0)
    {
        AGILE_TYPE_ID id(GetAllyFourCC());
        CGameData* game = g_pGameData;
        unsigned int hash = id.Hash();
        AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
        void* pool = type->m_allocator;

        SWidgetAgentQuery query;
        InitWidgetAgentQuery(&query, GetAllyFourCC(), pool);
        //  Written again over what InitWidgetAgentQuery has just put there.
        //  The shipped code really does store -1 into the block's +0x24 a
        //  second time, between the two calls; it is not a fold of Init's
        //  own store, which lives in another function this one calls.
        query.m_reserved24 = 0xFFFFFFFF;
        m_allySlot.Assign(((SMadeAgentHolder*)
                           SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);
    }
    //  Re-read from the slot rather than returned out of the branch: the
    //  shipped code keeps `&m_allySlot` in esi across the whole body and
    //  ends on one shared `mov eax, [esi]`, which both arms reach.
    return (SJassRelationAgent*)m_allySlot.m_value;
}
