//============================================================================
//  Twenty more "GetXxxAbility" accessors (see
//  Agent/checkedslot_agent7_batch_slot107.h for the family shape and why it
//  is a distinct extension of unit_getability_agent7_batch.cpp's own
//  cunit_agent7 batch) - all reached from CUnit's slot107 closure
//  (per-template initialiser, 0x6F2A0E30) rather than that batch's own
//  worklist.
//
//      SLocalAbilitySlot_<addr> temp(0);
//      AGILE_TYPE_ID id(GetFourCC());
//      AGILE_TYPE_DATA* type = g_pGameData->m_types.Ptr(id.Hash(), id);
//      SWidgetAgentQuery query;
//      InitWidgetAgentQuery(&query, GetFourCC(), type->m_allocator);
//      CAgent* found = ((SMadeAgent*)SubmitWidgetAgentQuery(&query,1,1))->m_pAgent;
//      temp.Assign(found);
//      return temp.m_value;                 // temp destroyed here (~dtor -> Release)
//
//  GetFourCC() is called twice (registry-lookup id, then InitWidgetAgentQuery's
//  own rawcode argument), the same double-call shape every sibling instance
//  of this family has.  Every one of these twenty opens the unreproducible
//  __except_handler4-shaped frame docs/msvc-vc8-idioms.md documents ("An
//  `__except_handler4`-shaped frame this toolchain cannot reproduce") - this
//  build's fixed /GS- /EHs-c- cannot emit it with the bundled cl.exe - so
//  every one is DIFFERS for that reason, not argued to EXACT.
//============================================================================
#include "unit.h"
#include "checkedslot_agent7_batch_slot107.h"
#include "agiletype.h"
#include "game.h"
#include "widgetagentquery.h"

//  What SubmitWidgetAgentQuery hands back; only +0x54 is ever read (the same
//  shape unit_getability_agent7_batch.cpp's own SMadeAgent_Agent7 documents).
struct SMadeAgent_Slot107
{
    char    m_reserved00[0x54];
    CAgent* m_pAgent;   // +0x54
};

//----------------------------------------------------------------------------
//  0x6F2380D0 - 'Aall'.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F2380D0()
{
    SLocalAbilitySlot_6F2380D0 temp(0);
    unsigned int rawcode = GetFourCC_6F026870();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F026870(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Slot107*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}

//----------------------------------------------------------------------------
//  0x6F23A3C0 - 'ACdv'.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F23A3C0()
{
    SLocalAbilitySlot_6F23A3C0 temp(0);
    unsigned int rawcode = GetFourCC_6F07CDF0();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F07CDF0(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Slot107*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}

//----------------------------------------------------------------------------
//  0x6F23BF60 - 'Afla'.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F23BF60()
{
    SLocalAbilitySlot_6F23BF60 temp(0);
    unsigned int rawcode = GetFourCC_6F081A60();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F081A60(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Slot107*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}

//----------------------------------------------------------------------------
//  0x6F23C6B0 - 'Agra'.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F23C6B0()
{
    SLocalAbilitySlot_6F23C6B0 temp(0);
    unsigned int rawcode = GetFourCC_6F07E3A0();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F07E3A0(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Slot107*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}

//----------------------------------------------------------------------------
//  0x6F23CD30 - 'Ahwd'.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F23CD30()
{
    SLocalAbilitySlot_6F23CD30 temp(0);
    unsigned int rawcode = GetFourCC_6F082830();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F082830(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Slot107*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}

//----------------------------------------------------------------------------
//  0x6F23D3B0 - 'Alsh'.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F23D3B0()
{
    SLocalAbilitySlot_6F23D3B0 temp(0);
    unsigned int rawcode = GetFourCC_6F1100B0();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F1100B0(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Slot107*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}

//----------------------------------------------------------------------------
//  0x6F23DB00 - 'Amed'.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F23DB00()
{
    SLocalAbilitySlot_6F23DB00 temp(0);
    unsigned int rawcode = GetFourCC_6F111830();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F111830(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Slot107*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}

//----------------------------------------------------------------------------
//  0x6F23E180 - 'Aneu'.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F23E180()
{
    SLocalAbilitySlot_6F23E180 temp(0);
    unsigned int rawcode = GetFourCC_6F026790();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F026790(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Slot107*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}

//----------------------------------------------------------------------------
//  0x6F23EDB0 - 'Apoi'.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F23EDB0()
{
    SLocalAbilitySlot_6F23EDB0 temp(0);
    unsigned int rawcode = GetFourCC_6F1139D0();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F1139D0(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Slot107*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}

//----------------------------------------------------------------------------
//  0x6F23F430 - 'Arav'.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F23F430()
{
    SLocalAbilitySlot_6F23F430 temp(0);
    unsigned int rawcode = GetFourCC_6F1D2D80();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F1D2D80(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Slot107*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}

//----------------------------------------------------------------------------
//  0x6F23FAB0 - 'Arpl'.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F23FAB0()
{
    SLocalAbilitySlot_6F23FAB0 temp(0);
    unsigned int rawcode = GetFourCC_6F1D33E0();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F1D33E0(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Slot107*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}

//----------------------------------------------------------------------------
//  0x6F240130 - 'Aroa'.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F240130()
{
    SLocalAbilitySlot_6F240130 temp(0);
    unsigned int rawcode = GetFourCC_6F1D3AA0();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F1D3AA0(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Slot107*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}

//----------------------------------------------------------------------------
//  0x6F240950 - 'Ashm'.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F240950()
{
    SLocalAbilitySlot_6F240950 temp(0);
    unsigned int rawcode = GetFourCC_6F1D4C20();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F1D4C20(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Slot107*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}

//----------------------------------------------------------------------------
//  0x6F241170 - 'Aspl'.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F241170()
{
    SLocalAbilitySlot_6F241170 temp(0);
    unsigned int rawcode = GetFourCC_6F1D73D0();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F1D73D0(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Slot107*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}

//----------------------------------------------------------------------------
//  0x6F241990 - 'Atlp'.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F241990()
{
    SLocalAbilitySlot_6F241990 temp(0);
    unsigned int rawcode = GetFourCC_6F156560();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F156560(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Slot107*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}

//----------------------------------------------------------------------------
//  0x6F2420E0 - 'Auns'.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F2420E0()
{
    SLocalAbilitySlot_6F2420E0 temp(0);
    unsigned int rawcode = GetFourCC_6F158F50();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F158F50(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Slot107*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}

//----------------------------------------------------------------------------
//  0x6F242760 - 'AUan'.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F242760()
{
    SLocalAbilitySlot_6F242760 temp(0);
    unsigned int rawcode = GetFourCC_6F194F10();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F194F10(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Slot107*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}

//----------------------------------------------------------------------------
//  0x6F245A90 - 'Aloc'.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F245A90()
{
    SLocalAbilitySlot_6F245A90 temp(0);
    unsigned int rawcode = GetFourCC_6F1102C0();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F1102C0(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Slot107*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}

//----------------------------------------------------------------------------
//  0x6F2478A0 - 'AHwe'.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F2478A0()
{
    SLocalAbilitySlot_6F2478A0 temp(0);
    unsigned int rawcode = GetFourCC_6F159920();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F159920(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Slot107*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}

//----------------------------------------------------------------------------
//  0x6F24A6F0 - 'AIsi'.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F24A6F0()
{
    SLocalAbilitySlot_6F24A6F0 temp(0);
    unsigned int rawcode = GetFourCC_6F0DCCA0();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F0DCCA0(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Slot107*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}
