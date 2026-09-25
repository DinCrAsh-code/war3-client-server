//============================================================================
//  Thirty-eight instantiations of one shared shape: build a type-checked,
//  reference-counted CAgent* slot for a fixed agile type, look that type up
//  in the game-wide agile type registry (game.h's g_pGameData->m_types) to
//  find its pool, build and submit a widget-agent query against that pool
//  (widgetagentquery.h), and hand back the resolved agent (or null) after
//  the slot's own destructor has run.
//
//  Byte-for-byte the same shape as CWidget::ClearPlayerMaskBits's own
//  "SWidgetAgentSlot held(0); ... held.Assign(...); return held.m_value;"
//  (widget_slot68.cpp) and SWidgetAgentSlot's own ctor/Assign pair
//  (widgetagentslot.h/.cpp) - just one required-type constant baked into
//  each address rather than the single 'hgw+' that family already has a
//  name for.
//
//  The slot's own converting constructor and its Assign are each a
//  distinct address per instantiation, all below this agent-6 slice's own
//  worklist (BFS depth 3+ from CUnit's own vtable) - so, matching the
//  established convention for a required-type constant with no established
//  meaning (checkedslot_batch25.cpp), both stay thunked here rather than
//  guessed at: each hands off to the real, unhooked shipped function, which
//  is the SEH-shaped "zero the value, Assign inside a __try, Release in a
//  __finally" converting constructor and the type-checked Assign every
//  other member of this family already has, just not reconstructed under
//  this session's own slice. Not reconstructions - delete each pair as its
//  real body lands, the way this whole codebase already does for every
//  other member of this family.
//
//  The required-type FourCC leaf (`mov eax,<imm32>; retn`) is thunked the
//  same way for the same reason, and is called twice per function (once
//  for the registry lookup, once for the query block) rather than hoisted
//  into a register - naked so the optimiser cannot see through it and fold
//  the two calls into one, the same reason
//  checkedslot_batch25_getters.cpp gives for its own out-of-line getters.
//============================================================================
#include "agiletype.h"
#include "agent.h"
#include "game.h"
#include "widgetagentquery.h"

namespace {

//  What the maker hands back: only the CAgent* at +0x54 is reached - the
//  same shape widget_slot68.cpp's own SMadeAgentHolder is, redeclared here
//  because that one is private to its own translation unit.
struct SMadeAgentHolderQF
{
    char    m_reserved00[0x54];
    CAgent* m_pAgent;   // +0x54
};

}  // namespace

#define QUERY_FIELD_SLOT(OUTERADDR, CTORADDR, TAGADDR, ASSIGNADDR)          \
namespace { \
struct Slot_##OUTERADDR \
{ \
    /* CTORADDR - thunked; see this file's own header comment. */ \
    Slot_##OUTERADDR(CAgent* candidate); \
    /* ASSIGNADDR - thunked; see this file's own header comment. */ \
    Slot_##OUTERADDR* Assign(CAgent* candidate); \
    ~Slot_##OUTERADDR() \
    { \
        CAgent* held = m_value; \
        if (held) \
        { \
            if (--held->m_refcount == 0) \
                held->ReleaseSelf(); \
        } \
    } \
    CAgent* m_value; \
}; \
__declspec(naked) Slot_##OUTERADDR::Slot_##OUTERADDR(CAgent*) \
{ \
    __asm { mov eax, CTORADDR } \
    __asm { jmp eax } \
} \
__declspec(naked) Slot_##OUTERADDR* Slot_##OUTERADDR::Assign(CAgent*) \
{ \
    __asm { mov eax, ASSIGNADDR } \
    __asm { jmp eax } \
} \
/* TAGADDR - the required-type FourCC. */ \
extern "C" __declspec(naked) unsigned int GetTag_##OUTERADDR() \
{ \
    __asm { mov eax, TAGADDR } \
    __asm { retn } \
} \
} \
CAgent* ResolveAgentQueryField_##OUTERADDR() \
{ \
    Slot_##OUTERADDR slot(0); \
\
    AGILE_TYPE_ID id(GetTag_##OUTERADDR()); \
    CGameData* game = g_pGameData; \
    unsigned int hash = id.Hash(); \
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id); \
    void* pool = type->m_allocator; \
\
    SWidgetAgentQuery query; \
    InitWidgetAgentQuery(&query, GetTag_##OUTERADDR(), pool); \
    slot.Assign(((SMadeAgentHolderQF*) \
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent); \
\
    return slot.m_value; \
}

//  0x6F238410 - 'Aabr' (41616272h).  ctor sub_6F220BE0 @ 0x6F220BE0, tag @ 0x6F022F90, Assign sub_6F211C80 @ 0x6F211C80.
QUERY_FIELD_SLOT(6F238410, 0x6F220BE0, 0x41616272, 0x6F211C80)

//  0x6F238B60 - 'Abof' (41626F66h).  ctor sub_6F220F40 @ 0x6F220F40, tag @ 0x6F15AB90, Assign sub_6F212100 @ 0x6F212100.
QUERY_FIELD_SLOT(6F238B60, 0x6F220F40, 0x41626F66, 0x6F212100)

//  0x6F2392B0 - 'Ablo' (41626C6Fh).  ctor sub_6F2212A0 @ 0x6F2212A0, tag @ 0x6F197F80, Assign sub_6F212580 @ 0x6F212580.
QUERY_FIELD_SLOT(6F2392B0, 0x6F2212A0, 0x41626C6F, 0x6F212580)

//  0x6F239930 - 'Achd' (41636864h).  ctor sub_6F2214E0 @ 0x6F2214E0, tag @ 0x6F0D9C60, Assign sub_6F212880 @ 0x6F212880.
QUERY_FIELD_SLOT(6F239930, 0x6F2214E0, 0x41636864, 0x6F212880)

//  0x6F23A080 - 'Acoa' (41636F61h).  ctor sub_6F221720 @ 0x6F221720, tag @ 0x6F19A1B0, Assign sub_6F212B80 @ 0x6F212B80.
QUERY_FIELD_SLOT(6F23A080, 0x6F221720, 0x41636F61, 0x6F212B80)

//  0x6F23A700 - 'Acri' (41637269h).  ctor sub_6F2219C0 @ 0x6F2219C0, tag @ 0x6F19A840, Assign sub_6F212F00 @ 0x6F212F00.
QUERY_FIELD_SLOT(6F23A700, 0x6F2219C0, 0x41637269, 0x6F212F00)

//  0x6F23AFF0 - 'Adtn' (4164746Eh).  ctor sub_6F221D20 @ 0x6F221D20, tag @ 0x6F0D9AE0, Assign sub_6F213380 @ 0x6F213380.
QUERY_FIELD_SLOT(6F23AFF0, 0x6F221D20, 0x4164746E, 0x6F213380)

//  0x6F23B670 - 'Aent' (41656E74h).  ctor sub_6F222020 @ 0x6F222020, tag @ 0x6F0DA090, Assign sub_6F213780 @ 0x6F213780.
QUERY_FIELD_SLOT(6F23B670, 0x6F222020, 0x41656E74, 0x6F213780)

//  0x6F23BCF0 - 'Afbk' (4166626Bh).  ctor sub_6F222320 @ 0x6F222320, tag @ 0x6F15A890, Assign sub_6F213B80 @ 0x6F213B80.
QUERY_FIELD_SLOT(6F23BCF0, 0x6F222320, 0x4166626B, 0x6F213B80)

//  0x6F23E590 - 'ANsp' (414E7370h).  ctor sub_6F223340 @ 0x6F223340, tag @ 0x6F024CB0, Assign sub_6F215100 @ 0x6F215100.
QUERY_FIELD_SLOT(6F23E590, 0x6F223340, 0x414E7370, 0x6F215100)

//  0x6F240D60 - 'ANse' (414E7365h).  ctor sub_6F224420 @ 0x6F224420, tag @ 0x6F1D6B00, Assign sub_6F216780 @ 0x6F216780.
QUERY_FIELD_SLOT(6F240D60, 0x6F224420, 0x414E7365, 0x6F216780)

//  0x6F242AA0 - 'AHad' (41486164h).  ctor sub_6F225020 @ 0x6F225020, tag @ 0x6F196000, Assign sub_6F217780 @ 0x6F217780.
QUERY_FIELD_SLOT(6F242AA0, 0x6F225020, 0x41486164, 0x6F217780)

//  0x6F243120 - 'AHbh' (41486268h).  ctor sub_6F225320 @ 0x6F225320, tag @ 0x6F196E00, Assign sub_6F217B80 @ 0x6F217B80.
QUERY_FIELD_SLOT(6F243120, 0x6F225320, 0x41486268, 0x6F217B80)

//  0x6F2437A0 - 'AUcs' (41556373h).  ctor sub_6F225620 @ 0x6F225620, tag @ 0x6F1D5680, Assign sub_6F217F80 @ 0x6F217F80.
QUERY_FIELD_SLOT(6F2437A0, 0x6F225620, 0x41556373, 0x6F217F80)

//  0x6F243E20 - 'AUdr' (41556472h).  ctor sub_6F2258C0 @ 0x6F2258C0, tag @ 0x6F07BF40, Assign sub_6F218300 @ 0x6F218300.
QUERY_FIELD_SLOT(6F243E20, 0x6F2258C0, 0x41556472, 0x6F218300)

//  0x6F2444A0 - 'AHdr' (41486472h).  ctor sub_6F225BC0 @ 0x6F225BC0, tag @ 0x6F07D9D0, Assign sub_6F218700 @ 0x6F218700.
QUERY_FIELD_SLOT(6F2444A0, 0x6F225BC0, 0x41486472, 0x6F218700)

//  0x6F244B20 - 'AOfs' (414F6673h).  ctor sub_6F225EC0 @ 0x6F225EC0, tag @ 0x6F024B70, Assign sub_6F218B00 @ 0x6F218B00.
QUERY_FIELD_SLOT(6F244B20, 0x6F225EC0, 0x414F6673, 0x6F218B00)

//  0x6F2451A0 - 'AUfu' (41556675h).  ctor sub_6F2261C0 @ 0x6F2261C0, tag @ 0x6F082180, Assign sub_6F218F00 @ 0x6F218F00.
QUERY_FIELD_SLOT(6F2451A0, 0x6F2261C0, 0x41556675, 0x6F218F00)

//  0x6F245820 - 'ANin' (414E696Eh).  ctor sub_6F2264C0 @ 0x6F2264C0, tag @ 0x6F10EF60, Assign sub_6F219300 @ 0x6F219300.
QUERY_FIELD_SLOT(6F245820, 0x6F2264C0, 0x414E696E, 0x6F219300)

//  0x6F245F70 - 'ANmo' (414E6D6Fh).  ctor sub_6F226820 @ 0x6F226820, tag @ 0x6F158960, Assign sub_6F219780 @ 0x6F219780.
QUERY_FIELD_SLOT(6F245F70, 0x6F226820, 0x414E6D6F, 0x6F219780)

//  0x6F2465F0 - 'ANsi' (414E7369h).  ctor sub_6F226B20 @ 0x6F226B20, tag @ 0x6F1D5D80, Assign sub_6F219B80 @ 0x6F219B80.
QUERY_FIELD_SLOT(6F2465F0, 0x6F226B20, 0x414E7369, 0x6F219B80)

//  0x6F246C70 - 'AOws' (414F7773h).  ctor sub_6F226E20 @ 0x6F226E20, tag @ 0x6F1570A0, Assign sub_6F219F80 @ 0x6F219F80.
QUERY_FIELD_SLOT(6F246C70, 0x6F226E20, 0x414F7773, 0x6F219F80)

//  0x6F2472F0 - 'ANto' (414E746Fh).  ctor sub_6F227120 @ 0x6F227120, tag @ 0x6F157FE0, Assign sub_6F21A380 @ 0x6F21A380.
QUERY_FIELD_SLOT(6F2472F0, 0x6F227120, 0x414E746F, 0x6F21A380)

//  0x6F247BE0 - 'ANab' (414E6162h).  ctor sub_6F227540 @ 0x6F227540, tag @ 0x6F194700, Assign sub_6F21A900 @ 0x6F21A900.
QUERY_FIELD_SLOT(6F247BE0, 0x6F227540, 0x414E6162, 0x6F21A900)

//  0x6F248260 - 'ANrg' (414E7267h).  ctor sub_6F2277E0 @ 0x6F2277E0, tag @ 0x6F157880, Assign sub_6F21AC80 @ 0x6F21AC80.
QUERY_FIELD_SLOT(6F248260, 0x6F2277E0, 0x414E7267, 0x6F21AC80)

//  0x6F2488E0 - 'AIxm' (4149786Dh).  ctor sub_6F227AE0 @ 0x6F227AE0, tag @ 0x6F0DCA20, Assign sub_6F21B080 @ 0x6F21B080.
QUERY_FIELD_SLOT(6F2488E0, 0x6F227AE0, 0x4149786D, 0x6F21B080)

//  0x6F248F60 - 'AIde' (41496465h).  ctor sub_6F227DE0 @ 0x6F227DE0, tag @ 0x6F0DC920, Assign sub_6F21B480 @ 0x6F21B480.
QUERY_FIELD_SLOT(6F248F60, 0x6F227DE0, 0x41496465, 0x6F21B480)

//  0x6F2495E0 - 'AIff' (41496666h).  ctor sub_6F2280E0 @ 0x6F2280E0, tag @ 0x6F07FF50, Assign sub_6F21B880 @ 0x6F21B880.
QUERY_FIELD_SLOT(6F2495E0, 0x6F2280E0, 0x41496666, 0x6F21B880)

//  0x6F249C60 - 'AIob' (41496F62h).  ctor sub_6F2283E0 @ 0x6F2283E0, tag @ 0x6F0DBC00, Assign sub_6F21BC80 @ 0x6F21BC80.
QUERY_FIELD_SLOT(6F249C60, 0x6F2283E0, 0x41496F62, 0x6F21BC80)

//  0x6F24A2E0 - 'AImn' (41496D6Eh).  ctor sub_6F2286E0 @ 0x6F2286E0, tag @ 0x6F0DD2A0, Assign sub_6F21C080 @ 0x6F21C080.
QUERY_FIELD_SLOT(6F24A2E0, 0x6F2286E0, 0x41496D6E, 0x6F21C080)

//  0x6F24AA30 - 'AIcf' (41496366h).  ctor sub_6F2289E0 @ 0x6F2289E0, tag @ 0x6F10E880, Assign sub_6F21C480 @ 0x6F21C480.
QUERY_FIELD_SLOT(6F24AA30, 0x6F2289E0, 0x41496366, 0x6F21C480)

//  0x6F24B0B0 - 'AIil' (4149696Ch).  ctor sub_6F228CE0 @ 0x6F228CE0, tag @ 0x6F112780, Assign sub_6F21C880 @ 0x6F21C880.
QUERY_FIELD_SLOT(6F24B0B0, 0x6F228CE0, 0x4149696C, 0x6F21C880)

//  0x6F24B730 - 'AIre' (41497265h).  ctor sub_6F228FE0 @ 0x6F228FE0, tag @ 0x6F1D3260, Assign sub_6F21CC80 @ 0x6F21CC80.
QUERY_FIELD_SLOT(6F24B730, 0x6F228FE0, 0x41497265, 0x6F21CC80)

//  0x6F24BDB0 - 'AIpi' (41497069h).  ctor sub_6F2292E0 @ 0x6F2292E0, tag @ 0x6F10F880, Assign sub_6F21D080 @ 0x6F21D080.
QUERY_FIELD_SLOT(6F24BDB0, 0x6F2292E0, 0x41497069, 0x6F21D080)

//  0x6F24C430 - 'AIwb' (41497762h).  ctor sub_6F229580 @ 0x6F229580, tag @ 0x6F07EE60, Assign sub_6F21D400 @ 0x6F21D400.
QUERY_FIELD_SLOT(6F24C430, 0x6F229580, 0x41497762, 0x6F21D400)

//  0x6F24CAB0 - 'AIpv' (41497076h).  ctor sub_6F229880 @ 0x6F229880, tag @ 0x6F0DDA20, Assign sub_6F21D800 @ 0x6F21D800.
QUERY_FIELD_SLOT(6F24CAB0, 0x6F229880, 0x41497076, 0x6F21D800)

//  0x6F29ABA0 - 'AObu' (414F6275h).  ctor sub_6F280DE0 @ 0x6F280DE0, tag @ 0x6F0D90A0, Assign sub_6F2755B0 @ 0x6F2755B0.
QUERY_FIELD_SLOT(6F29ABA0, 0x6F280DE0, 0x414F6275, 0x6F2755B0)

//  0x6F29B220 - 'Aupg' (41757067h).  ctor sub_6F2810E0 @ 0x6F2810E0, tag @ 0x6F15C300, Assign sub_6F2759B0 @ 0x6F2759B0.
QUERY_FIELD_SLOT(6F29B220, 0x6F2810E0, 0x41757067, 0x6F2759B0)

#undef QUERY_FIELD_SLOT
