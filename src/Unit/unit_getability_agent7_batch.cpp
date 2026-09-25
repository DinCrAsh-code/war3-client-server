//============================================================================
//  cunit_agent7 batch (CUnit closure BFS depth 1-2) - 33 near-identical
//  "GetXxxAbility" accessors, all in the same 0x6F2384E0-0x6F24CC50 /
//  0x6F29ACxx neighbourhood (one original module), plus 21 more of the
//  identical shape (slice B, appended below the original 33) filling
//  gaps in the same address range that a later BFS pass reached -
//  0x6F238680 through 0x6F24AD70, and 0x6F29AEE0 (six 0xD0-steps into the
//  same 13-instance 0x6F29AA00..0x6F29B3C0 run this file's own
//  0x6F29AC70/0x6F29B2F0 already sample).  Each:
//
//      SCheckedXxxSlot temp(0);                     // always empty
//      AGILE_TYPE_ID id(GetFourCC());                // this instance's tag
//      AGILE_TYPE_DATA* type = g_pGameData->m_types.Ptr(id.Hash(), id);
//      SWidgetAgentQuery query;
//      InitWidgetAgentQuery(&query, GetFourCC(), type->m_allocator);
//      CAgent* found = ((SMadeAgent*)SubmitWidgetAgentQuery(&query,1,1))->m_pAgent;
//      temp.Assign(found);
//      return temp.m_value;                          // temp destroyed inline
//
//  matches SCheckedSlot_6F2700B0::Make (checkedslot_batch26.cpp) leaf for
//  leaf, minus that function's extra `query.m_reserved24` write and its
//  three (int,int,kind) parameters - these take none and always assign a
//  null candidate at construction ("temp(0)"), where Make instead reuses
//  the already-live slot as `this`.  GetFourCC() is called twice, once for
//  the registry lookup id and once again as InitWidgetAgentQuery's own
//  rawcode argument - the same double-call SCheckedRectSlot::MakeRect
//  already has (jassrectmake.cpp), not a mistake here.
//
//  Every one of the 54 opens with the unreproducible __except_handler4-
//  shaped frame documented in docs/msvc-vc8-idioms.md ("An
//  `__except_handler4`-shaped frame this toolchain cannot reproduce") -
//  this build's fixed /GS- /EHs-c- cannot emit it with the bundled cl.exe -
//  so every one of the 54 is DIFFERS for that reason (recorded per-address
//  in agent_worktrees/, folded into tools/funcmap.py's BEHAVIOUR table by
//  sync_funcmap_from_worktrees.py), not argued to EXACT.
//
//  0x6F2458F0 is the one exception to the "throwaway local type" shape:
//  its ctor is this same batch's own 0x6F226520
//  (checkedinventoryabilityslot.cpp, SCheckedInventoryAbilitySlot), so it
//  reuses that real type and its Assign's real leaf (GetFourCC_6F0DB030,
//  Misc/misc_trivial_getters.cpp) instead of a throwaway one.
//============================================================================
#include "unit.h"
#include "checkedslot_agent7_family.h"
#include "agenttypedslots.h"
#include "agiletype.h"
#include "game.h"
#include "widgetagentquery.h"

//  What SubmitWidgetAgentQuery hands back; only +0x54 is ever read (the
//  same shape checkedslot_batch26.cpp's own SMadeAgent_Batch26 documents).
struct SMadeAgent_Agent7
{
    char    m_reserved00[0x54];
    CAgent* m_pAgent;   // +0x54
};

extern unsigned int GetFourCC_6F0DB030();   // 'AInv' - Misc/misc_trivial_getters.cpp

//----------------------------------------------------------------------------
//  0x6F2458F0 - 'AInv'.  Reuses SCheckedInventoryAbilitySlot
//  (checkedinventoryabilityslot.cpp) rather than a throwaway local type -
//  see this file's own header comment.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F2458F0()
{
    SCheckedInventoryAbilitySlot temp(0);
    unsigned int rawcode = GetFourCC_6F0DB030();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F0DB030(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Agent7*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}

//----------------------------------------------------------------------------
//  0x6F2384E0 - 'Aakb'.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F2384E0()
{
    SLocalAbilitySlot_6F2384E0 temp(0);
    unsigned int rawcode = GetFourCC_6F195D20();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F195D20(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Agent7*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}

//----------------------------------------------------------------------------
//  0x6F238C30 - 'Abar'.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F238C30()
{
    SLocalAbilitySlot_6F238C30 temp(0);
    unsigned int rawcode = GetFourCC_6F196D50();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F196D50(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Agent7*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}

//----------------------------------------------------------------------------
//  0x6F239380 - 'ACbf'.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F239380()
{
    SLocalAbilitySlot_6F239380 temp(0);
    unsigned int rawcode = GetFourCC_6F1984A0();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F1984A0(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Agent7*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}

//----------------------------------------------------------------------------
//  0x6F239AD0 - 'ANcl'.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F239AD0()
{
    SLocalAbilitySlot_6F239AD0 temp(0);
    unsigned int rawcode = GetFourCC_6F1955E0();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F1955E0(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Agent7*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}

//----------------------------------------------------------------------------
//  0x6F23A150 - 'Acoh'.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F23A150()
{
    SLocalAbilitySlot_6F23A150 temp(0);
    unsigned int rawcode = GetFourCC_6F19A230();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F19A230(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Agent7*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}

//----------------------------------------------------------------------------
//  0x6F23A7D0 - 'Acrs'.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F23A7D0()
{
    SLocalAbilitySlot_6F23A7D0 temp(0);
    unsigned int rawcode = GetFourCC_6F07F780();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F07F780(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Agent7*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}

//----------------------------------------------------------------------------
//  0x6F23B0C0 - 'Adev'.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F23B0C0()
{
    SLocalAbilitySlot_6F23B0C0 temp(0);
    unsigned int rawcode = GetFourCC_6F07CD70();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F07CD70(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Agent7*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}

//----------------------------------------------------------------------------
//  0x6F23B740 - 'Aenc'.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F23B740()
{
    SLocalAbilitySlot_6F23B740 temp(0);
    unsigned int rawcode = GetFourCC_6F0DA190();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F0DA190(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Agent7*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}

//----------------------------------------------------------------------------
//  0x6F23BE90 - 'Afak'.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F23BE90()
{
    SLocalAbilitySlot_6F23BE90 temp(0);
    unsigned int rawcode = GetFourCC_6F0818C0();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F0818C0(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Agent7*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}

//----------------------------------------------------------------------------
//  0x6F23E730 - 'ANpa'.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F23E730()
{
    SLocalAbilitySlot_6F23E730 temp(0);
    unsigned int rawcode = GetFourCC_6F112BD0();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F112BD0(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Agent7*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}

//----------------------------------------------------------------------------
//  0x6F241240 - 'AIsh'.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F241240()
{
    SLocalAbilitySlot_6F241240 temp(0);
    unsigned int rawcode = GetFourCC_6F1D7B10();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F1D7B10(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Agent7*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}

//----------------------------------------------------------------------------
//  0x6F242B70 - 'AOae'.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F242B70()
{
    SLocalAbilitySlot_6F242B70 temp(0);
    unsigned int rawcode = GetFourCC_6F1960A0();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F1960A0(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Agent7*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}

//----------------------------------------------------------------------------
//  0x6F2431F0 - 'ANbr'.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F2431F0()
{
    SLocalAbilitySlot_6F2431F0 temp(0);
    unsigned int rawcode = GetFourCC_6F1D3BA0();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F1D3BA0(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Agent7*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}

//----------------------------------------------------------------------------
//  0x6F243870 - 'AOcl'.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F243870()
{
    SLocalAbilitySlot_6F243870 temp(0);
    unsigned int rawcode = GetFourCC_6F198D10();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F198D10(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Agent7*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}

//----------------------------------------------------------------------------
//  0x6F243EF0 - 'AUds'.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F243EF0()
{
    SLocalAbilitySlot_6F243EF0 temp(0);
    unsigned int rawcode = GetFourCC_6F025EB0();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F025EB0(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Agent7*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}

//----------------------------------------------------------------------------
//  0x6F244570 - 'ANdb'.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F244570()
{
    SLocalAbilitySlot_6F244570 temp(0);
    unsigned int rawcode = GetFourCC_6F19AA40();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F19AA40(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Agent7*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}

//----------------------------------------------------------------------------
//  0x6F244BF0 - 'ANfd'.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F244BF0()
{
    SLocalAbilitySlot_6F244BF0 temp(0);
    unsigned int rawcode = GetFourCC_6F195460();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F195460(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Agent7*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}

//----------------------------------------------------------------------------
//  0x6F245270 - 'AUfn'.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F245270()
{
    SLocalAbilitySlot_6F245270 temp(0);
    unsigned int rawcode = GetFourCC_6F156E20();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F156E20(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Agent7*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}

//----------------------------------------------------------------------------
//  0x6F246040 - 'AEpa'.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F246040()
{
    SLocalAbilitySlot_6F246040 temp(0);
    unsigned int rawcode = GetFourCC_6F113B50();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F113B50(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Agent7*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}

//----------------------------------------------------------------------------
//  0x6F2466C0 - 'AOsh'.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F2466C0()
{
    SLocalAbilitySlot_6F2466C0 temp(0);
    unsigned int rawcode = GetFourCC_6F1D5800();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F1D5800(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Agent7*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}

//----------------------------------------------------------------------------
//  0x6F246D40 - 'ANsg'.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F246D40()
{
    SLocalAbilitySlot_6F246D40 temp(0);
    unsigned int rawcode = GetFourCC_6F159BA0();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F159BA0(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Agent7*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}

//----------------------------------------------------------------------------
//  0x6F247630 - 'AEtq'.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F247630()
{
    SLocalAbilitySlot_6F247630 temp(0);
    unsigned int rawcode = GetFourCC_6F158660();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F158660(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Agent7*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}

//----------------------------------------------------------------------------
//  0x6F247CB0 - 'ANcr'.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F247CB0()
{
    SLocalAbilitySlot_6F247CB0 temp(0);
    unsigned int rawcode = GetFourCC_6F194880();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F194880(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Agent7*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}

//----------------------------------------------------------------------------
//  0x6F248330 - 'ANsy'.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F248330()
{
    SLocalAbilitySlot_6F248330 temp(0);
    unsigned int rawcode = GetFourCC_6F157300();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F157300(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Agent7*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}

//----------------------------------------------------------------------------
//  0x6F2489B0 - 'AIam'.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F2489B0()
{
    SLocalAbilitySlot_6F2489B0 temp(0);
    unsigned int rawcode = GetFourCC_6F0DCB20();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F0DCB20(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Agent7*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}

//----------------------------------------------------------------------------
//  0x6F249030 - 'AIem'.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F249030()
{
    SLocalAbilitySlot_6F249030 temp(0);
    unsigned int rawcode = GetFourCC_6F0DD320();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F0DD320(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Agent7*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}

//----------------------------------------------------------------------------
//  0x6F2496B0 - 'AIfd'.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F2496B0()
{
    SLocalAbilitySlot_6F2496B0 temp(0);
    unsigned int rawcode = GetFourCC_6F07FFD0();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F07FFD0(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Agent7*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}

//----------------------------------------------------------------------------
//  0x6F249D30 - 'AIgl'.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F249D30()
{
    SLocalAbilitySlot_6F249D30 temp(0);
    unsigned int rawcode = GetFourCC_6F082220();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F082220(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Agent7*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}

//----------------------------------------------------------------------------
//  0x6F24A3B0 - 'AIms'.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F24A3B0()
{
    SLocalAbilitySlot_6F24A3B0 temp(0);
    unsigned int rawcode = GetFourCC_6F0DCE20();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F0DCE20(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Agent7*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}

//----------------------------------------------------------------------------
//  0x6F24AB00 - 'AIco'.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F24AB00()
{
    SLocalAbilitySlot_6F24AB00 temp(0);
    unsigned int rawcode = GetFourCC_6F1993C0();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F1993C0(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Agent7*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}

//----------------------------------------------------------------------------
//  0x6F24B180 - 'AIma'.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F24B180()
{
    SLocalAbilitySlot_6F24B180 temp(0);
    unsigned int rawcode = GetFourCC_6F111510();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F111510(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Agent7*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}

//----------------------------------------------------------------------------
//  0x6F24B800 - 'AIra'.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F24B800()
{
    SLocalAbilitySlot_6F24B800 temp(0);
    unsigned int rawcode = GetFourCC_6F1D32E0();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F1D32E0(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Agent7*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}

//----------------------------------------------------------------------------
//  0x6F24BE80 - 'AIvu'.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F24BE80()
{
    SLocalAbilitySlot_6F24BE80 temp(0);
    unsigned int rawcode = GetFourCC_6F10FA10();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F10FA10(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Agent7*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}

//----------------------------------------------------------------------------
//  0x6F24C500 - 'AImo'.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F24C500()
{
    SLocalAbilitySlot_6F24C500 temp(0);
    unsigned int rawcode = GetFourCC_6F1596A0();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F1596A0(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Agent7*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}

//----------------------------------------------------------------------------
//  0x6F24CC50 - 'AIsr'.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F24CC50()
{
    SLocalAbilitySlot_6F24CC50 temp(0);
    unsigned int rawcode = GetFourCC_6F0DDB20();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F0DDB20(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Agent7*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}

//----------------------------------------------------------------------------
//  0x6F29AC70 - 'AUbu'.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F29AC70()
{
    SLocalAbilitySlot_6F29AC70 temp(0);
    unsigned int rawcode = GetFourCC_6F0D91A0();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F0D91A0(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Agent7*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}

//----------------------------------------------------------------------------
//  0x6F29B2F0 - 'Amov'.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F29B2F0()
{
    SLocalAbilitySlot_6F29B2F0 temp(0);
    unsigned int rawcode = GetFourCC_6F200960();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F200960(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Agent7*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}

//----------------------------------------------------------------------------
//  0x6F238680 - 'Aoar'.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F238680()
{
    SLocalAbilitySlot_6F238680 temp(0);
    unsigned int rawcode = GetFourCC_6F022E90();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F022E90(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Agent7*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}

//----------------------------------------------------------------------------
//  0x6F23AA40 - 'Asod'.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F23AA40()
{
    SLocalAbilitySlot_6F23AA40 temp(0);
    unsigned int rawcode = GetFourCC_6F080250();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F080250(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Agent7*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}

//----------------------------------------------------------------------------
//  0x6F23C030 - 'Afsh'.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F23C030()
{
    SLocalAbilitySlot_6F23C030 temp(0);
    unsigned int rawcode = GetFourCC_6F15AA90();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F15AA90(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Agent7*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}

//----------------------------------------------------------------------------
//  0x6F23C780 - 'Agyd'.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F23C780()
{
    SLocalAbilitySlot_6F23C780 temp(0);
    unsigned int rawcode = GetFourCC_6F082660();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F082660(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Agent7*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}

//----------------------------------------------------------------------------
//  0x6F23CE00 - 'Ahid'.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F23CE00()
{
    SLocalAbilitySlot_6F23CE00 temp(0);
    unsigned int rawcode = GetFourCC_6F1D4CA0();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F1D4CA0(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Agent7*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}

//----------------------------------------------------------------------------
//  0x6F23D480 - 'Aliq'.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F23D480()
{
    SLocalAbilitySlot_6F23D480 temp(0);
    unsigned int rawcode = GetFourCC_6F15B410();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F15B410(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Agent7*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}

//----------------------------------------------------------------------------
//  0x6F23DBD0 - 'Amel'.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F23DBD0()
{
    SLocalAbilitySlot_6F23DBD0 temp(0);
    unsigned int rawcode = GetFourCC_6F1118B0();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F1118B0(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Agent7*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}

//----------------------------------------------------------------------------
//  0x6F23E250 - 'Andt'.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F23E250()
{
    SLocalAbilitySlot_6F23E250 temp(0);
    unsigned int rawcode = GetFourCC_6F024C50();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F024C50(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Agent7*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}

//----------------------------------------------------------------------------
//  0x6F23EE80 - 'Aply'.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F23EE80()
{
    SLocalAbilitySlot_6F23EE80 temp(0);
    unsigned int rawcode = GetFourCC_6F113C40();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F113C40(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Agent7*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}

//----------------------------------------------------------------------------
//  0x6F23F500 - 'Arbr'.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F23F500()
{
    SLocalAbilitySlot_6F23F500 temp(0);
    unsigned int rawcode = GetFourCC_6F1D7290();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F1D7290(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Agent7*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}

//----------------------------------------------------------------------------
//  0x6F23FB80 - 'Arpm'.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F23FB80()
{
    SLocalAbilitySlot_6F23FB80 temp(0);
    unsigned int rawcode = GetFourCC_6F1D3460();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F1D3460(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Agent7*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}

//----------------------------------------------------------------------------
//  0x6F240200 - 'Aroo'.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F240200()
{
    SLocalAbilitySlot_6F240200 temp(0);
    unsigned int rawcode = GetFourCC_6F0283A0();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F0283A0(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Agent7*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}

//----------------------------------------------------------------------------
//  0x6F240A20 - 'Ashs'.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F240A20()
{
    SLocalAbilitySlot_6F240A20 temp(0);
    unsigned int rawcode = GetFourCC_6F07FD10();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F07FD10(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Agent7*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}

//----------------------------------------------------------------------------
//  0x6F241310 - 'Astd'.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F241310()
{
    SLocalAbilitySlot_6F241310 temp(0);
    unsigned int rawcode = GetFourCC_6F198720();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F198720(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Agent7*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}

//----------------------------------------------------------------------------
//  0x6F241A60 - 'Atpi'.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F241A60()
{
    SLocalAbilitySlot_6F241A60 temp(0);
    unsigned int rawcode = GetFourCC_6F1565E0();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F1565E0(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Agent7*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}

//----------------------------------------------------------------------------
//  0x6F2421B0 - 'Avng'.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F2421B0()
{
    SLocalAbilitySlot_6F2421B0 temp(0);
    unsigned int rawcode = GetFourCC_6F1D7780();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F1D7780(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Agent7*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}

//----------------------------------------------------------------------------
//  0x6F242DE0 - 'AUau'.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F242DE0()
{
    SLocalAbilitySlot_6F242DE0 temp(0);
    unsigned int rawcode = GetFourCC_6F196690();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F196690(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Agent7*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}

//----------------------------------------------------------------------------
//  0x6F245C30 - 'ANms'.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F245C30()
{
    SLocalAbilitySlot_6F245C30 temp(0);
    unsigned int rawcode = GetFourCC_6F111610();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F111610(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Agent7*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}

//----------------------------------------------------------------------------
//  0x6F247F20 - 'ANcs'.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F247F20()
{
    SLocalAbilitySlot_6F247F20 temp(0);
    unsigned int rawcode = GetFourCC_6F157580();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F157580(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Agent7*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}

//----------------------------------------------------------------------------
//  0x6F24AD70 - 'AIta'.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F24AD70()
{
    SLocalAbilitySlot_6F24AD70 temp(0);
    unsigned int rawcode = GetFourCC_6F024BF0();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F024BF0(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Agent7*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}

//----------------------------------------------------------------------------
//  0x6F29AEE0 - 'Aque'.
//----------------------------------------------------------------------------
CAgent* CUnit::GetAbility_6F29AEE0()
{
    SLocalAbilitySlot_6F29AEE0 temp(0);
    unsigned int rawcode = GetFourCC_6F0278B0();
    AGILE_TYPE_ID id(rawcode);
    unsigned int hash = id.Hash();
    CGameData* game = g_pGameData;
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F0278B0(), type->m_allocator);

    CAgent* found = ((SMadeAgent_Agent7*)
                     SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent;
    temp.Assign(found);
    return temp.m_value;
}
