//============================================================================
//  cunit_agent3 batch - thirty-eight "make one ability-effect agent"
//  factories, CUnit's own vtable closure BFS depth 1-2.
//
//  Every one is the same five real calls widget_slot68.cpp's own opening
//  (CWidget::ClearPlayerMaskBits) and jassnatives_locationmake.cpp's own
//  JASS_Location already document for their own agile types: look the
//  ability's FourCC up in the game-wide agile type registry, take that
//  type record's own allocator, hand pool and type to the global agent
//  factory through the same 0x2C-byte request block, park the CAgent* the
//  returned holder carries at +0x54 in a type-checked scoped slot, and
//  return it.  No fields are written on the made object here (that is
//  MakeOrderAgent's own extra step, not this family's).
//
//  __cdecl, zero arguments: every dump in this family is a bare `retn` with
//  no locals coming from the caller.
//
//  The type-checked slot is a scoped local, so the whole body sits inside
//  the same unreproducible __except_handler4-shaped frame every
//  destructible local in this family carries - the only reason none of
//  these can reach 1.000.
//============================================================================
#include "cunit_agent3_checkedslots.h"
#include "cunit_agent3_abilityids.h"
#include "agiletype.h"
#include "widgetagentquery.h"
#include "game.h"

//  0x6F027BD0 - already GetFourCC_6F027BD0 (misc_trivial_getters.cpp),
//  not redefined in this batch; see cunit_agent3_abilityids.h.
extern unsigned int GetFourCC_6F027BD0();

//  What the factory hands back: only the CAgent* at +0x54 is reached, the
//  same holder shape widget_slot68.cpp and jassnatives_locationmake.cpp
//  each name for their own made object.
struct SMadeAbilityAgent
{
    char    m_reserved00[0x54];
    CAgent* m_pAgent;   // +0x54
};

//  0x6F18A2C0 - 'ARal'.
CAgent* MakeAbilityAgent_ARal()
{
    //  Constructed empty and assigned in one step, which is what the
    //  0x6F16DFC0 constructor is (cunit_agent3_checkedslots_ctor.cpp).
    SCheckedAbilitySlot_ARal held(0);

    AGILE_TYPE_ID id(GetFourCC_6F027BD0());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F027BD0(), type->m_allocator);

    held.Assign(((SMadeAbilityAgent*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}

//  0x6F2381A0 - 'Aast'.
CAgent* MakeAbilityAgent_Aast()
{
    //  Constructed empty and assigned in one step, which is what the
    //  0x6F220AC0 constructor is (cunit_agent3_checkedslots_ctor.cpp).
    SCheckedAbilitySlot_Aast held(0);

    AGILE_TYPE_ID id(GetAbilityAgileTypeId_Aast());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetAbilityAgileTypeId_Aast(), type->m_allocator);

    held.Assign(((SMadeAbilityAgent*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}

//  0x6F2388F0 - 'Aadm'.
CAgent* MakeAbilityAgent_Aadm()
{
    //  Constructed empty and assigned in one step, which is what the
    //  0x6F220E20 constructor is (cunit_agent3_checkedslots_ctor.cpp).
    SCheckedAbilitySlot_Aadm held(0);

    AGILE_TYPE_ID id(GetAbilityAgileTypeId_Aadm());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetAbilityAgileTypeId_Aadm(), type->m_allocator);

    held.Assign(((SMadeAbilityAgent*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}

//  0x6F239040 - 'Abli'.
CAgent* MakeAbilityAgent_Abli()
{
    //  Constructed empty and assigned in one step, which is what the
    //  0x6F221180 constructor is (cunit_agent3_checkedslots_ctor.cpp).
    SCheckedAbilitySlot_Abli held(0);

    AGILE_TYPE_ID id(GetAbilityAgileTypeId_Abli());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetAbilityAgileTypeId_Abli(), type->m_allocator);

    held.Assign(((SMadeAbilityAgent*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}

//  0x6F2396C0 - 'Adro'.
CAgent* MakeAbilityAgent_Adro()
{
    //  Constructed empty and assigned in one step, which is what the
    //  0x6F092640 constructor is (cunit_agent3_checkedslots_ctor.cpp).
    SCheckedAbilitySlot_Adro held(0);

    AGILE_TYPE_ID id(GetAbilityAgileTypeId_Adro());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetAbilityAgileTypeId_Adro(), type->m_allocator);

    held.Assign(((SMadeAbilityAgent*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}

//  0x6F239E10 - 'Acmg'.
CAgent* MakeAbilityAgent_Acmg()
{
    //  Constructed empty and assigned in one step, which is what the
    //  0x6F221660 constructor is (cunit_agent3_checkedslots_ctor.cpp).
    SCheckedAbilitySlot_Acmg held(0);

    AGILE_TYPE_ID id(GetAbilityAgileTypeId_Acmg());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetAbilityAgileTypeId_Acmg(), type->m_allocator);

    held.Assign(((SMadeAbilityAgent*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}

//  0x6F23A490 - 'ACrn'.
CAgent* MakeAbilityAgent_ACrn()
{
    //  Constructed empty and assigned in one step, which is what the
    //  0x6F2218A0 constructor is (cunit_agent3_checkedslots_ctor.cpp).
    SCheckedAbilitySlot_ACrn held(0);

    AGILE_TYPE_ID id(GetAbilityAgileTypeId_ACrn());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetAbilityAgileTypeId_ACrn(), type->m_allocator);

    held.Assign(((SMadeAbilityAgent*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}

//  0x6F23AD80 - 'Adef'.
CAgent* MakeAbilityAgent_Adef()
{
    //  Constructed empty and assigned in one step, which is what the
    //  0x6F221C60 constructor is (cunit_agent3_checkedslots_ctor.cpp).
    SCheckedAbilitySlot_Adef held(0);

    AGILE_TYPE_ID id(GetAbilityAgileTypeId_Adef());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetAbilityAgileTypeId_Adef(), type->m_allocator);

    held.Assign(((SMadeAbilityAgent*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}

//  0x6F23B400 - 'Adis'.
CAgent* MakeAbilityAgent_Adis()
{
    //  Constructed empty and assigned in one step, which is what the
    //  0x6F221F00 constructor is (cunit_agent3_checkedslots_ctor.cpp).
    SCheckedAbilitySlot_Adis held(0);

    AGILE_TYPE_ID id(GetAbilityAgileTypeId_Adis());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetAbilityAgileTypeId_Adis(), type->m_allocator);

    held.Assign(((SMadeAbilityAgent*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}

//  0x6F23BA80 - 'Aeye'.
CAgent* MakeAbilityAgent_Aeye()
{
    //  Constructed empty and assigned in one step, which is what the
    //  0x6F222200 constructor is (cunit_agent3_checkedslots_ctor.cpp).
    SCheckedAbilitySlot_Aeye held(0);

    AGILE_TYPE_ID id(GetAbilityAgileTypeId_Aeye());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetAbilityAgileTypeId_Aeye(), type->m_allocator);

    held.Assign(((SMadeAbilityAgent*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}

//  0x6F23E320 - 'ANgl'.
CAgent* MakeAbilityAgent_ANgl()
{
    //  Constructed empty and assigned in one step, which is what the
    //  0x6F223220 constructor is (cunit_agent3_checkedslots_ctor.cpp).
    SCheckedAbilitySlot_ANgl held(0);

    AGILE_TYPE_ID id(GetAbilityAgileTypeId_ANgl());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetAbilityAgileTypeId_ANgl(), type->m_allocator);

    held.Assign(((SMadeAbilityAgent*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}

//  0x6F240540 - 'ANsa'.
CAgent* MakeAbilityAgent_ANsa()
{
    //  Constructed empty and assigned in one step, which is what the
    //  0x6F224060 constructor is (cunit_agent3_checkedslots_ctor.cpp).
    SCheckedAbilitySlot_ANsa held(0);

    AGILE_TYPE_ID id(GetAbilityAgileTypeId_ANsa());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetAbilityAgileTypeId_ANsa(), type->m_allocator);

    held.Assign(((SMadeAbilityAgent*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}

//  0x6F242830 - 'Aamk'.
CAgent* MakeAbilityAgent_Aamk()
{
    //  Constructed empty and assigned in one step, which is what the
    //  0x6F224F00 constructor is (cunit_agent3_checkedslots_ctor.cpp).
    SCheckedAbilitySlot_Aamk held(0);

    AGILE_TYPE_ID id(GetAbilityAgileTypeId_Aamk());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetAbilityAgileTypeId_Aamk(), type->m_allocator);

    held.Assign(((SMadeAbilityAgent*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}

//  0x6F242EB0 - 'AUav'.
CAgent* MakeAbilityAgent_AUav()
{
    //  Constructed empty and assigned in one step, which is what the
    //  0x6F225200 constructor is (cunit_agent3_checkedslots_ctor.cpp).
    SCheckedAbilitySlot_AUav held(0);

    AGILE_TYPE_ID id(GetAbilityAgileTypeId_AUav());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetAbilityAgileTypeId_AUav(), type->m_allocator);

    held.Assign(((SMadeAbilityAgent*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}

//  0x6F243530 - 'AHbz'.
CAgent* MakeAbilityAgent_AHbz()
{
    //  Constructed empty and assigned in one step, which is what the
    //  0x6F225500 constructor is (cunit_agent3_checkedslots_ctor.cpp).
    SCheckedAbilitySlot_AHbz held(0);

    AGILE_TYPE_ID id(GetAbilityAgileTypeId_AHbz());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetAbilityAgileTypeId_AHbz(), type->m_allocator);

    held.Assign(((SMadeAbilityAgent*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}

//  0x6F243BB0 - 'AOcr'.
CAgent* MakeAbilityAgent_AOcr()
{
    //  Constructed empty and assigned in one step, which is what the
    //  0x6F2257A0 constructor is (cunit_agent3_checkedslots_ctor.cpp).
    SCheckedAbilitySlot_AOcr held(0);

    AGILE_TYPE_ID id(GetAbilityAgileTypeId_AOcr());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetAbilityAgileTypeId_AOcr(), type->m_allocator);

    held.Assign(((SMadeAbilityAgent*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}

//  0x6F244230 - 'AUin'.
CAgent* MakeAbilityAgent_AUin()
{
    //  Constructed empty and assigned in one step, which is what the
    //  0x6F225AA0 constructor is (cunit_agent3_checkedslots_ctor.cpp).
    SCheckedAbilitySlot_AUin held(0);

    AGILE_TYPE_ID id(GetAbilityAgileTypeId_AUin());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetAbilityAgileTypeId_AUin(), type->m_allocator);

    held.Assign(((SMadeAbilityAgent*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}

//  0x6F2448B0 - 'AEer'.
CAgent* MakeAbilityAgent_AEer()
{
    //  Constructed empty and assigned in one step, which is what the
    //  0x6F225DA0 constructor is (cunit_agent3_checkedslots_ctor.cpp).
    SCheckedAbilitySlot_AEer held(0);

    AGILE_TYPE_ID id(GetAbilityAgileTypeId_AEer());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetAbilityAgileTypeId_AEer(), type->m_allocator);

    held.Assign(((SMadeAbilityAgent*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}

//  0x6F244F30 - 'AEfn'.
CAgent* MakeAbilityAgent_AEfn()
{
    //  Constructed empty and assigned in one step, which is what the
    //  0x6F2260A0 constructor is (cunit_agent3_checkedslots_ctor.cpp).
    SCheckedAbilitySlot_AEfn held(0);

    AGILE_TYPE_ID id(GetAbilityAgileTypeId_AEfn());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetAbilityAgileTypeId_AEfn(), type->m_allocator);

    held.Assign(((SMadeAbilityAgent*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}

//  0x6F2455B0 - 'ANht'.
CAgent* MakeAbilityAgent_ANht()
{
    //  Constructed empty and assigned in one step, which is what the
    //  0x6F2263A0 constructor is (cunit_agent3_checkedslots_ctor.cpp).
    SCheckedAbilitySlot_ANht held(0);

    AGILE_TYPE_ID id(GetAbilityAgileTypeId_ANht());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetAbilityAgileTypeId_ANht(), type->m_allocator);

    held.Assign(((SMadeAbilityAgent*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}

//  0x6F245D00 - 'AHmt'.
CAgent* MakeAbilityAgent_AHmt()
{
    //  Constructed empty and assigned in one step, which is what the
    //  0x6F226700 constructor is (cunit_agent3_checkedslots_ctor.cpp).
    SCheckedAbilitySlot_AHmt held(0);

    AGILE_TYPE_ID id(GetAbilityAgileTypeId_AHmt());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetAbilityAgileTypeId_AHmt(), type->m_allocator);

    held.Assign(((SMadeAbilityAgent*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}

//  0x6F246380 - 'AHre'.
CAgent* MakeAbilityAgent_AHre()
{
    //  Constructed empty and assigned in one step, which is what the
    //  0x6F226A00 constructor is (cunit_agent3_checkedslots_ctor.cpp).
    SCheckedAbilitySlot_AHre held(0);

    AGILE_TYPE_ID id(GetAbilityAgileTypeId_AHre());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetAbilityAgileTypeId_AHre(), type->m_allocator);

    held.Assign(((SMadeAbilityAgent*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}

//  0x6F246A00 - 'AOsf'.
CAgent* MakeAbilityAgent_AOsf()
{
    //  Constructed empty and assigned in one step, which is what the
    //  0x6F226D00 constructor is (cunit_agent3_checkedslots_ctor.cpp).
    SCheckedAbilitySlot_AOsf held(0);

    AGILE_TYPE_ID id(GetAbilityAgileTypeId_AOsf());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetAbilityAgileTypeId_AOsf(), type->m_allocator);

    held.Assign(((SMadeAbilityAgent*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}

//  0x6F247080 - 'AUts'.
CAgent* MakeAbilityAgent_AUts()
{
    //  Constructed empty and assigned in one step, which is what the
    //  0x6F227000 constructor is (cunit_agent3_checkedslots_ctor.cpp).
    SCheckedAbilitySlot_AUts held(0);

    AGILE_TYPE_ID id(GetAbilityAgileTypeId_AUts());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetAbilityAgileTypeId_AUts(), type->m_allocator);

    held.Assign(((SMadeAbilityAgent*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}

//  0x6F247970 - 'ANwm'.
CAgent* MakeAbilityAgent_ANwm()
{
    //  Constructed empty and assigned in one step, which is what the
    //  0x6F227420 constructor is (cunit_agent3_checkedslots_ctor.cpp).
    SCheckedAbilitySlot_ANwm held(0);

    AGILE_TYPE_ID id(GetAbilityAgileTypeId_ANwm());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetAbilityAgileTypeId_ANwm(), type->m_allocator);

    held.Assign(((SMadeAbilityAgent*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}

//  0x6F247FF0 - 'ANde'.
CAgent* MakeAbilityAgent_ANde()
{
    //  Constructed empty and assigned in one step, which is what the
    //  0x6F2276C0 constructor is (cunit_agent3_checkedslots_ctor.cpp).
    SCheckedAbilitySlot_ANde held(0);

    AGILE_TYPE_ID id(GetAbilityAgileTypeId_ANde());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetAbilityAgileTypeId_ANde(), type->m_allocator);

    held.Assign(((SMadeAbilityAgent*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}

//  0x6F248670 - 'ANso'.
CAgent* MakeAbilityAgent_ANso()
{
    //  Constructed empty and assigned in one step, which is what the
    //  0x6F2279C0 constructor is (cunit_agent3_checkedslots_ctor.cpp).
    SCheckedAbilitySlot_ANso held(0);

    AGILE_TYPE_ID id(GetAbilityAgileTypeId_ANso());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetAbilityAgileTypeId_ANso(), type->m_allocator);

    held.Assign(((SMadeAbilityAgent*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}

//  0x6F248CF0 - 'AIat'.
CAgent* MakeAbilityAgent_AIat()
{
    //  Constructed empty and assigned in one step, which is what the
    //  0x6F227CC0 constructor is (cunit_agent3_checkedslots_ctor.cpp).
    SCheckedAbilitySlot_AIat held(0);

    AGILE_TYPE_ID id(GetAbilityAgileTypeId_AIat());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetAbilityAgileTypeId_AIat(), type->m_allocator);

    held.Assign(((SMadeAbilityAgent*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}

//  0x6F249370 - 'AIfe'.
CAgent* MakeAbilityAgent_AIfe()
{
    //  Constructed empty and assigned in one step, which is what the
    //  0x6F227FC0 constructor is (cunit_agent3_checkedslots_ctor.cpp).
    SCheckedAbilitySlot_AIfe held(0);

    AGILE_TYPE_ID id(GetAbilityAgileTypeId_AIfe());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetAbilityAgileTypeId_AIfe(), type->m_allocator);

    held.Assign(((SMadeAbilityAgent*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}

//  0x6F2499F0 - 'AIfs'.
CAgent* MakeAbilityAgent_AIfs()
{
    //  Constructed empty and assigned in one step, which is what the
    //  0x6F2282C0 constructor is (cunit_agent3_checkedslots_ctor.cpp).
    SCheckedAbilitySlot_AIfs held(0);

    AGILE_TYPE_ID id(GetAbilityAgileTypeId_AIfs());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetAbilityAgileTypeId_AIfs(), type->m_allocator);

    held.Assign(((SMadeAbilityAgent*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}

//  0x6F24A070 - 'AIml'.
CAgent* MakeAbilityAgent_AIml()
{
    //  Constructed empty and assigned in one step, which is what the
    //  0x6F2285C0 constructor is (cunit_agent3_checkedslots_ctor.cpp).
    SCheckedAbilitySlot_AIml held(0);

    AGILE_TYPE_ID id(GetAbilityAgileTypeId_AIml());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetAbilityAgileTypeId_AIml(), type->m_allocator);

    held.Assign(((SMadeAbilityAgent*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}

//  0x6F24A7C0 - 'AIsb'.
CAgent* MakeAbilityAgent_AIsb()
{
    //  Constructed empty and assigned in one step, which is what the
    //  0x6F2288C0 constructor is (cunit_agent3_checkedslots_ctor.cpp).
    SCheckedAbilitySlot_AIsb held(0);

    AGILE_TYPE_ID id(GetAbilityAgileTypeId_AIsb());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetAbilityAgileTypeId_AIsb(), type->m_allocator);

    held.Assign(((SMadeAbilityAgent*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}

//  0x6F24AE40 - 'AIdi'.
CAgent* MakeAbilityAgent_AIdi()
{
    //  Constructed empty and assigned in one step, which is what the
    //  0x6F228BC0 constructor is (cunit_agent3_checkedslots_ctor.cpp).
    SCheckedAbilitySlot_AIdi held(0);

    AGILE_TYPE_ID id(GetAbilityAgileTypeId_AIdi());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetAbilityAgileTypeId_AIdi(), type->m_allocator);

    held.Assign(((SMadeAbilityAgent*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}

//  0x6F24B4C0 - 'AIrt'.
CAgent* MakeAbilityAgent_AIrt()
{
    //  Constructed empty and assigned in one step, which is what the
    //  0x6F228EC0 constructor is (cunit_agent3_checkedslots_ctor.cpp).
    SCheckedAbilitySlot_AIrt held(0);

    AGILE_TYPE_ID id(GetAbilityAgileTypeId_AIrt());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetAbilityAgileTypeId_AIrt(), type->m_allocator);

    held.Assign(((SMadeAbilityAgent*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}

//  0x6F24BB40 - 'AIte'.
CAgent* MakeAbilityAgent_AIte()
{
    //  Constructed empty and assigned in one step, which is what the
    //  0x6F2291C0 constructor is (cunit_agent3_checkedslots_ctor.cpp).
    SCheckedAbilitySlot_AIte held(0);

    AGILE_TYPE_ID id(GetAbilityAgileTypeId_AIte());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetAbilityAgileTypeId_AIte(), type->m_allocator);

    held.Assign(((SMadeAbilityAgent*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}

//  0x6F24C1C0 - 'AIgo'.
CAgent* MakeAbilityAgent_AIgo()
{
    //  Constructed empty and assigned in one step, which is what the
    //  0x6F229460 constructor is (cunit_agent3_checkedslots_ctor.cpp).
    SCheckedAbilitySlot_AIgo held(0);

    AGILE_TYPE_ID id(GetAbilityAgileTypeId_AIgo());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetAbilityAgileTypeId_AIgo(), type->m_allocator);

    held.Assign(((SMadeAbilityAgent*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}

//  0x6F24C840 - 'AIbl'.
CAgent* MakeAbilityAgent_AIbl()
{
    //  Constructed empty and assigned in one step, which is what the
    //  0x6F229760 constructor is (cunit_agent3_checkedslots_ctor.cpp).
    SCheckedAbilitySlot_AIbl held(0);

    AGILE_TYPE_ID id(GetAbilityAgileTypeId_AIbl());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetAbilityAgileTypeId_AIbl(), type->m_allocator);

    held.Assign(((SMadeAbilityAgent*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}

//  0x6F29AFB0 - 'Amai'.
CAgent* MakeAbilityAgent_Amai()
{
    //  Constructed empty and assigned in one step, which is what the
    //  0x6F280FC0 constructor is (cunit_agent3_checkedslots_ctor.cpp).
    SCheckedAbilitySlot_Amai held(0);

    AGILE_TYPE_ID id(GetAbilityAgileTypeId_Amai());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetAbilityAgileTypeId_Amai(), type->m_allocator);

    held.Assign(((SMadeAbilityAgent*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}

