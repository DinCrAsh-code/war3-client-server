//============================================================================
//  Four more members of the "type-checked smart pointer to a CAgent-derived
//  object" family (agenttypedslots.h) reached from CUnit's own vtable-
//  closure walk - cunit_vtable_closure_worklist.json, CUnit batch-26.  Two
//  converting constructors for two of batch-25's four address-derived slots
//  (checkedslot_batch25.cpp), and a "MakeXXX" factory for the other two -
//  the same shape SCheckedRectSlot::MakeRect has (jassrectmake.cpp) and
//  SCheckedQumSlot's own registry-singleton finish step has
//  (agentregistrysingleton.cpp): look the type up in the game-wide agile
//  type registry, build a request block from its allocator, submit it, and
//  Assign the maker's answer into this slot.
//
//      0x6F293010  SCheckedSlot_6F2700B0::Make  (retn 0Ch - int,int,kind)
//      0x6F293090  SCheckedSlot_6F270130::Make  (retn 0Ch - int,int,kind)
//      0x6F412E30  SCheckedSlot_6F40BD90::SCheckedSlot_6F40BD90(CAgent*)
//      0x6F412E90  SCheckedSlot_6F40BE10::SCheckedSlot_6F40BE10(CAgent*)
//
//  Unlike SCheckedRectSlot::MakeRect, which calls its fixed-typeid getter
//  twice (once for the registry lookup, once again to fill the request
//  block in - the shipped code there, not a choice made here), both Make
//  bodies here call their getter exactly once and keep the result in a
//  register for both uses - the shipped stream has one `call` to the
//  getter, not two, so the local variable is not a stylistic choice, it is
//  what makes the single call happen.
//============================================================================
#include "checkedslot_batch2526.h"
#include "agiletype.h"
#include "widgetagentquery.h"
#include "game.h"
#include <excpt.h>   // AbnormalTermination(), for the two constructors' __finally

unsigned int GetFixedRawcode_6F2700B0();
unsigned int GetFixedRawcode_6F270130();

//  What the maker hands back; only the CAgent* at +0x54 is reached - the
//  same shape every other member of the family reads through (jassrectmake
//  .cpp, agentregistrysingleton.cpp, item_createatposition.cpp, ...).
struct SMadeAgent_Batch26
{
    char    m_reserved00[0x54];
    CAgent* m_pAgent;   // +0x54
};

//----------------------------------------------------------------------------
//  0x6F293010.
//----------------------------------------------------------------------------
SCheckedSlot_6F2700B0* SCheckedSlot_6F2700B0::Make(int, int, int kind)
{
    CGameData* game = g_pGameData;
    unsigned int rawcode = GetFixedRawcode_6F2700B0();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, rawcode, type->m_allocator);
    query.m_reserved24 = 0xFFFFFFFF - (unsigned int)(kind == 2);

    Assign(((SMadeAgent_Batch26*)
            SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);
    return this;
}

//----------------------------------------------------------------------------
//  0x6F293090.
//----------------------------------------------------------------------------
SCheckedSlot_6F270130* SCheckedSlot_6F270130::Make(int, int, int kind)
{
    CGameData* game = g_pGameData;
    unsigned int rawcode = GetFixedRawcode_6F270130();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, rawcode, type->m_allocator);
    query.m_reserved24 = 0xFFFFFFFF - (unsigned int)(kind == 2);

    Assign(((SMadeAgent_Batch26*)
            SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);
    return this;
}

//----------------------------------------------------------------------------
//  0x6F412E30.  The same __try/__finally every scoped member of the family
//  has (agenttypedslots.h) - Assign's virtual GetAgileTypeId() call through
//  `candidate` is the one thing here that can raise, and the frame releases
//  whatever half-assigned value is left if it does.
//----------------------------------------------------------------------------
SCheckedSlot_6F40BD90::SCheckedSlot_6F40BD90(CAgent* candidate)
{
    m_value = 0;
    __try
    {
        Assign(candidate);
    }
    __finally
    {
        if (AbnormalTermination())
            Release();
    }
}

//----------------------------------------------------------------------------
//  0x6F412E90.
//----------------------------------------------------------------------------
SCheckedSlot_6F40BE10::SCheckedSlot_6F40BE10(CAgent* candidate)
{
    m_value = 0;
    __try
    {
        Assign(candidate);
    }
    __finally
    {
        if (AbnormalTermination())
            Release();
    }
}
