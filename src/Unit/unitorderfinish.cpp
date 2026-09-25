//============================================================================
//  0x6F2964E0 - CUnit::FinishCurrentOrder: the order the unit has just
//  finished is announced, a CTaskAction is made to carry the news, and the
//  whole task chain at +0x174 is torn down.
//
//  In the shipped order:
//
//    * announce kEventOrderFinished with no subject;
//    * make a 'tskA' agent out of the game-wide agile type registry, the
//      same eight steps MakeOrderAgent takes for '+ord' (unitordermake.cpp)
//      and through the same 0x2C-byte request block, parked in a scoped
//      SCheckedTaskActionSlot;
//    * fill it in - CTaskActionFields::Init(0, this) - and announce a second
//      event carrying the task's own id (+0x30) and the task itself;
//    * release the task through its slot 23;
//    * unlink the task chain head at +0x174 before walking it, so that
//      nothing a release runs can find it, then release every task in the
//      chain and flush the pending controller ramp.
//
//  The scoped slot puts the whole body inside the same unreproducible
//  __except_handler4-shaped frame every destructible local in this family
//  carries; the two stack CEvents add two more unwind states to it.  See
//  docs/targets/JASS_Location.md, "The ceiling".
//
//  Own translation unit: six real calls out of it.
//============================================================================
#include "unit.h"
#include "unitorder.h"
#include "agent.h"
#include "agenttypedslots.h"
#include "agiletype.h"
#include "widgetagentquery.h"
#include "game.h"

//  0x6F271940 - 'tskA' (misc_rawcode_getters.cpp).
unsigned int GetTaskActionAgileTypeId();

//  What the factory hands back; only the CAgent* at +0x54 is reached.
struct SMadeTaskAgent
{
    char    m_reserved00[0x54];
    CAgent* m_pAgent;   // +0x54
};

void CUnit::FinishCurrentOrder()
{
    SOrderEvent finished;
    finished.m_reserved04 = 0;
    finished.m_vtable = &g_vftCEvent;
    finished.m_msgId = kEventOrderFinished;
    finished.m_pOrder = 0;
    ((PostAgentEventFn)(*(void***)this)[0x10 / 4])(this, &finished);

    SCheckedTaskActionSlot held(0);

    unsigned int typeId = GetTaskActionAgileTypeId();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, typeId, type->m_allocator);
    //  Stored again after Init has already stored it, the same way every
    //  other factory in this family does.
    query.m_reserved24 = 0xFFFFFFFF;

    held.Assign(((SMadeTaskAgent*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    CTaskActionFields* task = (CTaskActionFields*)held.m_value;
    task->Init(0, this);

    task = (CTaskActionFields*)held.m_value;

    SOrderEvent started;
    started.m_reserved04 = 0;
    started.m_vtable = &g_vftCEvent;
    started.m_msgId = (unsigned int)task->m_orderId;
    started.m_pOrder = task;
    ((PostAgentEventFn)(*(void***)this)[0x10 / 4])(this, &started);

    ((OrderReleaseFn)(*(void***)held.m_value)[0x5C / 4])(held.m_value);

    SOptionalHandleRef* chain = (SOptionalHandleRef*)&m_ref174;
    if ((chain->m_typeTag & (int)chain->m_handle) != -1)
    {
        CTaskActionFields* pending = (CTaskActionFields*)QueryHandleField0x54(chain);
        if (pending != 0)
        {
            //  Cut the chain loose before releasing anything on it: a
            //  release is free to look at +0x174 and must not find a task
            //  that is already going away.
            chain->m_typeTag = -1;
            chain->m_handle = 0xFFFFFFFF;

            do
            {
                CTaskActionFields* next =
                    (CTaskActionFields*)ResolveHandleRefOrNull(&pending->m_next);
                ((OrderReleaseFn)(*(void***)pending)[0x5C / 4])(pending);
                pending = next;
            }
            while (pending != 0);

            FlushPendingRamp();
        }
    }

    started.m_vtable = &g_vftTRefCnt;
}
