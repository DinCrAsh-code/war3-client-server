//============================================================================
//  MakeCheckedAgent_<tag> - build a widget-agent request block for this
//  instantiation's own fixed agile type, submit it, and hand back whatever
//  came out the other end through a type-checked local (CAgentPtr_<tag>),
//  exactly the shape CWidget::ClearPlayerMaskBits (widget_slot68.cpp) already
//  documents in full for its own '+hgw' instantiation - this file is the same
//  body with the widget-specific tail (world position, player mask, UI notify)
//  removed, because none of these 37 callers do anything with the made agent
//  beyond returning it.
//
//  Every one of the 37 raw dumps (cunit_agent1_worklist.md) is the identical
//  78-instruction template with only the fixed-type leaf and the SEH scope-
//  table furniture differing address to address - confirmed by diffing all
//  37 bodies against each other with every `call`/`loc_`/`SEH_` operand
//  blanked out.  The unmatched instructions on every one of these are the
//  same unreproducible __except_handler4-shaped SEH frame
//  docs/msvc-vc8-idioms.md already tracks for this whole family (see also
//  CWidget::ClearPlayerMaskBits' and SWidgetAgentSlot::SWidgetAgentSlot's own
//  BEHAVIOUR entries) - the body between the two frames is the same
//  instructions in the same order.
//============================================================================
#include "widgetagentquery.h"
#include "agiletype.h"
#include "cunit_agent1_agentptr_family.h"
#include "game.h"

extern unsigned int GetFourCC_6F021B40();
extern unsigned int GetFourCC_6F022F10();
extern unsigned int GetFourCC_6F196E60();
extern unsigned int GetFourCC_6F1988C0();
extern unsigned int GetFourCC_6F199040();
extern unsigned int GetFourCC_6F194F90();
extern unsigned int GetFourCC_6F1D4640();
extern unsigned int GetFourCC_6F07CCF0();
extern unsigned int GetFourCC_6F07F3B0();
extern unsigned int GetFourCC_6F024A90();
extern unsigned int GetFourCC_6F10E7A0();
extern unsigned int GetFourCC_6F1569A0();
extern unsigned int GetFourCC_6F195E20();
extern unsigned int GetFourCC_6F197260();
extern unsigned int GetFourCC_6F1995C0();
extern unsigned int GetFourCC_6F07BE00();
extern unsigned int GetFourCC_6F07E0F0();
extern unsigned int GetFourCC_6F081130();
extern unsigned int GetFourCC_6F0829F0();
extern unsigned int GetFourCC_6F110CE0();
extern unsigned int GetFourCC_6F197D80();
extern unsigned int GetFourCC_6F195360();
extern unsigned int GetFourCC_6F159CA0();
extern unsigned int GetFourCC_6F159520();
extern unsigned int GetFourCC_6F194980();
extern unsigned int GetFourCC_6F080500();
extern unsigned int GetFourCC_6F0DCBA0();
extern unsigned int GetFourCC_6F0DD6A0();
extern unsigned int GetFourCC_6F0800D0();
extern unsigned int GetFourCC_6F0DBC80();
extern unsigned int GetFourCC_6F0DD4A0();
extern unsigned int GetFourCC_6F07CB10();
extern unsigned int GetFourCC_6F10FB70();
extern unsigned int GetFourCC_6F1D6740();
extern unsigned int GetFourCC_6F0DB5A0();
extern unsigned int GetFourCC_6F0D9E60();
extern unsigned int GetFourCC_6F0D8FA0();
extern unsigned int GetFourCC_6F1D6180();   // Misc/cunit_agent6_leaves.cpp - already reconstructed

//  What the maker hands back: only the CAgent* at +0x54 is reached (same
//  local shape as widget_slot68.cpp's own SMadeAgentHolder).
struct SMadeAgentHolder
{
    char    m_reserved00[0x54];
    CAgent* m_pAgent;   // +0x54
};

//  0x6F238000
CAgent* MakeCheckedAgent_Aaha()
{
    CAgentPtr_Aaha held(0);

    AGILE_TYPE_ID id(GetFourCC_6F021B40());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F021B40(), pool);

    held.AssignChecked(((SMadeAgentHolder*)
                        SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_ptr;
}

//  0x6F238750
CAgent* MakeCheckedAgent_Aarm()
{
    CAgentPtr_Aarm held(0);

    AGILE_TYPE_ID id(GetFourCC_6F022F10());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F022F10(), pool);

    held.AssignChecked(((SMadeAgentHolder*)
                        SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_ptr;
}

//  0x6F238EA0
CAgent* MakeCheckedAgent_Abrf()
{
    CAgentPtr_Abrf held(0);

    AGILE_TYPE_ID id(GetFourCC_6F196E60());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F196E60(), pool);

    held.AssignChecked(((SMadeAgentHolder*)
                        SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_ptr;
}

//  0x6F239520
CAgent* MakeCheckedAgent_Abur()
{
    CAgentPtr_Abur held(0);

    AGILE_TYPE_ID id(GetFourCC_6F1988C0());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F1988C0(), pool);

    held.AssignChecked(((SMadeAgentHolder*)
                        SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_ptr;
}

//  0x6F239C70
CAgent* MakeCheckedAgent_Achl()
{
    CAgentPtr_Achl held(0);

    AGILE_TYPE_ID id(GetFourCC_6F199040());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F199040(), pool);

    held.AssignChecked(((SMadeAgentHolder*)
                        SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_ptr;
}

//  0x6F23A2F0
CAgent* MakeCheckedAgent_ACad()
{
    CAgentPtr_ACad held(0);

    AGILE_TYPE_ID id(GetFourCC_6F194F90());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F194F90(), pool);

    held.AssignChecked(((SMadeAgentHolder*)
                        SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_ptr;
}

//  0x6F23A970
CAgent* MakeCheckedAgent_Adda()
{
    CAgentPtr_Adda held(0);

    AGILE_TYPE_ID id(GetFourCC_6F1D4640());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F1D4640(), pool);

    held.AssignChecked(((SMadeAgentHolder*)
                        SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_ptr;
}

//  0x6F23B260
CAgent* MakeCheckedAgent_Advc()
{
    CAgentPtr_Advc held(0);

    AGILE_TYPE_ID id(GetFourCC_6F07CCF0());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F07CCF0(), pool);

    held.AssignChecked(((SMadeAgentHolder*)
                        SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_ptr;
}

//  0x6F23B8E0
CAgent* MakeCheckedAgent_Aetl()
{
    CAgentPtr_Aetl held(0);

    AGILE_TYPE_ID id(GetFourCC_6F07F3B0());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F07F3B0(), pool);

    held.AssignChecked(((SMadeAgentHolder*)
                        SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_ptr;
}

//  0x6F23D7C0
CAgent* MakeCheckedAgent_Adts()
{
    CAgentPtr_Adts held(0);

    AGILE_TYPE_ID id(GetFourCC_6F024A90());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F024A90(), pool);

    held.AssignChecked(((SMadeAgentHolder*)
                        SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_ptr;
}

//  0x6F23E8D0
CAgent* MakeCheckedAgent_ANpi()
{
    CAgentPtr_ANpi held(0);

    AGILE_TYPE_ID id(GetFourCC_6F10E7A0());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F10E7A0(), pool);

    held.AssignChecked(((SMadeAgentHolder*)
                        SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_ptr;
}

//  0x6F241CD0
CAgent* MakeCheckedAgent_ANth()
{
    CAgentPtr_ANth held(0);

    AGILE_TYPE_ID id(GetFourCC_6F1569A0());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F1569A0(), pool);

    held.AssignChecked(((SMadeAgentHolder*)
                        SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_ptr;
}

//  0x6F242D10
CAgent* MakeCheckedAgent_AEar()
{
    CAgentPtr_AEar held(0);

    AGILE_TYPE_ID id(GetFourCC_6F195E20());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F195E20(), pool);

    held.AssignChecked(((SMadeAgentHolder*)
                        SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_ptr;
}

//  0x6F243390
CAgent* MakeCheckedAgent_ANbs()
{
    CAgentPtr_ANbs held(0);

    AGILE_TYPE_ID id(GetFourCC_6F197260());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F197260(), pool);

    held.AssignChecked(((SMadeAgentHolder*)
                        SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_ptr;
}

//  0x6F243A10
CAgent* MakeCheckedAgent_ANca()
{
    CAgentPtr_ANca held(0);

    AGILE_TYPE_ID id(GetFourCC_6F1995C0());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F1995C0(), pool);

    held.AssignChecked(((SMadeAgentHolder*)
                        SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_ptr;
}

//  0x6F244090
CAgent* MakeCheckedAgent_AUdd()
{
    CAgentPtr_AUdd held(0);

    AGILE_TYPE_ID id(GetFourCC_6F07BE00());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F07BE00(), pool);

    held.AssignChecked(((SMadeAgentHolder*)
                        SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_ptr;
}

//  0x6F244710
CAgent* MakeCheckedAgent_AOeq()
{
    CAgentPtr_AOeq held(0);

    AGILE_TYPE_ID id(GetFourCC_6F07E0F0());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F07E0F0(), pool);

    held.AssignChecked(((SMadeAgentHolder*)
                        SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_ptr;
}

//  0x6F244D90
CAgent* MakeCheckedAgent_AHfs()
{
    CAgentPtr_AHfs held(0);

    AGILE_TYPE_ID id(GetFourCC_6F081130());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F081130(), pool);

    held.AssignChecked(((SMadeAgentHolder*)
                        SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_ptr;
}

//  0x6F245410
CAgent* MakeCheckedAgent_AOhx()
{
    CAgentPtr_AOhx held(0);

    AGILE_TYPE_ID id(GetFourCC_6F0829F0());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F0829F0(), pool);

    held.AssignChecked(((SMadeAgentHolder*)
                        SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_ptr;
}

//  0x6F245B60
CAgent* MakeCheckedAgent_AEmb()
{
    CAgentPtr_AEmb held(0);

    AGILE_TYPE_ID id(GetFourCC_6F110CE0());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F110CE0(), pool);

    held.AssignChecked(((SMadeAgentHolder*)
                        SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_ptr;
}

//  0x6F2461E0
CAgent* MakeCheckedAgent_ANrf()
{
    CAgentPtr_ANrf held(0);

    AGILE_TYPE_ID id(GetFourCC_6F197D80());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F197D80(), pool);

    held.AssignChecked(((SMadeAgentHolder*)
                        SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_ptr;
}

//  0x6F246860
CAgent* MakeCheckedAgent_ANsl()
{
    CAgentPtr_ANsl held(0);

    AGILE_TYPE_ID id(GetFourCC_6F195360());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F195360(), pool);

    held.AssignChecked(((SMadeAgentHolder*)
                        SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_ptr;
}

//  0x6F246EE0
CAgent* MakeCheckedAgent_ANsq()
{
    CAgentPtr_ANsq held(0);

    AGILE_TYPE_ID id(GetFourCC_6F159CA0());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F159CA0(), pool);

    held.AssignChecked(((SMadeAgentHolder*)
                        SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_ptr;
}

//  0x6F2477D0
CAgent* MakeCheckedAgent_AOwd()
{
    CAgentPtr_AOwd held(0);

    AGILE_TYPE_ID id(GetFourCC_6F159520());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F159520(), pool);

    held.AssignChecked(((SMadeAgentHolder*)
                        SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_ptr;
}

//  0x6F247E50
CAgent* MakeCheckedAgent_ANtm()
{
    CAgentPtr_ANtm held(0);

    AGILE_TYPE_ID id(GetFourCC_6F194980());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F194980(), pool);

    held.AssignChecked(((SMadeAgentHolder*)
                        SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_ptr;
}

//  0x6F2484D0
CAgent* MakeCheckedAgent_ANia()
{
    CAgentPtr_ANia held(0);

    AGILE_TYPE_ID id(GetFourCC_6F080500());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F080500(), pool);

    held.AssignChecked(((SMadeAgentHolder*)
                        SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_ptr;
}

//  0x6F248B50
CAgent* MakeCheckedAgent_AIim()
{
    CAgentPtr_AIim held(0);

    AGILE_TYPE_ID id(GetFourCC_6F0DCBA0());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F0DCBA0(), pool);

    held.AssignChecked(((SMadeAgentHolder*)
                        SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_ptr;
}

//  0x6F2491D0
CAgent* MakeCheckedAgent_AIfm()
{
    CAgentPtr_AIfm held(0);

    AGILE_TYPE_ID id(GetFourCC_6F0DD6A0());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F0DD6A0(), pool);

    held.AssignChecked(((SMadeAgentHolder*)
                        SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_ptr;
}

//  0x6F249850
CAgent* MakeCheckedAgent_AIfu()
{
    CAgentPtr_AIfu held(0);

    AGILE_TYPE_ID id(GetFourCC_6F0800D0());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F0800D0(), pool);

    held.AssignChecked(((SMadeAgentHolder*)
                        SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_ptr;
}

//  0x6F249ED0
CAgent* MakeCheckedAgent_AIlb()
{
    CAgentPtr_AIlb held(0);

    AGILE_TYPE_ID id(GetFourCC_6F0DBC80());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F0DBC80(), pool);

    held.AssignChecked(((SMadeAgentHolder*)
                        SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_ptr;
}

//  0x6F24A620
CAgent* MakeCheckedAgent_AIrb()
{
    CAgentPtr_AIrb held(0);

    AGILE_TYPE_ID id(GetFourCC_6F0DD4A0());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F0DD4A0(), pool);

    held.AssignChecked(((SMadeAgentHolder*)
                        SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_ptr;
}

//  0x6F24ACA0
CAgent* MakeCheckedAgent_AIda()
{
    CAgentPtr_AIda held(0);

    AGILE_TYPE_ID id(GetFourCC_6F07CB10());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F07CB10(), pool);

    held.AssignChecked(((SMadeAgentHolder*)
                        SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_ptr;
}

//  0x6F24B320
CAgent* MakeCheckedAgent_AIpm()
{
    CAgentPtr_AIpm held(0);

    AGILE_TYPE_ID id(GetFourCC_6F10FB70());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F10FB70(), pool);

    held.AssignChecked(((SMadeAgentHolder*)
                        SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_ptr;
}

//  0x6F24B9A0
CAgent* MakeCheckedAgent_AIsp()
{
    CAgentPtr_AIsp held(0);

    AGILE_TYPE_ID id(GetFourCC_6F1D6740());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F1D6740(), pool);

    held.AssignChecked(((SMadeAgentHolder*)
                        SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_ptr;
}

//  0x6F24C020
CAgent* MakeCheckedAgent_AIfc()
{
    CAgentPtr_AIfc held(0);

    AGILE_TYPE_ID id(GetFourCC_6F0DB5A0());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F0DB5A0(), pool);

    held.AssignChecked(((SMadeAgentHolder*)
                        SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_ptr;
}

//  0x6F24C6A0
CAgent* MakeCheckedAgent_AIca()
{
    CAgentPtr_AIca held(0);

    AGILE_TYPE_ID id(GetFourCC_6F0D9E60());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F0D9E60(), pool);

    held.AssignChecked(((SMadeAgentHolder*)
                        SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_ptr;
}

//  0x6F29AE10
CAgent* MakeCheckedAgent_ANbu()
{
    CAgentPtr_ANbu held(0);

    AGILE_TYPE_ID id(GetFourCC_6F0D8FA0());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F0D8FA0(), pool);

    held.AssignChecked(((SMadeAgentHolder*)
                        SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_ptr;
}

//  0x6F1FA010 - the 38th instance ('ACsp'), reached from a different
//  caller/sweep than the other 37 (see cunit_agent1_agentptr_family.h's
//  own note on CAgentPtr_ACsp) - was a thunk in
//  Unit/cunit_agent1_applyabilityeffect.cpp until this session gave it a
//  real body here, alongside its 37 siblings.
CAgent* MakeCheckedAgent_ACsp()
{
    CAgentPtr_ACsp held(0);

    AGILE_TYPE_ID id(GetFourCC_6F1D6180());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F1D6180(), pool);

    held.AssignChecked(((SMadeAgentHolder*)
                        SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_ptr;
}
