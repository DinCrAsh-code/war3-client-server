//============================================================================
//  A family of near-identical "make a fresh hidden-ability CAgent"
//  factories, reached from CUnit's vtable BFS closure.  Several independent
//  agent sessions worked disjoint, interleaved address ranges of the same
//  compiler-generated shape (the address-sorted slice cunit_agent8_worklist.md
//  plus further slices reached from other BFS roots/depths of the same CUnit
//  vtable-closure sweep) and landed their finds in this one shared file - the
//  shipped binary places every instantiation of this shape together in the
//  0x6F237F30-0x6F24C0F0 ctor/getter address range (plus at least one
//  further-out outlier, 0x6F1F9F40, see its own block below), and no single
//  session's worklist slice reached all of them.  See the count of
//  `MakeAbilityAgent_` definitions actually present in this file for the
//  current total rather than trusting any one session's tally in an older
//  comment here - each prior session wrote its own snapshot count as it
//  landed its slice, and those numbers go stale the moment another slice
//  merges in.  Every member is instruction-for-instruction the same shape
//  as CWidget::ClearPlayerMaskBits's own agent build (0x6F2AF6A0,
//  widget_slot68.cpp) with three addresses substituted per instance - a
//  type-checked slot constructor, a rawcode getter (called twice, exactly
//  as GetWidgetAgileTypeId is - except 0x6F1F9F40, called once; see its own
//  comment), and that slot's own Assign - and the same four
//  already-reconstructed shared calls in between (AGILE_TYPE_ID::Hash
//  0x6F4C8520, TSHashTable::Ptr 0x6F001EC0, InitWidgetAgentQuery
//  0x6F472C20, SubmitWidgetAgentQuery 0x6F48AB80) plus the same
//  g_pGameData global (0x6FAB7368).
//
//  Every rawcode getter is a plain `mov eax, <imm32> / retn` leaf (the same
//  shape misc_rawcode_getters.cpp already catalogues) and every one of them
//  decodes to a real four-character, 'A'-first ability rawcode (WC3's own
//  object-editor convention for a hidden/internal ability) - "Aabs",
//  "Aapl", "AIdd", and so on below.  Nothing in any dump here says which
//  named ability each one really is, so - per misc_rawcode_getters.cpp's own
//  rule for the FourCCs it can't place - each keeps its literal rawcode
//  rather than a guessed name.  Every getter is a self-redirect thunk rather
//  than a real `return 0x...;` body, even where agent_worktrees now has
//  the confirming raw_asm/raw_bytes: this build has no whole-program
//  optimisation, but a real one-line definition living in the *same* TU
//  as its two call sites is still visible to /O2's own inliner, which
//  folds both away and drops the `call` instructions the shipped dump
//  plainly keeps (misc_rawcode_getters.cpp's own leaves get away with a
//  real body only because every one of *their* callers lives in a
//  different TU, where there is nothing to inline across). A naked body
//  is opaque to the optimiser and can never be inlined, which is the
//  actual reason every getter here is thunked - a real build with a real
//  `return 0x...;` definition regressed MakeAbilityAgent_Acan from its
//  usual ~0.49 to a shorter, wrong-shaped listing the first time this file
//  tried it that way.
//
//  The type-checked slot's own constructor and Assign are declared here,
//  not defined: neither address is in any one agent's worklist slice (BFS
//  reached them one hop further out, past the cut each session worked to),
//  so their bodies are somebody else's reconstruction - most likely a
//  sibling agent working an adjacent slice of the same BFS closure
//  concurrently.  Declaring them lets this translation unit compile and
//  score its own addresses; their exact addresses are named in each block's
//  comment so a later session that reconstructs one can wire a real
//  definition straight in.  Every one of them is byte-for-byte the
//  SCheckedXXXSlot::Assign shape agenttypedslots.h already documents
//  (candidate's own agile type id, checked with AgileTypeIsDerivedFrom
//  against the getter's rawcode, refcount swap on the old/new value) - see
//  unittypedassign.cpp for the exact body once one of these addresses gets
//  its own dump.  Per the "Link-completion thunks" comment below, each is
//  defined as a self-redirect thunk rather than left merely declared,
//  specifically so a same-address, differently-named reconstruction some
//  other concurrent agent lands does not leave this TU with an unresolved
//  extern.
//
//  Same unreproducible __except_handler4-shaped SEH frame every destructible
//  local in this family carries (docs/msvc-vc8-idioms.md, widget_slot68.cpp)
//  - this build's /GS- reaches a different, reproducible __except_handler3
//  frame instead, so every one of these is capped well short of EXACT by
//  frame bytes alone; the behaviour verdict for each says so.
//============================================================================
#include "agent.h"
#include "agiletype.h"
#include "game.h"
#include "widgetagentquery.h"

//  What SubmitWidgetAgentQuery hands back: only the CAgent* at +0x54 is
//  reached (same reasoning as widget_slot68.cpp's own copy - a private,
//  per-TU redeclaration rather than a shared header, since neither file
//  needs more than the one field).
namespace {
struct SMadeAgentHolder
{
    char    m_reserved00[0x54];
    CAgent* m_pAgent;   // +0x54
};
}

//  0x6F237F30 - MakeAbilityAgent_Aabs.  Type-checked slot ctor sub_6F220A00 @ 0x6F220A00,
//  Assign sub_6F211A00 @ 0x6F211A00, rawcode getter sub_6F07C0E0 @ 0x6F07C0E0
//  ('Aabs' = 0x41616273).
struct SCheckedAbilitySlot_Aabs
{
    SCheckedAbilitySlot_Aabs(CAgent* candidate);          //  0x6F220A00
    SCheckedAbilitySlot_Aabs* Assign(CAgent* candidate);  //  0x6F211A00

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_Aabs() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F07C0E0();   //  0x6F07C0E0 - 'Aabs'

CAgent* MakeAbilityAgent_Aabs()
{
    SCheckedAbilitySlot_Aabs held(0);

    unsigned int typeId = GetFourCC_6F07C0E0();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F07C0E0(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F2385B0 - MakeAbilityAgent_Aapl.  Type-checked slot ctor sub_6F220CA0 @ 0x6F220CA0,
//  Assign sub_6F211D80 @ 0x6F211D80, rawcode getter sub_6F1962A0 @ 0x6F1962A0
//  ('Aapl' = 0x4161706c).
struct SCheckedAbilitySlot_Aapl
{
    SCheckedAbilitySlot_Aapl(CAgent* candidate);          //  0x6F220CA0
    SCheckedAbilitySlot_Aapl* Assign(CAgent* candidate);  //  0x6F211D80

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_Aapl() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F1962A0();   //  0x6F1962A0 - 'Aapl'

CAgent* MakeAbilityAgent_Aapl()
{
    SCheckedAbilitySlot_Aapl held(0);

    unsigned int typeId = GetFourCC_6F1962A0();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F1962A0(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F238DD0 - MakeAbilityAgent_Abtl.  Type-checked slot ctor sub_6F221060 @ 0x6F221060,
//  Assign sub_6F212280 @ 0x6F212280, rawcode getter sub_6F1986A0 @ 0x6F1986A0
//  ('Abtl' = 0x4162746c).
struct SCheckedAbilitySlot_Abtl
{
    SCheckedAbilitySlot_Abtl(CAgent* candidate);          //  0x6F221060
    SCheckedAbilitySlot_Abtl* Assign(CAgent* candidate);  //  0x6F212280

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_Abtl() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F1986A0();   //  0x6F1986A0 - 'Abtl'

CAgent* MakeAbilityAgent_Abtl()
{
    SCheckedAbilitySlot_Abtl held(0);

    unsigned int typeId = GetFourCC_6F1986A0();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F1986A0(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F239450 - MakeAbilityAgent_Abun.  Type-checked slot ctor sub_6F221360 @ 0x6F221360,
//  Assign sub_6F212680 @ 0x6F212680, rawcode getter sub_6F198620 @ 0x6F198620
//  ('Abun' = 0x4162756e).
struct SCheckedAbilitySlot_Abun
{
    SCheckedAbilitySlot_Abun(CAgent* candidate);          //  0x6F221360
    SCheckedAbilitySlot_Abun* Assign(CAgent* candidate);  //  0x6F212680

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_Abun() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F198620();   //  0x6F198620 - 'Abun'

CAgent* MakeAbilityAgent_Abun()
{
    SCheckedAbilitySlot_Abun held(0);

    unsigned int typeId = GetFourCC_6F198620();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F198620(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F239BA0 - MakeAbilityAgent_Acha.  Type-checked slot ctor sub_6F2215A0 @ 0x6F2215A0,
//  Assign sub_6F212980 @ 0x6F212980, rawcode getter sub_6F198FC0 @ 0x6F198FC0
//  ('Acha' = 0x41636861).
struct SCheckedAbilitySlot_Acha
{
    SCheckedAbilitySlot_Acha(CAgent* candidate);          //  0x6F2215A0
    SCheckedAbilitySlot_Acha* Assign(CAgent* candidate);  //  0x6F212980

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_Acha() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F198FC0();   //  0x6F198FC0 - 'Acha'

CAgent* MakeAbilityAgent_Acha()
{
    SCheckedAbilitySlot_Acha held(0);

    unsigned int typeId = GetFourCC_6F198FC0();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F198FC0(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F23A220 - MakeAbilityAgent_Acoi.  Type-checked slot ctor sub_6F092760 @ 0x6F092760,
//  Assign sub_6F084B70 @ 0x6F084B70, rawcode getter sub_6F19A2B0 @ 0x6F19A2B0
//  ('Acoi' = 0x41636f69).
struct SCheckedAbilitySlot_Acoi
{
    SCheckedAbilitySlot_Acoi(CAgent* candidate);          //  0x6F092760
    SCheckedAbilitySlot_Acoi* Assign(CAgent* candidate);  //  0x6F084B70

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_Acoi() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F19A2B0();   //  0x6F19A2B0 - 'Acoi'

CAgent* MakeAbilityAgent_Acoi()
{
    SCheckedAbilitySlot_Acoi held(0);

    unsigned int typeId = GetFourCC_6F19A2B0();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F19A2B0(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F23A8A0 - MakeAbilityAgent_Acyc.  Type-checked slot ctor sub_6F221A80 @ 0x6F221A80,
//  Assign sub_6F213000 @ 0x6F213000, rawcode getter sub_6F19ACA0 @ 0x6F19ACA0
//  ('Acyc' = 0x41637963).
struct SCheckedAbilitySlot_Acyc
{
    SCheckedAbilitySlot_Acyc(CAgent* candidate);          //  0x6F221A80
    SCheckedAbilitySlot_Acyc* Assign(CAgent* candidate);  //  0x6F213000

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_Acyc() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F19ACA0();   //  0x6F19ACA0 - 'Acyc'

CAgent* MakeAbilityAgent_Acyc()
{
    SCheckedAbilitySlot_Acyc held(0);

    unsigned int typeId = GetFourCC_6F19ACA0();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F19ACA0(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F23B190 - MakeAbilityAgent_Advm.  Type-checked slot ctor sub_6F221DE0 @ 0x6F221DE0,
//  Assign sub_6F213480 @ 0x6F213480, rawcode getter sub_6F07CFA0 @ 0x6F07CFA0
//  ('Advm' = 0x4164766d).
struct SCheckedAbilitySlot_Advm
{
    SCheckedAbilitySlot_Advm(CAgent* candidate);          //  0x6F221DE0
    SCheckedAbilitySlot_Advm* Assign(CAgent* candidate);  //  0x6F213480

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_Advm() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F07CFA0();   //  0x6F07CFA0 - 'Advm'

CAgent* MakeAbilityAgent_Advm()
{
    SCheckedAbilitySlot_Advm held(0);

    unsigned int typeId = GetFourCC_6F07CFA0();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F07CFA0(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F23B810 - MakeAbilityAgent_Aegm.  Type-checked slot ctor sub_6F2220E0 @ 0x6F2220E0,
//  Assign sub_6F213880 @ 0x6F213880, rawcode getter sub_6F0DA110 @ 0x6F0DA110
//  ('Aegm' = 0x4165676d).
struct SCheckedAbilitySlot_Aegm
{
    SCheckedAbilitySlot_Aegm(CAgent* candidate);          //  0x6F2220E0
    SCheckedAbilitySlot_Aegm* Assign(CAgent* candidate);  //  0x6F213880

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_Aegm() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F0DA110();   //  0x6F0DA110 - 'Aegm'

CAgent* MakeAbilityAgent_Aegm()
{
    SCheckedAbilitySlot_Aegm held(0);

    unsigned int typeId = GetFourCC_6F0DA110();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F0DA110(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F23C510 - MakeAbilityAgent_Aeth.  Type-checked slot ctor sub_6F222680 @ 0x6F222680,
//  Assign sub_6F214000 @ 0x6F214000, rawcode getter sub_6F10F780 @ 0x6F10F780
//  ('Aeth' = 0x41657468).
struct SCheckedAbilitySlot_Aeth
{
    SCheckedAbilitySlot_Aeth(CAgent* candidate);          //  0x6F222680
    SCheckedAbilitySlot_Aeth* Assign(CAgent* candidate);  //  0x6F214000

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_Aeth() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F10F780();   //  0x6F10F780 - 'Aeth'

CAgent* MakeAbilityAgent_Aeth()
{
    SCheckedAbilitySlot_Aeth held(0);

    unsigned int typeId = GetFourCC_6F10F780();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F10F780(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F23E800 - MakeAbilityAgent_AIdd.  Type-checked slot ctor sub_6F223460 @ 0x6F223460,
//  Assign sub_6F215280 @ 0x6F215280, rawcode getter sub_6F07C840 @ 0x6F07C840
//  ('AIdd' = 0x41496464).
struct SCheckedAbilitySlot_AIdd
{
    SCheckedAbilitySlot_AIdd(CAgent* candidate);          //  0x6F223460
    SCheckedAbilitySlot_AIdd* Assign(CAgent* candidate);  //  0x6F215280

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_AIdd() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F07C840();   //  0x6F07C840 - 'AIdd'

CAgent* MakeAbilityAgent_AIdd()
{
    SCheckedAbilitySlot_AIdd held(0);

    unsigned int typeId = GetFourCC_6F07C840();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F07C840(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F2417F0 - MakeAbilityAgent_ANsu.  Type-checked slot ctor sub_6F2248A0 @ 0x6F2248A0,
//  Assign sub_6F216D80 @ 0x6F216D80, rawcode getter sub_6F198940 @ 0x6F198940
//  ('ANsu' = 0x414e7375).
struct SCheckedAbilitySlot_ANsu
{
    SCheckedAbilitySlot_ANsu(CAgent* candidate);          //  0x6F2248A0
    SCheckedAbilitySlot_ANsu* Assign(CAgent* candidate);  //  0x6F216D80

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_ANsu() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F198940();   //  0x6F198940 - 'ANsu'

CAgent* MakeAbilityAgent_ANsu()
{
    SCheckedAbilitySlot_ANsu held(0);

    unsigned int typeId = GetFourCC_6F198940();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F198940(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F242C40 - MakeAbilityAgent_AEah.  Type-checked slot ctor sub_6F2250E0 @ 0x6F2250E0,
//  Assign sub_6F217880 @ 0x6F217880, rawcode getter sub_6F1964C0 @ 0x6F1964C0
//  ('AEah' = 0x41456168).
struct SCheckedAbilitySlot_AEah
{
    SCheckedAbilitySlot_AEah(CAgent* candidate);          //  0x6F2250E0
    SCheckedAbilitySlot_AEah* Assign(CAgent* candidate);  //  0x6F217880

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_AEah() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F1964C0();   //  0x6F1964C0 - 'AEah'

CAgent* MakeAbilityAgent_AEah()
{
    SCheckedAbilitySlot_AEah held(0);

    unsigned int typeId = GetFourCC_6F1964C0();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F1964C0(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F2432C0 - MakeAbilityAgent_ANba.  Type-checked slot ctor sub_6F2253E0 @ 0x6F2253E0,
//  Assign sub_6F217C80 @ 0x6F217C80, rawcode getter sub_6F1971E0 @ 0x6F1971E0
//  ('ANba' = 0x414e6261).
struct SCheckedAbilitySlot_ANba
{
    SCheckedAbilitySlot_ANba(CAgent* candidate);          //  0x6F2253E0
    SCheckedAbilitySlot_ANba* Assign(CAgent* candidate);  //  0x6F217C80

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_ANba() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F1971E0();   //  0x6F1971E0 - 'ANba'

CAgent* MakeAbilityAgent_ANba()
{
    SCheckedAbilitySlot_ANba held(0);

    unsigned int typeId = GetFourCC_6F1971E0();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F1971E0(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F243940 - MakeAbilityAgent_ANch.  Type-checked slot ctor sub_6F225680 @ 0x6F225680,
//  Assign sub_6F218000 @ 0x6F218000, rawcode getter sub_6F199440 @ 0x6F199440
//  ('ANch' = 0x414e6368).
struct SCheckedAbilitySlot_ANch
{
    SCheckedAbilitySlot_ANch(CAgent* candidate);          //  0x6F225680
    SCheckedAbilitySlot_ANch* Assign(CAgent* candidate);  //  0x6F218000

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_ANch() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F199440();   //  0x6F199440 - 'ANch'

CAgent* MakeAbilityAgent_ANch()
{
    SCheckedAbilitySlot_ANch held(0);

    unsigned int typeId = GetFourCC_6F199440();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F199440(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F243FC0 - MakeAbilityAgent_AUdc.  Type-checked slot ctor sub_6F225980 @ 0x6F225980,
//  Assign sub_6F218400 @ 0x6F218400, rawcode getter sub_6F082BF0 @ 0x6F082BF0
//  ('AUdc' = 0x41556463).
struct SCheckedAbilitySlot_AUdc
{
    SCheckedAbilitySlot_AUdc(CAgent* candidate);          //  0x6F225980
    SCheckedAbilitySlot_AUdc* Assign(CAgent* candidate);  //  0x6F218400

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_AUdc() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F082BF0();   //  0x6F082BF0 - 'AUdc'

CAgent* MakeAbilityAgent_AUdc()
{
    SCheckedAbilitySlot_AUdc held(0);

    unsigned int typeId = GetFourCC_6F082BF0();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F082BF0(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F244640 - MakeAbilityAgent_ANdh.  Type-checked slot ctor sub_6F225C80 @ 0x6F225C80,
//  Assign sub_6F218800 @ 0x6F218800, rawcode getter sub_6F07DED0 @ 0x6F07DED0
//  ('ANdh' = 0x414e6468).
struct SCheckedAbilitySlot_ANdh
{
    SCheckedAbilitySlot_ANdh(CAgent* candidate);          //  0x6F225C80
    SCheckedAbilitySlot_ANdh* Assign(CAgent* candidate);  //  0x6F218800

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_ANdh() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F07DED0();   //  0x6F07DED0 - 'ANdh'

CAgent* MakeAbilityAgent_ANdh()
{
    SCheckedAbilitySlot_ANdh held(0);

    unsigned int typeId = GetFourCC_6F07DED0();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F07DED0(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F244CC0 - MakeAbilityAgent_ANfb.  Type-checked slot ctor sub_6F225F80 @ 0x6F225F80,
//  Assign sub_6F218C00 @ 0x6F218C00, rawcode getter sub_6F156D40 @ 0x6F156D40
//  ('ANfb' = 0x414e6662).
struct SCheckedAbilitySlot_ANfb
{
    SCheckedAbilitySlot_ANfb(CAgent* candidate);          //  0x6F225F80
    SCheckedAbilitySlot_ANfb* Assign(CAgent* candidate);  //  0x6F218C00

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_ANfb() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F156D40();   //  0x6F156D40 - 'ANfb'

CAgent* MakeAbilityAgent_ANfb()
{
    SCheckedAbilitySlot_ANfb held(0);

    unsigned int typeId = GetFourCC_6F156D40();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F156D40(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F245340 - MakeAbilityAgent_AOhw.  Type-checked slot ctor sub_6F226280 @ 0x6F226280,
//  Assign sub_6F219000 @ 0x6F219000, rawcode getter sub_6F082970 @ 0x6F082970
//  ('AOhw' = 0x414f6877).
struct SCheckedAbilitySlot_AOhw
{
    SCheckedAbilitySlot_AOhw(CAgent* candidate);          //  0x6F226280
    SCheckedAbilitySlot_AOhw* Assign(CAgent* candidate);  //  0x6F219000

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_AOhw() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F082970();   //  0x6F082970 - 'AOhw'

CAgent* MakeAbilityAgent_AOhw()
{
    SCheckedAbilitySlot_AOhw held(0);

    unsigned int typeId = GetFourCC_6F082970();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F082970(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F2459C0 - MakeAbilityAgent_AUls.  Type-checked slot ctor sub_6F226580 @ 0x6F226580,
//  Assign sub_6F219400 @ 0x6F219400, rawcode getter sub_6F110240 @ 0x6F110240
//  ('AUls' = 0x41556c73).
struct SCheckedAbilitySlot_AUls
{
    SCheckedAbilitySlot_AUls(CAgent* candidate);          //  0x6F226580
    SCheckedAbilitySlot_AUls* Assign(CAgent* candidate);  //  0x6F219400

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_AUls() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F110240();   //  0x6F110240 - 'AUls'

CAgent* MakeAbilityAgent_AUls()
{
    SCheckedAbilitySlot_AUls held(0);

    unsigned int typeId = GetFourCC_6F110240();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F110240(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F246110 - MakeAbilityAgent_ANrc.  Type-checked slot ctor sub_6F2268E0 @ 0x6F2268E0,
//  Assign sub_6F219880 @ 0x6F219880, rawcode getter sub_6F1954E0 @ 0x6F1954E0
//  ('ANrc' = 0x414e7263).
struct SCheckedAbilitySlot_ANrc
{
    SCheckedAbilitySlot_ANrc(CAgent* candidate);          //  0x6F2268E0
    SCheckedAbilitySlot_ANrc* Assign(CAgent* candidate);  //  0x6F219880

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_ANrc() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F1954E0();   //  0x6F1954E0 - 'ANrc'

CAgent* MakeAbilityAgent_ANrc()
{
    SCheckedAbilitySlot_ANrc held(0);

    unsigned int typeId = GetFourCC_6F1954E0();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F1954E0(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F246790 - MakeAbilityAgent_AUsl.  Type-checked slot ctor sub_6F226BE0 @ 0x6F226BE0,
//  Assign sub_6F219C80 @ 0x6F219C80, rawcode getter sub_6F1D6080 @ 0x6F1D6080
//  ('AUsl' = 0x4155736c).
struct SCheckedAbilitySlot_AUsl
{
    SCheckedAbilitySlot_AUsl(CAgent* candidate);          //  0x6F226BE0
    SCheckedAbilitySlot_AUsl* Assign(CAgent* candidate);  //  0x6F219C80

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_AUsl() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F1D6080();   //  0x6F1D6080 - 'AUsl'

CAgent* MakeAbilityAgent_AUsl()
{
    SCheckedAbilitySlot_AUsl held(0);

    unsigned int typeId = GetFourCC_6F1D6080();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F1D6080(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F246E10 - MakeAbilityAgent_AHpx.  Type-checked slot ctor sub_6F226EE0 @ 0x6F226EE0,
//  Assign sub_6F21A080 @ 0x6F21A080, rawcode getter sub_6F113320 @ 0x6F113320
//  ('AHpx' = 0x41487078).
struct SCheckedAbilitySlot_AHpx
{
    SCheckedAbilitySlot_AHpx(CAgent* candidate);          //  0x6F226EE0
    SCheckedAbilitySlot_AHpx* Assign(CAgent* candidate);  //  0x6F21A080

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_AHpx() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F113320();   //  0x6F113320 - 'AHpx'

CAgent* MakeAbilityAgent_AHpx()
{
    SCheckedAbilitySlot_AHpx held(0);

    unsigned int typeId = GetFourCC_6F113320();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F113320(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F247700 - MakeAbilityAgent_AOvd.  Type-checked slot ctor sub_6F227300 @ 0x6F227300,
//  Assign sub_6F21A600 @ 0x6F21A600, rawcode getter sub_6F1590A0 @ 0x6F1590A0
//  ('AOvd' = 0x414f7664).
struct SCheckedAbilitySlot_AOvd
{
    SCheckedAbilitySlot_AOvd(CAgent* candidate);          //  0x6F227300
    SCheckedAbilitySlot_AOvd* Assign(CAgent* candidate);  //  0x6F21A600

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_AOvd() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F1590A0();   //  0x6F1590A0 - 'AOvd'

CAgent* MakeAbilityAgent_AOvd()
{
    SCheckedAbilitySlot_AOvd held(0);

    unsigned int typeId = GetFourCC_6F1590A0();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F1590A0(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F247D80 - MakeAbilityAgent_ANhs.  Type-checked slot ctor sub_6F227600 @ 0x6F227600,
//  Assign sub_6F21AA00 @ 0x6F21AA00, rawcode getter sub_6F194580 @ 0x6F194580
//  ('ANhs' = 0x414e6873).
struct SCheckedAbilitySlot_ANhs
{
    SCheckedAbilitySlot_ANhs(CAgent* candidate);          //  0x6F227600
    SCheckedAbilitySlot_ANhs* Assign(CAgent* candidate);  //  0x6F21AA00

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_ANhs() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F194580();   //  0x6F194580 - 'ANhs'

CAgent* MakeAbilityAgent_ANhs()
{
    SCheckedAbilitySlot_ANhs held(0);

    unsigned int typeId = GetFourCC_6F194580();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F194580(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F248400 - MakeAbilityAgent_ANic.  Type-checked slot ctor sub_6F2278A0 @ 0x6F2278A0,
//  Assign sub_6F21AD80 @ 0x6F21AD80, rawcode getter sub_6F080480 @ 0x6F080480
//  ('ANic' = 0x414e6963).
struct SCheckedAbilitySlot_ANic
{
    SCheckedAbilitySlot_ANic(CAgent* candidate);          //  0x6F2278A0
    SCheckedAbilitySlot_ANic* Assign(CAgent* candidate);  //  0x6F21AD80

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_ANic() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F080480();   //  0x6F080480 - 'ANic'

CAgent* MakeAbilityAgent_ANic()
{
    SCheckedAbilitySlot_ANic held(0);

    unsigned int typeId = GetFourCC_6F080480();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F080480(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F248A80 - MakeAbilityAgent_AIaa.  Type-checked slot ctor sub_6F227BA0 @ 0x6F227BA0,
//  Assign sub_6F21B180 @ 0x6F21B180, rawcode getter sub_6F0DD1A0 @ 0x6F0DD1A0
//  ('AIaa' = 0x41496161).
struct SCheckedAbilitySlot_AIaa
{
    SCheckedAbilitySlot_AIaa(CAgent* candidate);          //  0x6F227BA0
    SCheckedAbilitySlot_AIaa* Assign(CAgent* candidate);  //  0x6F21B180

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_AIaa() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F0DD1A0();   //  0x6F0DD1A0 - 'AIaa'

CAgent* MakeAbilityAgent_AIaa()
{
    SCheckedAbilitySlot_AIaa held(0);

    unsigned int typeId = GetFourCC_6F0DD1A0();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F0DD1A0(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F249100 - MakeAbilityAgent_AIfl.  Type-checked slot ctor sub_6F227EA0 @ 0x6F227EA0,
//  Assign sub_6F21B580 @ 0x6F21B580, rawcode getter sub_6F0DD620 @ 0x6F0DD620
//  ('AIfl' = 0x4149666c).
struct SCheckedAbilitySlot_AIfl
{
    SCheckedAbilitySlot_AIfl(CAgent* candidate);          //  0x6F227EA0
    SCheckedAbilitySlot_AIfl* Assign(CAgent* candidate);  //  0x6F21B580

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_AIfl() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F0DD620();   //  0x6F0DD620 - 'AIfl'

CAgent* MakeAbilityAgent_AIfl()
{
    SCheckedAbilitySlot_AIfl held(0);

    unsigned int typeId = GetFourCC_6F0DD620();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F0DD620(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F249780 - MakeAbilityAgent_AIfr.  Type-checked slot ctor sub_6F2281A0 @ 0x6F2281A0,
//  Assign sub_6F21B980 @ 0x6F21B980, rawcode getter sub_6F080050 @ 0x6F080050
//  ('AIfr' = 0x41496672).
struct SCheckedAbilitySlot_AIfr
{
    SCheckedAbilitySlot_AIfr(CAgent* candidate);          //  0x6F2281A0
    SCheckedAbilitySlot_AIfr* Assign(CAgent* candidate);  //  0x6F21B980

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_AIfr() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F080050();   //  0x6F080050 - 'AIfr'

CAgent* MakeAbilityAgent_AIfr()
{
    SCheckedAbilitySlot_AIfr held(0);

    unsigned int typeId = GetFourCC_6F080050();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F080050(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F249E00 - MakeAbilityAgent_AIlm.  Type-checked slot ctor sub_6F2284A0 @ 0x6F2284A0,
//  Assign sub_6F21BD80 @ 0x6F21BD80, rawcode getter sub_6F0DD120 @ 0x6F0DD120
//  ('AIlm' = 0x41496c6d).
struct SCheckedAbilitySlot_AIlm
{
    SCheckedAbilitySlot_AIlm(CAgent* candidate);          //  0x6F2284A0
    SCheckedAbilitySlot_AIlm* Assign(CAgent* candidate);  //  0x6F21BD80

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_AIlm() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F0DD120();   //  0x6F0DD120 - 'AIlm'

CAgent* MakeAbilityAgent_AIlm()
{
    SCheckedAbilitySlot_AIlm held(0);

    unsigned int typeId = GetFourCC_6F0DD120();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F0DD120(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F24A480 - MakeAbilityAgent_AIpb.  Type-checked slot ctor sub_6F2287A0 @ 0x6F2287A0,
//  Assign sub_6F21C180 @ 0x6F21C180, rawcode getter sub_6F0DBE80 @ 0x6F0DBE80
//  ('AIpb' = 0x41497062).
struct SCheckedAbilitySlot_AIpb
{
    SCheckedAbilitySlot_AIpb(CAgent* candidate);          //  0x6F2287A0
    SCheckedAbilitySlot_AIpb* Assign(CAgent* candidate);  //  0x6F21C180

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_AIpb() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F0DBE80();   //  0x6F0DBE80 - 'AIpb'

CAgent* MakeAbilityAgent_AIpb()
{
    SCheckedAbilitySlot_AIpb held(0);

    unsigned int typeId = GetFourCC_6F0DBE80();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F0DBE80(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F24ABD0 - MakeAbilityAgent_AIdm.  Type-checked slot ctor sub_6F228AA0 @ 0x6F228AA0,
//  Assign sub_6F21C580 @ 0x6F21C580, rawcode getter sub_6F07BC00 @ 0x6F07BC00
//  ('AIdm' = 0x4149646d).
struct SCheckedAbilitySlot_AIdm
{
    SCheckedAbilitySlot_AIdm(CAgent* candidate);          //  0x6F228AA0
    SCheckedAbilitySlot_AIdm* Assign(CAgent* candidate);  //  0x6F21C580

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_AIdm() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F07BC00();   //  0x6F07BC00 - 'AIdm'

CAgent* MakeAbilityAgent_AIdm()
{
    SCheckedAbilitySlot_AIdm held(0);

    unsigned int typeId = GetFourCC_6F07BC00();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F07BC00(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F24B250 - MakeAbilityAgent_AImr.  Type-checked slot ctor sub_6F228DA0 @ 0x6F228DA0,
//  Assign sub_6F21C980 @ 0x6F21C980, rawcode getter sub_6F111590 @ 0x6F111590
//  ('AImr' = 0x41496d72).
struct SCheckedAbilitySlot_AImr
{
    SCheckedAbilitySlot_AImr(CAgent* candidate);          //  0x6F228DA0
    SCheckedAbilitySlot_AImr* Assign(CAgent* candidate);  //  0x6F21C980

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_AImr() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F111590();   //  0x6F111590 - 'AImr'

CAgent* MakeAbilityAgent_AImr()
{
    SCheckedAbilitySlot_AImr held(0);

    unsigned int typeId = GetFourCC_6F111590();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F111590(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F24B8D0 - MakeAbilityAgent_AIrs.  Type-checked slot ctor sub_6F2290A0 @ 0x6F2290A0,
//  Assign sub_6F21CD80 @ 0x6F21CD80, rawcode getter sub_6F1D37C0 @ 0x6F1D37C0
//  ('AIrs' = 0x41497273).
struct SCheckedAbilitySlot_AIrs
{
    SCheckedAbilitySlot_AIrs(CAgent* candidate);          //  0x6F2290A0
    SCheckedAbilitySlot_AIrs* Assign(CAgent* candidate);  //  0x6F21CD80

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_AIrs() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F1D37C0();   //  0x6F1D37C0 - 'AIrs'

CAgent* MakeAbilityAgent_AIrs()
{
    SCheckedAbilitySlot_AIrs held(0);

    unsigned int typeId = GetFourCC_6F1D37C0();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F1D37C0(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F24BF50 - MakeAbilityAgent_AIso.  Type-checked slot ctor sub_6F2293A0 @ 0x6F2293A0,
//  Assign sub_6F21D180 @ 0x6F21D180, rawcode getter sub_6F1D64D0 @ 0x6F1D64D0
//  ('AIso' = 0x4149736f).
struct SCheckedAbilitySlot_AIso
{
    SCheckedAbilitySlot_AIso(CAgent* candidate);          //  0x6F2293A0
    SCheckedAbilitySlot_AIso* Assign(CAgent* candidate);  //  0x6F21D180

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_AIso() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F1D64D0();   //  0x6F1D64D0 - 'AIso'

CAgent* MakeAbilityAgent_AIso()
{
    SCheckedAbilitySlot_AIso held(0);

    unsigned int typeId = GetFourCC_6F1D64D0();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F1D64D0(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F24C5D0 - MakeAbilityAgent_AIdc.  Type-checked slot ctor sub_6F229640 @ 0x6F229640,
//  Assign sub_6F21D500 @ 0x6F21D500, rawcode getter sub_6F0D9DE0 @ 0x6F0D9DE0
//  ('AIdc' = 0x41496463).
struct SCheckedAbilitySlot_AIdc
{
    SCheckedAbilitySlot_AIdc(CAgent* candidate);          //  0x6F229640
    SCheckedAbilitySlot_AIdc* Assign(CAgent* candidate);  //  0x6F21D500

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_AIdc() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F0D9DE0();   //  0x6F0D9DE0 - 'AIdc'

CAgent* MakeAbilityAgent_AIdc()
{
    SCheckedAbilitySlot_AIdc held(0);

    unsigned int typeId = GetFourCC_6F0D9DE0();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F0D9DE0(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F24CD20 - MakeAbilityAgent_AItb.  Type-checked slot ctor sub_6F2299A0 @ 0x6F2299A0,
//  Assign sub_6F21D980 @ 0x6F21D980, rawcode getter sub_6F024D70 @ 0x6F024D70
//  ('AItb' = 0x41497462).
struct SCheckedAbilitySlot_AItb
{
    SCheckedAbilitySlot_AItb(CAgent* candidate);          //  0x6F2299A0
    SCheckedAbilitySlot_AItb* Assign(CAgent* candidate);  //  0x6F21D980

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_AItb() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F024D70();   //  0x6F024D70 - 'AItb'

CAgent* MakeAbilityAgent_AItb()
{
    SCheckedAbilitySlot_AItb held(0);

    unsigned int typeId = GetFourCC_6F024D70();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F024D70(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F29AD40 - MakeAbilityAgent_AGbu.  Type-checked slot ctor sub_6F280EA0 @ 0x6F280EA0,
//  Assign sub_6F2756B0 @ 0x6F2756B0, rawcode getter sub_6F0D9220 @ 0x6F0D9220
//  ('AGbu' = 0x41476275).
struct SCheckedAbilitySlot_AGbu
{
    SCheckedAbilitySlot_AGbu(CAgent* candidate);          //  0x6F280EA0
    SCheckedAbilitySlot_AGbu* Assign(CAgent* candidate);  //  0x6F2756B0

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_AGbu() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F0D9220();   //  0x6F0D9220 - 'AGbu'

CAgent* MakeAbilityAgent_AGbu()
{
    SCheckedAbilitySlot_AGbu held(0);

    unsigned int typeId = GetFourCC_6F0D9220();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F0D9220(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F29B3C0 - MakeAbilityAgent_Aatk.  Type-checked slot ctor sub_6F092A00 @ 0x6F092A00,
//  Assign sub_6F084EF0 @ 0x6F084EF0, rawcode getter sub_6F0C4D10 @ 0x6F0C4D10
//  ('Aatk' = 0x4161746b).
struct SCheckedAbilitySlot_Aatk
{
    SCheckedAbilitySlot_Aatk(CAgent* candidate);          //  0x6F092A00
    SCheckedAbilitySlot_Aatk* Assign(CAgent* candidate);  //  0x6F084EF0

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_Aatk() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F0C4D10();   //  0x6F0C4D10 - 'Aatk'

CAgent* MakeAbilityAgent_Aatk()
{
    SCheckedAbilitySlot_Aatk held(0);

    unsigned int typeId = GetFourCC_6F0C4D10();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F0C4D10(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F238F70 - MakeAbilityAgent_Absk.  Type-checked slot ctor sub_6F221120 @ 0x6F221120,
//  Assign sub_6F212380 @ 0x6F212380, rawcode getter sub_6F196F60 @ 0x6F196F60
//  ('Absk' = 0x4162736b).
struct SCheckedAbilitySlot_Absk
{
    SCheckedAbilitySlot_Absk(CAgent* candidate);          //  0x6F221120
    SCheckedAbilitySlot_Absk* Assign(CAgent* candidate);  //  0x6F212380

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_Absk() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F196F60();   //  0x6F196F60 - 'Absk'

CAgent* MakeAbilityAgent_Absk()
{
    SCheckedAbilitySlot_Absk held(0);

    unsigned int typeId = GetFourCC_6F196F60();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F196F60(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F23ACB0 - MakeAbilityAgent_Adec.  Type-checked slot ctor sub_6F092820 @ 0x6F092820,
//  Assign sub_6F084C70 @ 0x6F084C70, rawcode getter sub_6F19A330 @ 0x6F19A330
//  ('Adec' = 0x41646563).
struct SCheckedAbilitySlot_Adec
{
    SCheckedAbilitySlot_Adec(CAgent* candidate);          //  0x6F092820
    SCheckedAbilitySlot_Adec* Assign(CAgent* candidate);  //  0x6F084C70

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_Adec() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F19A330();   //  0x6F19A330 - 'Adec'

CAgent* MakeAbilityAgent_Adec()
{
    SCheckedAbilitySlot_Adec held(0);

    unsigned int typeId = GetFourCC_6F19A330();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F19A330(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F23C2A0 - MakeAbilityAgent_Afrb.  Type-checked slot ctor sub_6F222560 @ 0x6F222560,
//  Assign sub_6F213E80 @ 0x6F213E80, rawcode getter sub_6F15B610 @ 0x6F15B610
//  ('Afrb' = 0x41667262).
struct SCheckedAbilitySlot_Afrb
{
    SCheckedAbilitySlot_Afrb(CAgent* candidate);          //  0x6F222560
    SCheckedAbilitySlot_Afrb* Assign(CAgent* candidate);  //  0x6F213E80

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_Afrb() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F15B610();   //  0x6F15B610 - 'Afrb'

CAgent* MakeAbilityAgent_Afrb()
{
    SCheckedAbilitySlot_Afrb held(0);

    unsigned int typeId = GetFourCC_6F15B610();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F15B610(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F23C9F0 - MakeAbilityAgent_Ahar.  Type-checked slot ctor sub_6F222860 @ 0x6F222860,
//  Assign sub_6F214280 @ 0x6F214280, rawcode getter sub_6F025000 @ 0x6F025000
//  ('Ahar' = 0x41686172).
struct SCheckedAbilitySlot_Ahar
{
    SCheckedAbilitySlot_Ahar(CAgent* candidate);          //  0x6F222860
    SCheckedAbilitySlot_Ahar* Assign(CAgent* candidate);  //  0x6F214280

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_Ahar() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F025000();   //  0x6F025000 - 'Ahar'

CAgent* MakeAbilityAgent_Ahar()
{
    SCheckedAbilitySlot_Ahar held(0);

    unsigned int typeId = GetFourCC_6F025000();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F025000(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F23D070 - MakeAbilityAgent_Aivs.  Type-checked slot ctor sub_6F222B00 @ 0x6F222B00,
//  Assign sub_6F214600 @ 0x6F214600, rawcode getter sub_6F10F600 @ 0x6F10F600
//  ('Aivs' = 0x41697673).
struct SCheckedAbilitySlot_Aivs
{
    SCheckedAbilitySlot_Aivs(CAgent* candidate);          //  0x6F222B00
    SCheckedAbilitySlot_Aivs* Assign(CAgent* candidate);  //  0x6F214600

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_Aivs() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F10F600();   //  0x6F10F600 - 'Aivs'

CAgent* MakeAbilityAgent_Aivs()
{
    SCheckedAbilitySlot_Aivs held(0);

    unsigned int typeId = GetFourCC_6F10F600();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F10F600(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F23D6F0 - MakeAbilityAgent_Amls.  Type-checked slot ctor sub_6F222D40 @ 0x6F222D40,
//  Assign sub_6F214900 @ 0x6F214900, rawcode getter sub_6F1108B0 @ 0x6F1108B0
//  ('Amls' = 0x416d6c73).
struct SCheckedAbilitySlot_Amls
{
    SCheckedAbilitySlot_Amls(CAgent* candidate);          //  0x6F222D40
    SCheckedAbilitySlot_Amls* Assign(CAgent* candidate);  //  0x6F214900

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_Amls() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F1108B0();   //  0x6F1108B0 - 'Amls'

CAgent* MakeAbilityAgent_Amls()
{
    SCheckedAbilitySlot_Amls held(0);

    unsigned int typeId = GetFourCC_6F1108B0();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F1108B0(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F23DE40 - MakeAbilityAgent_Amic.  Type-checked slot ctor sub_6F222FE0 @ 0x6F222FE0,
//  Assign sub_6F214C80 @ 0x6F214C80, rawcode getter sub_6F111F60 @ 0x6F111F60
//  ('Amic' = 0x416d6963).
struct SCheckedAbilitySlot_Amic
{
    SCheckedAbilitySlot_Amic(CAgent* candidate);          //  0x6F222FE0
    SCheckedAbilitySlot_Amic* Assign(CAgent* candidate);  //  0x6F214C80

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_Amic() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F111F60();   //  0x6F111F60 - 'Amic'

CAgent* MakeAbilityAgent_Amic()
{
    SCheckedAbilitySlot_Amic held(0);

    unsigned int typeId = GetFourCC_6F111F60();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F111F60(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F23EA70 - MakeAbilityAgent_Apsh.  Type-checked slot ctor sub_6F092880 @ 0x6F092880,
//  Assign sub_6F084CF0 @ 0x6F084CF0, rawcode getter sub_6F112F00 @ 0x6F112F00
//  ('Apsh' = 0x41707368).
struct SCheckedAbilitySlot_Apsh
{
    SCheckedAbilitySlot_Apsh(CAgent* candidate);          //  0x6F092880
    SCheckedAbilitySlot_Apsh* Assign(CAgent* candidate);  //  0x6F084CF0

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_Apsh() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F112F00();   //  0x6F112F00 - 'Apsh'

CAgent* MakeAbilityAgent_Apsh()
{
    SCheckedAbilitySlot_Apsh held(0);

    unsigned int typeId = GetFourCC_6F112F00();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F112F00(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F23F0F0 - MakeAbilityAgent_ANpr.  Type-checked slot ctor sub_6F2237C0 @ 0x6F2237C0,
//  Assign sub_6F215700 @ 0x6F215700, rawcode getter sub_6F113EF0 @ 0x6F113EF0
//  ('ANpr' = 0x414e7072).
struct SCheckedAbilitySlot_ANpr
{
    SCheckedAbilitySlot_ANpr(CAgent* candidate);          //  0x6F2237C0
    SCheckedAbilitySlot_ANpr* Assign(CAgent* candidate);  //  0x6F215700

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_ANpr() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F113EF0();   //  0x6F113EF0 - 'ANpr'

CAgent* MakeAbilityAgent_ANpr()
{
    SCheckedAbilitySlot_ANpr held(0);

    unsigned int typeId = GetFourCC_6F113EF0();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F113EF0(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F23F770 - MakeAbilityAgent_Arem.  Type-checked slot ctor sub_6F223AC0 @ 0x6F223AC0,
//  Assign sub_6F215B00 @ 0x6F215B00, rawcode getter sub_6F0DCFA0 @ 0x6F0DCFA0
//  ('Arem' = 0x4172656d).
struct SCheckedAbilitySlot_Arem
{
    SCheckedAbilitySlot_Arem(CAgent* candidate);          //  0x6F223AC0
    SCheckedAbilitySlot_Arem* Assign(CAgent* candidate);  //  0x6F215B00

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_Arem() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F0DCFA0();   //  0x6F0DCFA0 - 'Arem'

CAgent* MakeAbilityAgent_Arem()
{
    SCheckedAbilitySlot_Arem held(0);

    unsigned int typeId = GetFourCC_6F0DCFA0();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F0DCFA0(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F23FDF0 - MakeAbilityAgent_Aret.  Type-checked slot ctor sub_6F223D60 @ 0x6F223D60,
//  Assign sub_6F215E80 @ 0x6F215E80, rawcode getter sub_6F0DDBA0 @ 0x6F0DDBA0
//  ('Aret' = 0x41726574).
struct SCheckedAbilitySlot_Aret
{
    SCheckedAbilitySlot_Aret(CAgent* candidate);          //  0x6F223D60
    SCheckedAbilitySlot_Aret* Assign(CAgent* candidate);  //  0x6F215E80

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_Aret() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F0DDBA0();   //  0x6F0DDBA0 - 'Aret'

CAgent* MakeAbilityAgent_Aret()
{
    SCheckedAbilitySlot_Aret held(0);

    unsigned int typeId = GetFourCC_6F0DDBA0();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F0DDBA0(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F240470 - MakeAbilityAgent_Asal.  Type-checked slot ctor sub_6F224000 @ 0x6F224000,
//  Assign sub_6F216200 @ 0x6F216200, rawcode getter sub_6F0DD5A0 @ 0x6F0DD5A0
//  ('Asal' = 0x4173616c).
struct SCheckedAbilitySlot_Asal
{
    SCheckedAbilitySlot_Asal(CAgent* candidate);          //  0x6F224000
    SCheckedAbilitySlot_Asal* Assign(CAgent* candidate);  //  0x6F216200

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_Asal() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F0DD5A0();   //  0x6F0DD5A0 - 'Asal'

CAgent* MakeAbilityAgent_Asal()
{
    SCheckedAbilitySlot_Asal held(0);

    unsigned int typeId = GetFourCC_6F0DD5A0();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F0DD5A0(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F240E30 - MakeAbilityAgent_Asps.  Type-checked slot ctor sub_6F224480 @ 0x6F224480,
//  Assign sub_6F216800 @ 0x6F216800, rawcode getter sub_6F1D6C60 @ 0x6F1D6C60
//  ('Asps' = 0x41737073).
struct SCheckedAbilitySlot_Asps
{
    SCheckedAbilitySlot_Asps(CAgent* candidate);          //  0x6F224480
    SCheckedAbilitySlot_Asps* Assign(CAgent* candidate);  //  0x6F216800

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_Asps() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F1D6C60();   //  0x6F1D6C60 - 'Asps'

CAgent* MakeAbilityAgent_Asps()
{
    SCheckedAbilitySlot_Asps held(0);

    unsigned int typeId = GetFourCC_6F1D6C60();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F1D6C60(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F241580 - MakeAbilityAgent_Astn.  Type-checked slot ctor sub_6F224780 @ 0x6F224780,
//  Assign sub_6F216C00 @ 0x6F216C00, rawcode getter sub_6F1D2E00 @ 0x6F1D2E00
//  ('Astn' = 0x4173746e).
struct SCheckedAbilitySlot_Astn
{
    SCheckedAbilitySlot_Astn(CAgent* candidate);          //  0x6F224780
    SCheckedAbilitySlot_Astn* Assign(CAgent* candidate);  //  0x6F216C00

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_Astn() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F1D2E00();   //  0x6F1D2E00 - 'Astn'

CAgent* MakeAbilityAgent_Astn()
{
    SCheckedAbilitySlot_Astn held(0);

    unsigned int typeId = GetFourCC_6F1D2E00();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F1D2E00(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F241DA0 - MakeAbilityAgent_Atol.  Type-checked slot ctor sub_6F224B40 @ 0x6F224B40,
//  Assign sub_6F217100 @ 0x6F217100, rawcode getter sub_6F028420 @ 0x6F028420
//  ('Atol' = 0x41746f6c).
struct SCheckedAbilitySlot_Atol
{
    SCheckedAbilitySlot_Atol(CAgent* candidate);          //  0x6F224B40
    SCheckedAbilitySlot_Atol* Assign(CAgent* candidate);  //  0x6F217100

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_Atol() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F028420();   //  0x6F028420 - 'Atol'

CAgent* MakeAbilityAgent_Atol()
{
    SCheckedAbilitySlot_Atol held(0);

    unsigned int typeId = GetFourCC_6F028420();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F028420(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F242420 - MakeAbilityAgent_Awrp.  Type-checked slot ctor sub_6F224DE0 @ 0x6F224DE0,
//  Assign sub_6F217480 @ 0x6F217480, rawcode getter sub_6F0260D0 @ 0x6F0260D0
//  ('Awrp' = 0x41777270).
struct SCheckedAbilitySlot_Awrp
{
    SCheckedAbilitySlot_Awrp(CAgent* candidate);          //  0x6F224DE0
    SCheckedAbilitySlot_Awrp* Assign(CAgent* candidate);  //  0x6F217480

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_Awrp() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F0260D0();   //  0x6F0260D0 - 'Awrp'

CAgent* MakeAbilityAgent_Awrp()
{
    SCheckedAbilitySlot_Awrp held(0);

    unsigned int typeId = GetFourCC_6F0260D0();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F0260D0(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F244160 - MakeAbilityAgent_AUdp.  Type-checked slot ctor sub_6F225A40 @ 0x6F225A40,
//  Assign sub_6F218500 @ 0x6F218500, rawcode getter sub_6F07BFC0 @ 0x6F07BFC0
//  ('AUdp' = 0x41556470).
struct SCheckedAbilitySlot_AUdp
{
    SCheckedAbilitySlot_AUdp(CAgent* candidate);          //  0x6F225A40
    SCheckedAbilitySlot_AUdp* Assign(CAgent* candidate);  //  0x6F218500

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_AUdp() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F07BFC0();   //  0x6F07BFC0 - 'AUdp'

CAgent* MakeAbilityAgent_AUdp()
{
    SCheckedAbilitySlot_AUdp held(0);

    unsigned int typeId = GetFourCC_6F07BFC0();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F07BFC0(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F246FB0 - MakeAbilityAgent_ANsw.  Type-checked slot ctor sub_6F226FA0 @ 0x6F226FA0,
//  Assign sub_6F21A180 @ 0x6F21A180, rawcode getter sub_6F159DA0 @ 0x6F159DA0
//  ('ANsw' = 0x414e7377).
struct SCheckedAbilitySlot_ANsw
{
    SCheckedAbilitySlot_ANsw(CAgent* candidate);          //  0x6F226FA0
    SCheckedAbilitySlot_ANsw* Assign(CAgent* candidate);  //  0x6F21A180

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_ANsw() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F159DA0();   //  0x6F159DA0 - 'ANsw'

CAgent* MakeAbilityAgent_ANsw()
{
    SCheckedAbilitySlot_ANsw held(0);

    unsigned int typeId = GetFourCC_6F159DA0();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F159DA0(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F2492A0 - MakeAbilityAgent_AIfo.  Type-checked slot ctor sub_6F227F60 @ 0x6F227F60,
//  Assign sub_6F21B680 @ 0x6F21B680, rawcode getter sub_6F0DD720 @ 0x6F0DD720
//  ('AIfo' = 0x4149666f).
struct SCheckedAbilitySlot_AIfo
{
    SCheckedAbilitySlot_AIfo(CAgent* candidate);          //  0x6F227F60
    SCheckedAbilitySlot_AIfo* Assign(CAgent* candidate);  //  0x6F21B680

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_AIfo() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F0DD720();   //  0x6F0DD720 - 'AIfo'

CAgent* MakeAbilityAgent_AIfo()
{
    SCheckedAbilitySlot_AIfo held(0);

    unsigned int typeId = GetFourCC_6F0DD720();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F0DD720(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F24C0F0 - MakeAbilityAgent_AIrg.  Type-checked slot ctor sub_6F229400 @ 0x6F229400,
//  Assign sub_6F21D200 @ 0x6F21D200, rawcode getter sub_6F0DAD80 @ 0x6F0DAD80
//  ('AIrg' = 0x41497267).
struct SCheckedAbilitySlot_AIrg
{
    SCheckedAbilitySlot_AIrg(CAgent* candidate);          //  0x6F229400
    SCheckedAbilitySlot_AIrg* Assign(CAgent* candidate);  //  0x6F21D200

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_AIrg() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F0DAD80();   //  0x6F0DAD80 - 'AIrg'

CAgent* MakeAbilityAgent_AIrg()
{
    SCheckedAbilitySlot_AIrg held(0);

    unsigned int typeId = GetFourCC_6F0DAD80();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F0DAD80(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F1F9F40 - MakeAbilityAgent_Asla.  Same shape as the rest of this
//  family except InitWidgetAgentQuery's fourcc argument is the already-
//  computed `typeId` local, not a second call to the getter - the getter
//  (0x6F1D6100) is called exactly once here, its result kept in a register
//  across the Hash/Ptr calls and reused, where every other instance in this
//  file calls its getter a second time immediately before Init. Both are
//  legitimate C++ (`InitWidgetAgentQuery(&query, typeId, pool)` vs.
//  `InitWidgetAgentQuery(&query, GetFourCC_X(), pool)`), and the dump
//  settles which one this address actually is - the second call site
//  simply is not there. Type-checked slot ctor sub_6F0928E0 @ 0x6F0928E0,
//  Assign sub_6F084D70 @ 0x6F084D70, rawcode getter sub_6F1D6100 @
//  0x6F1D6100 ('Asla' = 0x41736c61).
struct SCheckedAbilitySlot_Asla
{
    SCheckedAbilitySlot_Asla(CAgent* candidate);          //  0x6F0928E0
    SCheckedAbilitySlot_Asla* Assign(CAgent* candidate);  //  0x6F084D70

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_Asla() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F1D6100();   //  0x6F1D6100 - 'Asla'

CAgent* MakeAbilityAgent_Asla()
{
    SCheckedAbilitySlot_Asla held(0);

    unsigned int typeId = GetFourCC_6F1D6100();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, typeId, pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F06EFF0 - MakeAbilityAgent_Afin.  Back to the two-getter-call shape
//  every member but 0x6F1F9F40 uses.  Type-checked slot ctor sub_6F03F880 @
//  0x6F03F880, Assign sub_6F02D500 @ 0x6F02D500, rawcode getter
//  sub_6F026DE0 @ 0x6F026DE0 ('Afin' = 0x4166696e).
struct SCheckedAbilitySlot_Afin
{
    SCheckedAbilitySlot_Afin(CAgent* candidate);          //  0x6F03F880
    SCheckedAbilitySlot_Afin* Assign(CAgent* candidate);  //  0x6F02D500

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_Afin() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F026DE0();   //  0x6F026DE0 - 'Afin'

CAgent* MakeAbilityAgent_Afin()
{
    SCheckedAbilitySlot_Afin held(0);

    unsigned int typeId = GetFourCC_6F026DE0();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F026DE0(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F2395F0 - MakeAbilityAgent_Acan.  Type-checked slot ctor sub_6F221420 @ 0x6F221420,
//  Assign sub_6F212780 @ 0x6F212780, rawcode getter sub_6F198B50 @ 0x6F198B50
//  ('Acan' = 0x4163616e).
struct SCheckedAbilitySlot_Acan
{
    SCheckedAbilitySlot_Acan(CAgent* candidate);          //  0x6F221420
    SCheckedAbilitySlot_Acan* Assign(CAgent* candidate);  //  0x6F212780

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_Acan() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F198B50();   //  0x6F198B50 - 'Acan'

CAgent* MakeAbilityAgent_Acan()
{
    SCheckedAbilitySlot_Acan held(0);

    unsigned int typeId = GetFourCC_6F198B50();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F198B50(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F23B330 - MakeAbilityAgent_Adch.  Type-checked slot ctor sub_6F221EA0 @ 0x6F221EA0,
//  Assign sub_6F213580 @ 0x6F213580, rawcode getter sub_6F0D9960 @ 0x6F0D9960
//  ('Adch' = 0x41646368).
struct SCheckedAbilitySlot_Adch
{
    SCheckedAbilitySlot_Adch(CAgent* candidate);          //  0x6F221EA0
    SCheckedAbilitySlot_Adch* Assign(CAgent* candidate);  //  0x6F213580

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_Adch() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F0D9960();   //  0x6F0D9960 - 'Adch'

CAgent* MakeAbilityAgent_Adch()
{
    SCheckedAbilitySlot_Adch held(0);

    unsigned int typeId = GetFourCC_6F0D9960();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F0D9960(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F23C370 - MakeAbilityAgent_Afzy.  Type-checked slot ctor sub_6F2225C0 @ 0x6F2225C0,
//  Assign sub_6F213F00 @ 0x6F213F00, rawcode getter sub_6F198080 @ 0x6F198080
//  ('Afzy' = 0x41667a79).
struct SCheckedAbilitySlot_Afzy
{
    SCheckedAbilitySlot_Afzy(CAgent* candidate);          //  0x6F2225C0
    SCheckedAbilitySlot_Afzy* Assign(CAgent* candidate);  //  0x6F213F00

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_Afzy() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F198080();   //  0x6F198080 - 'Afzy'

CAgent* MakeAbilityAgent_Afzy()
{
    SCheckedAbilitySlot_Afzy held(0);

    unsigned int typeId = GetFourCC_6F198080();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F198080(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F23CAC0 - MakeAbilityAgent_Ahrl.  Type-checked slot ctor sub_6F0923A0 @ 0x6F0923A0,
//  Assign sub_6F084670 @ 0x6F084670, rawcode getter sub_6F024F80 @ 0x6F024F80
//  ('Ahrl' = 0x4168726c).
struct SCheckedAbilitySlot_Ahrl
{
    SCheckedAbilitySlot_Ahrl(CAgent* candidate);          //  0x6F0923A0
    SCheckedAbilitySlot_Ahrl* Assign(CAgent* candidate);  //  0x6F084670

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_Ahrl() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F024F80();   //  0x6F024F80 - 'Ahrl'

CAgent* MakeAbilityAgent_Ahrl()
{
    SCheckedAbilitySlot_Ahrl held(0);

    unsigned int typeId = GetFourCC_6F024F80();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F024F80(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F23D140 - MakeAbilityAgent_Amin.  Type-checked slot ctor sub_6F222B60 @ 0x6F222B60,
//  Assign sub_6F214680 @ 0x6F214680, rawcode getter sub_6F10FBF0 @ 0x6F10FBF0
//  ('Amin' = 0x416d696e).
struct SCheckedAbilitySlot_Amin
{
    SCheckedAbilitySlot_Amin(CAgent* candidate);          //  0x6F222B60
    SCheckedAbilitySlot_Amin* Assign(CAgent* candidate);  //  0x6F214680

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_Amin() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F10FBF0();   //  0x6F10FBF0 - 'Amin'

CAgent* MakeAbilityAgent_Amin()
{
    SCheckedAbilitySlot_Amin held(0);

    unsigned int typeId = GetFourCC_6F10FBF0();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F10FBF0(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F23D890 - MakeAbilityAgent_Ambt.  Type-checked slot ctor sub_6F222E00 @ 0x6F222E00,
//  Assign sub_6F214A00 @ 0x6F214A00, rawcode getter sub_6F110BC0 @ 0x6F110BC0
//  ('Ambt' = 0x416d6274).
struct SCheckedAbilitySlot_Ambt
{
    SCheckedAbilitySlot_Ambt(CAgent* candidate);          //  0x6F222E00
    SCheckedAbilitySlot_Ambt* Assign(CAgent* candidate);  //  0x6F214A00

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_Ambt() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F110BC0();   //  0x6F110BC0 - 'Ambt'

CAgent* MakeAbilityAgent_Ambt()
{
    SCheckedAbilitySlot_Ambt held(0);

    unsigned int typeId = GetFourCC_6F110BC0();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F110BC0(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F23DF10 - MakeAbilityAgent_ANmr.  Type-checked slot ctor sub_6F223040 @ 0x6F223040,
//  Assign sub_6F214D00 @ 0x6F214D00, rawcode getter sub_6F112390 @ 0x6F112390
//  ('ANmr' = 0x414e6d72).
struct SCheckedAbilitySlot_ANmr
{
    SCheckedAbilitySlot_ANmr(CAgent* candidate);          //  0x6F223040
    SCheckedAbilitySlot_ANmr* Assign(CAgent* candidate);  //  0x6F214D00

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_ANmr() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F112390();   //  0x6F112390 - 'ANmr'

CAgent* MakeAbilityAgent_ANmr()
{
    SCheckedAbilitySlot_ANmr held(0);

    unsigned int typeId = GetFourCC_6F112390();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F112390(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F23EB40 - MakeAbilityAgent_Aphx.  Type-checked slot ctor sub_6F092280 @ 0x6F092280,
//  Assign sub_6F0844F0 @ 0x6F0844F0, rawcode getter sub_6F1132A0 @ 0x6F1132A0
//  ('Aphx' = 0x41706878).
struct SCheckedAbilitySlot_Aphx
{
    SCheckedAbilitySlot_Aphx(CAgent* candidate);          //  0x6F092280
    SCheckedAbilitySlot_Aphx* Assign(CAgent* candidate);  //  0x6F0844F0

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_Aphx() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F1132A0();   //  0x6F1132A0 - 'Aphx'

CAgent* MakeAbilityAgent_Aphx()
{
    SCheckedAbilitySlot_Aphx held(0);

    unsigned int typeId = GetFourCC_6F1132A0();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F1132A0(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F23F1C0 - MakeAbilityAgent_Apit.  Type-checked slot ctor sub_6F223820 @ 0x6F223820,
//  Assign sub_6F215780 @ 0x6F215780, rawcode getter sub_6F0289C0 @ 0x6F0289C0
//  ('Apit' = 0x41706974).
struct SCheckedAbilitySlot_Apit
{
    SCheckedAbilitySlot_Apit(CAgent* candidate);          //  0x6F223820
    SCheckedAbilitySlot_Apit* Assign(CAgent* candidate);  //  0x6F215780

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_Apit() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F0289C0();   //  0x6F0289C0 - 'Apit'

CAgent* MakeAbilityAgent_Apit()
{
    SCheckedAbilitySlot_Apit held(0);

    unsigned int typeId = GetFourCC_6F0289C0();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F0289C0(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F23F840 - MakeAbilityAgent_Aren.  Type-checked slot ctor sub_6F223B20 @ 0x6F223B20,
//  Assign sub_6F215B80 @ 0x6F215B80, rawcode getter sub_6F027F80 @ 0x6F027F80
//  ('Aren' = 0x4172656e).
struct SCheckedAbilitySlot_Aren
{
    SCheckedAbilitySlot_Aren(CAgent* candidate);          //  0x6F223B20
    SCheckedAbilitySlot_Aren* Assign(CAgent* candidate);  //  0x6F215B80

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_Aren() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F027F80();   //  0x6F027F80 - 'Aren'

CAgent* MakeAbilityAgent_Aren()
{
    SCheckedAbilitySlot_Aren held(0);

    unsigned int typeId = GetFourCC_6F027F80();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F027F80(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F23FEC0 - MakeAbilityAgent_Artn.  Type-checked slot ctor sub_6F223DC0 @ 0x6F223DC0,
//  Assign sub_6F215F00 @ 0x6F215F00, rawcode getter sub_6F1D3880 @ 0x6F1D3880
//  ('Artn' = 0x4172746e).
struct SCheckedAbilitySlot_Artn
{
    SCheckedAbilitySlot_Artn(CAgent* candidate);          //  0x6F223DC0
    SCheckedAbilitySlot_Artn* Assign(CAgent* candidate);  //  0x6F215F00

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_Artn() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F1D3880();   //  0x6F1D3880 - 'Artn'

CAgent* MakeAbilityAgent_Artn()
{
    SCheckedAbilitySlot_Artn held(0);

    unsigned int typeId = GetFourCC_6F1D3880();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F1D3880(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F240610 - MakeAbilityAgent_Asds.  Type-checked slot ctor sub_6F2240C0 @ 0x6F2240C0,
//  Assign sub_6F216300 @ 0x6F216300, rawcode getter sub_6F1D44C0 @ 0x6F1D44C0
//  ('Asds' = 0x41736473).
struct SCheckedAbilitySlot_Asds
{
    SCheckedAbilitySlot_Asds(CAgent* candidate);          //  0x6F2240C0
    SCheckedAbilitySlot_Asds* Assign(CAgent* candidate);  //  0x6F216300

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_Asds() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F1D44C0();   //  0x6F1D44C0 - 'Asds'

CAgent* MakeAbilityAgent_Asds()
{
    SCheckedAbilitySlot_Asds held(0);

    unsigned int typeId = GetFourCC_6F1D44C0();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F1D44C0(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F240F00 - MakeAbilityAgent_Asph.  Type-checked slot ctor sub_6F2244E0 @ 0x6F2244E0,
//  Assign sub_6F216880 @ 0x6F216880, rawcode getter sub_6F1D6EC0 @ 0x6F1D6EC0
//  ('Asph' = 0x41737068).
struct SCheckedAbilitySlot_Asph
{
    SCheckedAbilitySlot_Asph(CAgent* candidate);          //  0x6F2244E0
    SCheckedAbilitySlot_Asph* Assign(CAgent* candidate);  //  0x6F216880

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_Asph() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F1D6EC0();   //  0x6F1D6EC0 - 'Asph'

CAgent* MakeAbilityAgent_Asph()
{
    SCheckedAbilitySlot_Asph held(0);

    unsigned int typeId = GetFourCC_6F1D6EC0();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F1D6EC0(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F241650 - MakeAbilityAgent_Assk.  Type-checked slot ctor sub_6F2247E0 @ 0x6F2247E0,
//  Assign sub_6F216C80 @ 0x6F216C80, rawcode getter sub_6F1D8250 @ 0x6F1D8250
//  ('Assk' = 0x4173736b).
struct SCheckedAbilitySlot_Assk
{
    SCheckedAbilitySlot_Assk(CAgent* candidate);          //  0x6F2247E0
    SCheckedAbilitySlot_Assk* Assign(CAgent* candidate);  //  0x6F216C80

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_Assk() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F1D8250();   //  0x6F1D8250 - 'Assk'

CAgent* MakeAbilityAgent_Assk()
{
    SCheckedAbilitySlot_Assk held(0);

    unsigned int typeId = GetFourCC_6F1D8250();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F1D8250(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F241E70 - MakeAbilityAgent_Atru.  Type-checked slot ctor sub_6F224BA0 @ 0x6F224BA0,
//  Assign sub_6F217180 @ 0x6F217180, rawcode getter sub_6F024990 @ 0x6F024990
//  ('Atru' = 0x41747275).
struct SCheckedAbilitySlot_Atru
{
    SCheckedAbilitySlot_Atru(CAgent* candidate);          //  0x6F224BA0
    SCheckedAbilitySlot_Atru* Assign(CAgent* candidate);  //  0x6F217180

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_Atru() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F024990();   //  0x6F024990 - 'Atru'

CAgent* MakeAbilityAgent_Atru()
{
    SCheckedAbilitySlot_Atru held(0);

    unsigned int typeId = GetFourCC_6F024990();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F024990(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F2424F0 - MakeAbilityAgent_Awar.  Type-checked slot ctor sub_6F224E40 @ 0x6F224E40,
//  Assign sub_6F217500 @ 0x6F217500, rawcode getter sub_6F159820 @ 0x6F159820
//  ('Awar' = 0x41776172).
struct SCheckedAbilitySlot_Awar
{
    SCheckedAbilitySlot_Awar(CAgent* candidate);          //  0x6F224E40
    SCheckedAbilitySlot_Awar* Assign(CAgent* candidate);  //  0x6F217500

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_Awar() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F159820();   //  0x6F159820 - 'Awar'

CAgent* MakeAbilityAgent_Awar()
{
    SCheckedAbilitySlot_Awar held(0);

    unsigned int typeId = GetFourCC_6F159820();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F159820(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F2447E0 - MakeAbilityAgent_ANef.  Type-checked slot ctor sub_6F225D40 @ 0x6F225D40,
//  Assign sub_6F218900 @ 0x6F218900, rawcode getter sub_6F07E6D0 @ 0x6F07E6D0
//  ('ANef' = 0x414e6566).
struct SCheckedAbilitySlot_ANef
{
    SCheckedAbilitySlot_ANef(CAgent* candidate);          //  0x6F225D40
    SCheckedAbilitySlot_ANef* Assign(CAgent* candidate);  //  0x6F218900

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_ANef() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F07E6D0();   //  0x6F07E6D0 - 'ANef'

CAgent* MakeAbilityAgent_ANef()
{
    SCheckedAbilitySlot_ANef held(0);

    unsigned int typeId = GetFourCC_6F07E6D0();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F07E6D0(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F2473C0 - MakeAbilityAgent_Atdg.  Type-checked slot ctor sub_6F227180 @ 0x6F227180,
//  Assign sub_6F21A400 @ 0x6F21A400, rawcode getter sub_6F158140 @ 0x6F158140
//  ('Atdg' = 0x41746467).
struct SCheckedAbilitySlot_Atdg
{
    SCheckedAbilitySlot_Atdg(CAgent* candidate);          //  0x6F227180
    SCheckedAbilitySlot_Atdg* Assign(CAgent* candidate);  //  0x6F21A400

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_Atdg() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F158140();   //  0x6F158140 - 'Atdg'

CAgent* MakeAbilityAgent_Atdg()
{
    SCheckedAbilitySlot_Atdg held(0);

    unsigned int typeId = GetFourCC_6F158140();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F158140(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F249920 - MakeAbilityAgent_AIfh.  Type-checked slot ctor sub_6F228260 @ 0x6F228260,
//  Assign sub_6F21BA80 @ 0x6F21BA80, rawcode getter sub_6F080150 @ 0x6F080150
//  ('AIfh' = 0x41496668).
struct SCheckedAbilitySlot_AIfh
{
    SCheckedAbilitySlot_AIfh(CAgent* candidate);          //  0x6F228260
    SCheckedAbilitySlot_AIfh* Assign(CAgent* candidate);  //  0x6F21BA80

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_AIfh() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F080150();   //  0x6F080150 - 'AIfh'

CAgent* MakeAbilityAgent_AIfh()
{
    SCheckedAbilitySlot_AIfh held(0);

    unsigned int typeId = GetFourCC_6F080150();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F080150(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F24C770 - MakeAbilityAgent_AIfa.  Type-checked slot ctor sub_6F229700 @ 0x6F229700,
//  Assign sub_6F21D600 @ 0x6F21D600, rawcode getter sub_6F081AE0 @ 0x6F081AE0
//  ('AIfa' = 0x41496661).
struct SCheckedAbilitySlot_AIfa
{
    SCheckedAbilitySlot_AIfa(CAgent* candidate);          //  0x6F229700
    SCheckedAbilitySlot_AIfa* Assign(CAgent* candidate);  //  0x6F21D600

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_AIfa() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F081AE0();   //  0x6F081AE0 - 'AIfa'

CAgent* MakeAbilityAgent_AIfa()
{
    SCheckedAbilitySlot_AIfa held(0);

    unsigned int typeId = GetFourCC_6F081AE0();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F081AE0(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}



//----------------------------------------------------------------------------
//  Link-completion thunks - each of these addresses was declared (not
//  defined) elsewhere in this file because comments record it as another
//  parallel agent's own worklist target, but the two agents independently
//  invented different class/struct names for the same shared address, so
//  the declaring extern reference never actually resolves against the
//  sibling's real (differently-named) reconstruction. Defining a plain
//  redirect thunk here - the same `mov eax,<addr>/jmp eax` idiom this
//  repo already uses for any out-of-scope callee - makes this TU link
//  without re-claiming the address (no funcmap.py row points at these).
//----------------------------------------------------------------------------
__declspec(naked) SCheckedAbilitySlot_AEah::SCheckedAbilitySlot_AEah(CAgent* candidate)
{
    __asm { mov eax, 06F2250E0h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_AGbu::SCheckedAbilitySlot_AGbu(CAgent* candidate)
{
    __asm { mov eax, 06F280EA0h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_AHpx::SCheckedAbilitySlot_AHpx(CAgent* candidate)
{
    __asm { mov eax, 06F226EE0h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_AIaa::SCheckedAbilitySlot_AIaa(CAgent* candidate)
{
    __asm { mov eax, 06F227BA0h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_AIdc::SCheckedAbilitySlot_AIdc(CAgent* candidate)
{
    __asm { mov eax, 06F229640h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_AIdd::SCheckedAbilitySlot_AIdd(CAgent* candidate)
{
    __asm { mov eax, 06F223460h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_AIdm::SCheckedAbilitySlot_AIdm(CAgent* candidate)
{
    __asm { mov eax, 06F228AA0h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_AIfl::SCheckedAbilitySlot_AIfl(CAgent* candidate)
{
    __asm { mov eax, 06F227EA0h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_AIfr::SCheckedAbilitySlot_AIfr(CAgent* candidate)
{
    __asm { mov eax, 06F2281A0h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_AIlm::SCheckedAbilitySlot_AIlm(CAgent* candidate)
{
    __asm { mov eax, 06F2284A0h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_AImr::SCheckedAbilitySlot_AImr(CAgent* candidate)
{
    __asm { mov eax, 06F228DA0h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_AIpb::SCheckedAbilitySlot_AIpb(CAgent* candidate)
{
    __asm { mov eax, 06F2287A0h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_AIrs::SCheckedAbilitySlot_AIrs(CAgent* candidate)
{
    __asm { mov eax, 06F2290A0h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_AIso::SCheckedAbilitySlot_AIso(CAgent* candidate)
{
    __asm { mov eax, 06F2293A0h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_AItb::SCheckedAbilitySlot_AItb(CAgent* candidate)
{
    __asm { mov eax, 06F2299A0h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_ANba::SCheckedAbilitySlot_ANba(CAgent* candidate)
{
    __asm { mov eax, 06F2253E0h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_ANch::SCheckedAbilitySlot_ANch(CAgent* candidate)
{
    __asm { mov eax, 06F225680h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_ANdh::SCheckedAbilitySlot_ANdh(CAgent* candidate)
{
    __asm { mov eax, 06F225C80h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_ANfb::SCheckedAbilitySlot_ANfb(CAgent* candidate)
{
    __asm { mov eax, 06F225F80h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_ANhs::SCheckedAbilitySlot_ANhs(CAgent* candidate)
{
    __asm { mov eax, 06F227600h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_ANic::SCheckedAbilitySlot_ANic(CAgent* candidate)
{
    __asm { mov eax, 06F2278A0h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_ANrc::SCheckedAbilitySlot_ANrc(CAgent* candidate)
{
    __asm { mov eax, 06F2268E0h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_ANsu::SCheckedAbilitySlot_ANsu(CAgent* candidate)
{
    __asm { mov eax, 06F2248A0h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_AOhw::SCheckedAbilitySlot_AOhw(CAgent* candidate)
{
    __asm { mov eax, 06F226280h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_AOvd::SCheckedAbilitySlot_AOvd(CAgent* candidate)
{
    __asm { mov eax, 06F227300h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_AUdc::SCheckedAbilitySlot_AUdc(CAgent* candidate)
{
    __asm { mov eax, 06F225980h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_AUls::SCheckedAbilitySlot_AUls(CAgent* candidate)
{
    __asm { mov eax, 06F226580h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_AUsl::SCheckedAbilitySlot_AUsl(CAgent* candidate)
{
    __asm { mov eax, 06F226BE0h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Aabs::SCheckedAbilitySlot_Aabs(CAgent* candidate)
{
    __asm { mov eax, 06F220A00h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Aapl::SCheckedAbilitySlot_Aapl(CAgent* candidate)
{
    __asm { mov eax, 06F220CA0h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Aatk::SCheckedAbilitySlot_Aatk(CAgent* candidate)
{
    __asm { mov eax, 06F092A00h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Abtl::SCheckedAbilitySlot_Abtl(CAgent* candidate)
{
    __asm { mov eax, 06F221060h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Abun::SCheckedAbilitySlot_Abun(CAgent* candidate)
{
    __asm { mov eax, 06F221360h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Acha::SCheckedAbilitySlot_Acha(CAgent* candidate)
{
    __asm { mov eax, 06F2215A0h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Acoi::SCheckedAbilitySlot_Acoi(CAgent* candidate)
{
    __asm { mov eax, 06F092760h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Acyc::SCheckedAbilitySlot_Acyc(CAgent* candidate)
{
    __asm { mov eax, 06F221A80h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Advm::SCheckedAbilitySlot_Advm(CAgent* candidate)
{
    __asm { mov eax, 06F221DE0h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Aegm::SCheckedAbilitySlot_Aegm(CAgent* candidate)
{
    __asm { mov eax, 06F2220E0h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Aeth::SCheckedAbilitySlot_Aeth(CAgent* candidate)
{
    __asm { mov eax, 06F222680h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Absk::SCheckedAbilitySlot_Absk(CAgent* candidate)
{
    __asm { mov eax, 06F221120h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Adec::SCheckedAbilitySlot_Adec(CAgent* candidate)
{
    __asm { mov eax, 06F092820h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Afrb::SCheckedAbilitySlot_Afrb(CAgent* candidate)
{
    __asm { mov eax, 06F222560h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Ahar::SCheckedAbilitySlot_Ahar(CAgent* candidate)
{
    __asm { mov eax, 06F222860h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Aivs::SCheckedAbilitySlot_Aivs(CAgent* candidate)
{
    __asm { mov eax, 06F222B00h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Amls::SCheckedAbilitySlot_Amls(CAgent* candidate)
{
    __asm { mov eax, 06F222D40h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Amic::SCheckedAbilitySlot_Amic(CAgent* candidate)
{
    __asm { mov eax, 06F222FE0h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Apsh::SCheckedAbilitySlot_Apsh(CAgent* candidate)
{
    __asm { mov eax, 06F092880h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_ANpr::SCheckedAbilitySlot_ANpr(CAgent* candidate)
{
    __asm { mov eax, 06F2237C0h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Arem::SCheckedAbilitySlot_Arem(CAgent* candidate)
{
    __asm { mov eax, 06F223AC0h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Aret::SCheckedAbilitySlot_Aret(CAgent* candidate)
{
    __asm { mov eax, 06F223D60h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Asal::SCheckedAbilitySlot_Asal(CAgent* candidate)
{
    __asm { mov eax, 06F224000h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Asps::SCheckedAbilitySlot_Asps(CAgent* candidate)
{
    __asm { mov eax, 06F224480h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Astn::SCheckedAbilitySlot_Astn(CAgent* candidate)
{
    __asm { mov eax, 06F224780h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Atol::SCheckedAbilitySlot_Atol(CAgent* candidate)
{
    __asm { mov eax, 06F224B40h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Awrp::SCheckedAbilitySlot_Awrp(CAgent* candidate)
{
    __asm { mov eax, 06F224DE0h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_AUdp::SCheckedAbilitySlot_AUdp(CAgent* candidate)
{
    __asm { mov eax, 06F225A40h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_ANsw::SCheckedAbilitySlot_ANsw(CAgent* candidate)
{
    __asm { mov eax, 06F226FA0h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_AIfo::SCheckedAbilitySlot_AIfo(CAgent* candidate)
{
    __asm { mov eax, 06F227F60h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_AIrg::SCheckedAbilitySlot_AIrg(CAgent* candidate)
{
    __asm { mov eax, 06F229400h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Asla::SCheckedAbilitySlot_Asla(CAgent* candidate)
{
    __asm { mov eax, 06F0928E0h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Afin::SCheckedAbilitySlot_Afin(CAgent* candidate)
{
    __asm { mov eax, 06F03F880h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_AEah* SCheckedAbilitySlot_AEah::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F217880h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_AHpx* SCheckedAbilitySlot_AHpx::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F21A080h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_AIaa* SCheckedAbilitySlot_AIaa::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F21B180h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_AIdc* SCheckedAbilitySlot_AIdc::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F21D500h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_AIdd* SCheckedAbilitySlot_AIdd::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F215280h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_AIdm* SCheckedAbilitySlot_AIdm::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F21C580h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_AIfl* SCheckedAbilitySlot_AIfl::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F21B580h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_AIfr* SCheckedAbilitySlot_AIfr::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F21B980h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_AIlm* SCheckedAbilitySlot_AIlm::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F21BD80h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_AImr* SCheckedAbilitySlot_AImr::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F21C980h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_AIpb* SCheckedAbilitySlot_AIpb::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F21C180h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_AIrs* SCheckedAbilitySlot_AIrs::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F21CD80h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_AIso* SCheckedAbilitySlot_AIso::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F21D180h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_AItb* SCheckedAbilitySlot_AItb::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F21D980h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_ANba* SCheckedAbilitySlot_ANba::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F217C80h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_ANch* SCheckedAbilitySlot_ANch::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F218000h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_ANdh* SCheckedAbilitySlot_ANdh::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F218800h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_ANfb* SCheckedAbilitySlot_ANfb::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F218C00h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_ANhs* SCheckedAbilitySlot_ANhs::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F21AA00h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_ANic* SCheckedAbilitySlot_ANic::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F21AD80h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_ANrc* SCheckedAbilitySlot_ANrc::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F219880h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_ANsu* SCheckedAbilitySlot_ANsu::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F216D80h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_AOhw* SCheckedAbilitySlot_AOhw::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F219000h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_AOvd* SCheckedAbilitySlot_AOvd::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F21A600h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_AUdc* SCheckedAbilitySlot_AUdc::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F218400h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_AUls* SCheckedAbilitySlot_AUls::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F219400h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_AUsl* SCheckedAbilitySlot_AUsl::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F219C80h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Aabs* SCheckedAbilitySlot_Aabs::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F211A00h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Aapl* SCheckedAbilitySlot_Aapl::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F211D80h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Aatk* SCheckedAbilitySlot_Aatk::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F084EF0h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Abtl* SCheckedAbilitySlot_Abtl::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F212280h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Abun* SCheckedAbilitySlot_Abun::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F212680h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Acha* SCheckedAbilitySlot_Acha::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F212980h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Acoi* SCheckedAbilitySlot_Acoi::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F084B70h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Acyc* SCheckedAbilitySlot_Acyc::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F213000h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Advm* SCheckedAbilitySlot_Advm::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F213480h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Aegm* SCheckedAbilitySlot_Aegm::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F213880h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Aeth* SCheckedAbilitySlot_Aeth::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F214000h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Absk* SCheckedAbilitySlot_Absk::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F212380h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Adec* SCheckedAbilitySlot_Adec::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F084C70h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Afrb* SCheckedAbilitySlot_Afrb::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F213E80h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Ahar* SCheckedAbilitySlot_Ahar::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F214280h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Aivs* SCheckedAbilitySlot_Aivs::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F214600h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Amls* SCheckedAbilitySlot_Amls::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F214900h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Amic* SCheckedAbilitySlot_Amic::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F214C80h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Apsh* SCheckedAbilitySlot_Apsh::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F084CF0h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_ANpr* SCheckedAbilitySlot_ANpr::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F215700h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Arem* SCheckedAbilitySlot_Arem::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F215B00h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Aret* SCheckedAbilitySlot_Aret::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F215E80h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Asal* SCheckedAbilitySlot_Asal::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F216200h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Asps* SCheckedAbilitySlot_Asps::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F216800h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Astn* SCheckedAbilitySlot_Astn::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F216C00h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Atol* SCheckedAbilitySlot_Atol::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F217100h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Awrp* SCheckedAbilitySlot_Awrp::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F217480h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_AUdp* SCheckedAbilitySlot_AUdp::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F218500h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_ANsw* SCheckedAbilitySlot_ANsw::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F21A180h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_AIfo* SCheckedAbilitySlot_AIfo::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F21B680h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_AIrg* SCheckedAbilitySlot_AIrg::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F21D200h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Asla* SCheckedAbilitySlot_Asla::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F084D70h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Afin* SCheckedAbilitySlot_Afin::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F02D500h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F024D70()
{
    __asm { mov eax, 06F024D70h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F07BC00()
{
    __asm { mov eax, 06F07BC00h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F07C0E0()
{
    __asm { mov eax, 06F07C0E0h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F07C840()
{
    __asm { mov eax, 06F07C840h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F07CFA0()
{
    __asm { mov eax, 06F07CFA0h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F07DED0()
{
    __asm { mov eax, 06F07DED0h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F080050()
{
    __asm { mov eax, 06F080050h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F080480()
{
    __asm { mov eax, 06F080480h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F082970()
{
    __asm { mov eax, 06F082970h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F082BF0()
{
    __asm { mov eax, 06F082BF0h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F0C4D10()
{
    __asm { mov eax, 06F0C4D10h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F0D9DE0()
{
    __asm { mov eax, 06F0D9DE0h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F0DA110()
{
    __asm { mov eax, 06F0DA110h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F0DBE80()
{
    __asm { mov eax, 06F0DBE80h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F0DD120()
{
    __asm { mov eax, 06F0DD120h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F0DD1A0()
{
    __asm { mov eax, 06F0DD1A0h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F0DD620()
{
    __asm { mov eax, 06F0DD620h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F10F780()
{
    __asm { mov eax, 06F10F780h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F110240()
{
    __asm { mov eax, 06F110240h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F111590()
{
    __asm { mov eax, 06F111590h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F113320()
{
    __asm { mov eax, 06F113320h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F156D40()
{
    __asm { mov eax, 06F156D40h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F1590A0()
{
    __asm { mov eax, 06F1590A0h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F194580()
{
    __asm { mov eax, 06F194580h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F1954E0()
{
    __asm { mov eax, 06F1954E0h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F1962A0()
{
    __asm { mov eax, 06F1962A0h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F1964C0()
{
    __asm { mov eax, 06F1964C0h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F1971E0()
{
    __asm { mov eax, 06F1971E0h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F198620()
{
    __asm { mov eax, 06F198620h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F1986A0()
{
    __asm { mov eax, 06F1986A0h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F198940()
{
    __asm { mov eax, 06F198940h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F198FC0()
{
    __asm { mov eax, 06F198FC0h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F199440()
{
    __asm { mov eax, 06F199440h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F19A2B0()
{
    __asm { mov eax, 06F19A2B0h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F19ACA0()
{
    __asm { mov eax, 06F19ACA0h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F1D37C0()
{
    __asm { mov eax, 06F1D37C0h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F1D6080()
{
    __asm { mov eax, 06F1D6080h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F1D64D0()
{
    __asm { mov eax, 06F1D64D0h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F196F60()
{
    __asm { mov eax, 06F196F60h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F19A330()
{
    __asm { mov eax, 06F19A330h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F15B610()
{
    __asm { mov eax, 06F15B610h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F025000()
{
    __asm { mov eax, 06F025000h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F10F600()
{
    __asm { mov eax, 06F10F600h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F1108B0()
{
    __asm { mov eax, 06F1108B0h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F111F60()
{
    __asm { mov eax, 06F111F60h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F112F00()
{
    __asm { mov eax, 06F112F00h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F113EF0()
{
    __asm { mov eax, 06F113EF0h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F0DCFA0()
{
    __asm { mov eax, 06F0DCFA0h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F0DDBA0()
{
    __asm { mov eax, 06F0DDBA0h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F0DD5A0()
{
    __asm { mov eax, 06F0DD5A0h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F1D6C60()
{
    __asm { mov eax, 06F1D6C60h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F1D2E00()
{
    __asm { mov eax, 06F1D2E00h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F028420()
{
    __asm { mov eax, 06F028420h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F07BFC0()
{
    __asm { mov eax, 06F07BFC0h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F159DA0()
{
    __asm { mov eax, 06F159DA0h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F0DD720()
{
    __asm { mov eax, 06F0DD720h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F0DAD80()
{
    __asm { mov eax, 06F0DAD80h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F1D6100()
{
    __asm { mov eax, 06F1D6100h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F026DE0()
{
    __asm { mov eax, 06F026DE0h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Acan::SCheckedAbilitySlot_Acan(CAgent* candidate)
{
    __asm { mov eax, 06F221420h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Adch::SCheckedAbilitySlot_Adch(CAgent* candidate)
{
    __asm { mov eax, 06F221EA0h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Afzy::SCheckedAbilitySlot_Afzy(CAgent* candidate)
{
    __asm { mov eax, 06F2225C0h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Ahrl::SCheckedAbilitySlot_Ahrl(CAgent* candidate)
{
    __asm { mov eax, 06F0923A0h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Amin::SCheckedAbilitySlot_Amin(CAgent* candidate)
{
    __asm { mov eax, 06F222B60h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Ambt::SCheckedAbilitySlot_Ambt(CAgent* candidate)
{
    __asm { mov eax, 06F222E00h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_ANmr::SCheckedAbilitySlot_ANmr(CAgent* candidate)
{
    __asm { mov eax, 06F223040h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Aphx::SCheckedAbilitySlot_Aphx(CAgent* candidate)
{
    __asm { mov eax, 06F092280h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Apit::SCheckedAbilitySlot_Apit(CAgent* candidate)
{
    __asm { mov eax, 06F223820h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Aren::SCheckedAbilitySlot_Aren(CAgent* candidate)
{
    __asm { mov eax, 06F223B20h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Artn::SCheckedAbilitySlot_Artn(CAgent* candidate)
{
    __asm { mov eax, 06F223DC0h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Asds::SCheckedAbilitySlot_Asds(CAgent* candidate)
{
    __asm { mov eax, 06F2240C0h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Asph::SCheckedAbilitySlot_Asph(CAgent* candidate)
{
    __asm { mov eax, 06F2244E0h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Assk::SCheckedAbilitySlot_Assk(CAgent* candidate)
{
    __asm { mov eax, 06F2247E0h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Atru::SCheckedAbilitySlot_Atru(CAgent* candidate)
{
    __asm { mov eax, 06F224BA0h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Awar::SCheckedAbilitySlot_Awar(CAgent* candidate)
{
    __asm { mov eax, 06F224E40h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_ANef::SCheckedAbilitySlot_ANef(CAgent* candidate)
{
    __asm { mov eax, 06F225D40h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Atdg::SCheckedAbilitySlot_Atdg(CAgent* candidate)
{
    __asm { mov eax, 06F227180h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_AIfh::SCheckedAbilitySlot_AIfh(CAgent* candidate)
{
    __asm { mov eax, 06F228260h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_AIfa::SCheckedAbilitySlot_AIfa(CAgent* candidate)
{
    __asm { mov eax, 06F229700h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Acan* SCheckedAbilitySlot_Acan::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F212780h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Adch* SCheckedAbilitySlot_Adch::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F213580h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Afzy* SCheckedAbilitySlot_Afzy::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F213F00h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Ahrl* SCheckedAbilitySlot_Ahrl::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F084670h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Amin* SCheckedAbilitySlot_Amin::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F214680h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Ambt* SCheckedAbilitySlot_Ambt::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F214A00h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_ANmr* SCheckedAbilitySlot_ANmr::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F214D00h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Aphx* SCheckedAbilitySlot_Aphx::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F0844F0h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Apit* SCheckedAbilitySlot_Apit::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F215780h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Aren* SCheckedAbilitySlot_Aren::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F215B80h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Artn* SCheckedAbilitySlot_Artn::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F215F00h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Asds* SCheckedAbilitySlot_Asds::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F216300h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Asph* SCheckedAbilitySlot_Asph::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F216880h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Assk* SCheckedAbilitySlot_Assk::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F216C80h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Atru* SCheckedAbilitySlot_Atru::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F217180h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Awar* SCheckedAbilitySlot_Awar::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F217500h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_ANef* SCheckedAbilitySlot_ANef::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F218900h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Atdg* SCheckedAbilitySlot_Atdg::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F21A400h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_AIfh* SCheckedAbilitySlot_AIfh::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F21BA80h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_AIfa* SCheckedAbilitySlot_AIfa::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F21D600h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F198B50()
{
    __asm { mov eax, 06F198B50h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F0D9960()
{
    __asm { mov eax, 06F0D9960h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F198080()
{
    __asm { mov eax, 06F198080h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F024F80()
{
    __asm { mov eax, 06F024F80h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F10FBF0()
{
    __asm { mov eax, 06F10FBF0h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F112390()
{
    __asm { mov eax, 06F112390h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F1132A0()
{
    __asm { mov eax, 06F1132A0h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F0289C0()
{
    __asm { mov eax, 06F0289C0h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F027F80()
{
    __asm { mov eax, 06F027F80h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F1D3880()
{
    __asm { mov eax, 06F1D3880h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F1D44C0()
{
    __asm { mov eax, 06F1D44C0h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F1D6EC0()
{
    __asm { mov eax, 06F1D6EC0h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F1D8250()
{
    __asm { mov eax, 06F1D8250h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F024990()
{
    __asm { mov eax, 06F024990h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F159820()
{
    __asm { mov eax, 06F159820h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F07E6D0()
{
    __asm { mov eax, 06F07E6D0h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F158140()
{
    __asm { mov eax, 06F158140h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F080150()
{
    __asm { mov eax, 06F080150h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int GetFourCC_6F081AE0()
{
    __asm { mov eax, 06F081AE0h }
    __asm { jmp eax }
}


//============================================================================
//  A further slice of the same family, reached from this batch's own
//  JASS_UnitAddAbility closure walk (jassvm-uaa-C) rather than the CUnit
//  vtable BFS the rest of this file came from - same shape, same shared
//  calls, landed here because this is where the count of
//  `MakeAbilityAgent_` definitions already lives. All 19 addresses below
//  are this batch's own worklist entries; their type-checked slot's own
//  ctor/Assign and each rawcode getter are not - each is defined as a
//  naked self-redirect thunk at the bottom of this block, the same
//  convention cunit_agent_seh_abilityfactories.cpp uses for its own
//  out-of-slice triples, so this TU links without reserving a funcmap.py
//  row for an address nobody here has actually reconstructed.
//============================================================================

//  0x6F23FC50 - MakeAbilityAgent_Arsk.  Type-checked slot ctor 0x6F223CA0,
//  Assign 0x6F215D80, rawcode getter 0x6F1D82D0 ('Arsk' = 0x4172736B).
struct SCheckedAbilitySlot_Arsk
{
    SCheckedAbilitySlot_Arsk(CAgent* candidate);          //  0x6F223CA0
    SCheckedAbilitySlot_Arsk* Assign(CAgent* candidate);  //  0x6F215D80

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_Arsk() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F1D82D0();   //  0x6F1D82D0 - 'Arsk'

CAgent* MakeAbilityAgent_Arsk()
{
    SCheckedAbilitySlot_Arsk held(0);

    unsigned int typeId = GetFourCC_6F1D82D0();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F1D82D0(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F240060 - MakeAbilityAgent_Arev.  Type-checked slot ctor 0x6F223E80,
//  Assign 0x6F216000, rawcode getter 0x6F028170 ('Arev' = 0x41726576).
struct SCheckedAbilitySlot_Arev
{
    SCheckedAbilitySlot_Arev(CAgent* candidate);          //  0x6F223E80
    SCheckedAbilitySlot_Arev* Assign(CAgent* candidate);  //  0x6F216000

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_Arev() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F028170();   //  0x6F028170 - 'Arev'

CAgent* MakeAbilityAgent_Arev()
{
    SCheckedAbilitySlot_Arev held(0);

    unsigned int typeId = GetFourCC_6F028170();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F028170(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F2402D0 - MakeAbilityAgent_Asac.  Type-checked slot ctor 0x6F223FA0,
//  Assign 0x6F216180, rawcode getter 0x6F1D3D80 ('Asac' = 0x41736163).
struct SCheckedAbilitySlot_Asac
{
    SCheckedAbilitySlot_Asac(CAgent* candidate);          //  0x6F223FA0
    SCheckedAbilitySlot_Asac* Assign(CAgent* candidate);  //  0x6F216180

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_Asac() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F1D3D80();   //  0x6F1D3D80 - 'Asac'

CAgent* MakeAbilityAgent_Asac()
{
    SCheckedAbilitySlot_Asac held(0);

    unsigned int typeId = GetFourCC_6F1D3D80();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F1D3D80(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F2407B0 - MakeAbilityAgent_Asud.  Type-checked slot ctor 0x6F224180,
//  Assign 0x6F216400, rawcode getter 0x6F0287C0 ('Asud' = 0x41737564).
struct SCheckedAbilitySlot_Asud
{
    SCheckedAbilitySlot_Asud(CAgent* candidate);          //  0x6F224180
    SCheckedAbilitySlot_Asud* Assign(CAgent* candidate);  //  0x6F216400

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_Asud() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F0287C0();   //  0x6F0287C0 - 'Asud'

CAgent* MakeAbilityAgent_Asud()
{
    SCheckedAbilitySlot_Asud held(0);

    unsigned int typeId = GetFourCC_6F0287C0();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F0287C0(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F240AF0 - MakeAbilityAgent_Aslo.  Type-checked slot ctor 0x6F224300,
//  Assign 0x6F216600, rawcode getter 0x6F1D6450 ('Aslo' = 0x41736C6F).
struct SCheckedAbilitySlot_Aslo
{
    SCheckedAbilitySlot_Aslo(CAgent* candidate);          //  0x6F224300
    SCheckedAbilitySlot_Aslo* Assign(CAgent* candidate);  //  0x6F216600

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_Aslo() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F1D6450();   //  0x6F1D6450 - 'Aslo'

CAgent* MakeAbilityAgent_Aslo()
{
    SCheckedAbilitySlot_Aslo held(0);

    unsigned int typeId = GetFourCC_6F1D6450();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F1D6450(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F2410A0 - MakeAbilityAgent_Aspi.  Type-checked slot ctor 0x6F2245A0,
//  Assign 0x6F216980, rawcode getter 0x6F1D7210 ('Aspi' = 0x41737069).
struct SCheckedAbilitySlot_Aspi
{
    SCheckedAbilitySlot_Aspi(CAgent* candidate);          //  0x6F2245A0
    SCheckedAbilitySlot_Aspi* Assign(CAgent* candidate);  //  0x6F216980

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_Aspi() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F1D7210();   //  0x6F1D7210 - 'Aspi'

CAgent* MakeAbilityAgent_Aspi()
{
    SCheckedAbilitySlot_Aspi held(0);

    unsigned int typeId = GetFourCC_6F1D7210();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F1D7210(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F2413E0 - MakeAbilityAgent_Asta.  Type-checked slot ctor 0x6F2246C0,
//  Assign 0x6F216B00, rawcode getter 0x6F1D8150 ('Asta' = 0x41737461).
struct SCheckedAbilitySlot_Asta
{
    SCheckedAbilitySlot_Asta(CAgent* candidate);          //  0x6F2246C0
    SCheckedAbilitySlot_Asta* Assign(CAgent* candidate);  //  0x6F216B00

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_Asta() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F1D8150();   //  0x6F1D8150 - 'Asta'

CAgent* MakeAbilityAgent_Asta()
{
    SCheckedAbilitySlot_Asta held(0);

    unsigned int typeId = GetFourCC_6F1D8150();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F1D8150(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F2418C0 - MakeAbilityAgent_Atdp.  Type-checked slot ctor 0x6F224900,
//  Assign 0x6F216E00, rawcode getter 0x6F1564E0 ('Atdp' = 0x41746470).
struct SCheckedAbilitySlot_Atdp
{
    SCheckedAbilitySlot_Atdp(CAgent* candidate);          //  0x6F224900
    SCheckedAbilitySlot_Atdp* Assign(CAgent* candidate);  //  0x6F216E00

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_Atdp() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F1564E0();   //  0x6F1564E0 - 'Atdp'

CAgent* MakeAbilityAgent_Atdp()
{
    SCheckedAbilitySlot_Atdp held(0);

    unsigned int typeId = GetFourCC_6F1564E0();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F1564E0(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F241B30 - MakeAbilityAgent_Attu.  Type-checked slot ctor 0x6F224A20,
//  Assign 0x6F216F80, rawcode getter 0x6F156660 ('Attu' = 0x41747475).
struct SCheckedAbilitySlot_Attu
{
    SCheckedAbilitySlot_Attu(CAgent* candidate);          //  0x6F224A20
    SCheckedAbilitySlot_Attu* Assign(CAgent* candidate);  //  0x6F216F80

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_Attu() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F156660();   //  0x6F156660 - 'Attu'

CAgent* MakeAbilityAgent_Attu()
{
    SCheckedAbilitySlot_Attu held(0);

    unsigned int typeId = GetFourCC_6F156660();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F156660(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F242010 - MakeAbilityAgent_Auco.  Type-checked slot ctor 0x6F224C60,
//  Assign 0x6F217280, rawcode getter 0x6F1D4540 ('Auco' = 0x4175636F).
struct SCheckedAbilitySlot_Auco
{
    SCheckedAbilitySlot_Auco(CAgent* candidate);          //  0x6F224C60
    SCheckedAbilitySlot_Auco* Assign(CAgent* candidate);  //  0x6F217280

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_Auco() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F1D4540();   //  0x6F1D4540 - 'Auco'

CAgent* MakeAbilityAgent_Auco()
{
    SCheckedAbilitySlot_Auco held(0);

    unsigned int typeId = GetFourCC_6F1D4540();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F1D4540(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F242280 - MakeAbilityAgent_Aven.  Type-checked slot ctor 0x6F224D20,
//  Assign 0x6F217380, rawcode getter 0x6F113A50 ('Aven' = 0x4176656E).
struct SCheckedAbilitySlot_Aven
{
    SCheckedAbilitySlot_Aven(CAgent* candidate);          //  0x6F224D20
    SCheckedAbilitySlot_Aven* Assign(CAgent* candidate);  //  0x6F217380

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_Aven() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F113A50();   //  0x6F113A50 - 'Aven'

CAgent* MakeAbilityAgent_Aven()
{
    SCheckedAbilitySlot_Aven held(0);

    unsigned int typeId = GetFourCC_6F113A50();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F113A50(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F242690 - MakeAbilityAgent_Awha.  Type-checked slot ctor 0x6F092340,
//  Assign 0x6F0845F0, rawcode getter 0x6F15A310 ('Awha' = 0x41776861).
struct SCheckedAbilitySlot_Awha
{
    SCheckedAbilitySlot_Awha(CAgent* candidate);          //  0x6F092340
    SCheckedAbilitySlot_Awha* Assign(CAgent* candidate);  //  0x6F0845F0

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_Awha() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F15A310();   //  0x6F15A310 - 'Awha'

CAgent* MakeAbilityAgent_Awha()
{
    SCheckedAbilitySlot_Awha held(0);

    unsigned int typeId = GetFourCC_6F15A310();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F15A310(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F243460 - MakeAbilityAgent_AEbl.  Type-checked slot ctor 0x6F2254A0,
//  Assign 0x6F217D80, rawcode getter 0x6F197690 ('AEbl' = 0x4145626C).
struct SCheckedAbilitySlot_AEbl
{
    SCheckedAbilitySlot_AEbl(CAgent* candidate);          //  0x6F2254A0
    SCheckedAbilitySlot_AEbl* Assign(CAgent* candidate);  //  0x6F217D80

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_AEbl() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F197690();   //  0x6F197690 - 'AEbl'

CAgent* MakeAbilityAgent_AEbl()
{
    SCheckedAbilitySlot_AEbl held(0);

    unsigned int typeId = GetFourCC_6F197690();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F197690(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F2454E0 - MakeAbilityAgent_AHhb.  Type-checked slot ctor 0x6F226340,
//  Assign 0x6F219100, rawcode getter 0x6F082AF0 ('AHhb' = 0x41486862).
struct SCheckedAbilitySlot_AHhb
{
    SCheckedAbilitySlot_AHhb(CAgent* candidate);          //  0x6F226340
    SCheckedAbilitySlot_AHhb* Assign(CAgent* candidate);  //  0x6F219100

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_AHhb() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F082AF0();   //  0x6F082AF0 - 'AHhb'

CAgent* MakeAbilityAgent_AHhb()
{
    SCheckedAbilitySlot_AHhb held(0);

    unsigned int typeId = GetFourCC_6F082AF0();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F082AF0(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F2462B0 - MakeAbilityAgent_AOre.  Type-checked slot ctor 0x6F2269A0,
//  Assign 0x6F219980, rawcode getter 0x6F1D2FA0 ('AOre' = 0x414F7265).
struct SCheckedAbilitySlot_AOre
{
    SCheckedAbilitySlot_AOre(CAgent* candidate);          //  0x6F2269A0
    SCheckedAbilitySlot_AOre* Assign(CAgent* candidate);  //  0x6F219980

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_AOre() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F1D2FA0();   //  0x6F1D2FA0 - 'AOre'

CAgent* MakeAbilityAgent_AOre()
{
    SCheckedAbilitySlot_AOre held(0);

    unsigned int typeId = GetFourCC_6F1D2FA0();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F1D2FA0(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F247560 - MakeAbilityAgent_Atsp.  Type-checked slot ctor 0x6F227240,
//  Assign 0x6F21A500, rawcode getter 0x6F158240 ('Atsp' = 0x41747370).
struct SCheckedAbilitySlot_Atsp
{
    SCheckedAbilitySlot_Atsp(CAgent* candidate);          //  0x6F227240
    SCheckedAbilitySlot_Atsp* Assign(CAgent* candidate);  //  0x6F21A500

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_Atsp() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F158240();   //  0x6F158240 - 'Atsp'

CAgent* MakeAbilityAgent_Atsp()
{
    SCheckedAbilitySlot_Atsp held(0);

    unsigned int typeId = GetFourCC_6F158240();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F158240(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F2485A0 - MakeAbilityAgent_ANlm.  Type-checked slot ctor 0x6F227960,
//  Assign 0x6F21AE80, rawcode getter 0x6F080680 ('ANlm' = 0x414E6C6D).
struct SCheckedAbilitySlot_ANlm
{
    SCheckedAbilitySlot_ANlm(CAgent* candidate);          //  0x6F227960
    SCheckedAbilitySlot_ANlm* Assign(CAgent* candidate);  //  0x6F21AE80

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_ANlm() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F080680();   //  0x6F080680 - 'ANlm'

CAgent* MakeAbilityAgent_ANlm()
{
    SCheckedAbilitySlot_ANlm held(0);

    unsigned int typeId = GetFourCC_6F080680();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F080680(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F24A550 - MakeAbilityAgent_Apo2.  Type-checked slot ctor 0x6F0EC240,
//  Assign 0x6F0E07C0, rawcode getter 0x6F0DBF00 ('Apo2' = 0x41706F32).
struct SCheckedAbilitySlot_Apo2
{
    SCheckedAbilitySlot_Apo2(CAgent* candidate);          //  0x6F0EC240
    SCheckedAbilitySlot_Apo2* Assign(CAgent* candidate);  //  0x6F0E07C0

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_Apo2() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F0DBF00();   //  0x6F0DBF00 - 'Apo2'

CAgent* MakeAbilityAgent_Apo2()
{
    SCheckedAbilitySlot_Apo2 held(0);

    unsigned int typeId = GetFourCC_6F0DBF00();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F0DBF00(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//  0x6F24B3F0 - MakeAbilityAgent_AIrd.  Type-checked slot ctor 0x6F228E60,
//  Assign 0x6F21CA80, rawcode getter 0x6F0DE7C0 ('AIrd' = 0x41497264).
struct SCheckedAbilitySlot_AIrd
{
    SCheckedAbilitySlot_AIrd(CAgent* candidate);          //  0x6F228E60
    SCheckedAbilitySlot_AIrd* Assign(CAgent* candidate);  //  0x6F21CA80

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_AIrd() { Release(); }

    CAgent* m_value;   // +0x00
};

unsigned int GetFourCC_6F0DE7C0();   //  0x6F0DE7C0 - 'AIrd'

CAgent* MakeAbilityAgent_AIrd()
{
    SCheckedAbilitySlot_AIrd held(0);

    unsigned int typeId = GetFourCC_6F0DE7C0();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F0DE7C0(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}


//----------------------------------------------------------------------------
//  Out-of-slice bodies for this batch's own 19 addresses above: each ctor/
//  Assign/rawcode-getter triple belongs to a different BFS slice (the
//  type-checked slot's own constructor and Assign, and the rawcode leaf),
//  none of them this batch's own worklist target. Defined here as naked
//  self-redirect thunks straight into the real, unhooked shipped function -
//  same convention cunit_agent_seh_abilityfactories.cpp uses - so this TU
//  links (link_check.py) without claiming any of these addresses in
//  funcmap.py. Every getter stays a self-redirect thunk rather than a real
//  `return 0x...;` body for the same inlining reason the rest of this file
//  gives: a real one-line body in the same TU as its two call sites is
//  visible to /O2's own inliner and drops the `call` the shipped dump keeps.
//  retn sizes are read off each target's own raw_bytes: all thiscall ctor/
//  Assign pairs retn 4, all rawcode getters retn 0.
//----------------------------------------------------------------------------
__declspec(naked) SCheckedAbilitySlot_Arsk::SCheckedAbilitySlot_Arsk(CAgent*)
{
    __asm { mov eax, 06F223CA0h }
    __asm { jmp eax }
}
__declspec(naked) SCheckedAbilitySlot_Arsk* SCheckedAbilitySlot_Arsk::Assign(CAgent*)
{
    __asm { mov eax, 06F215D80h }
    __asm { jmp eax }
}
__declspec(naked) unsigned int GetFourCC_6F1D82D0()
{
    __asm { mov eax, 06F1D82D0h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Arev::SCheckedAbilitySlot_Arev(CAgent*)
{
    __asm { mov eax, 06F223E80h }
    __asm { jmp eax }
}
__declspec(naked) SCheckedAbilitySlot_Arev* SCheckedAbilitySlot_Arev::Assign(CAgent*)
{
    __asm { mov eax, 06F216000h }
    __asm { jmp eax }
}
__declspec(naked) unsigned int GetFourCC_6F028170()
{
    __asm { mov eax, 06F028170h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Asac::SCheckedAbilitySlot_Asac(CAgent*)
{
    __asm { mov eax, 06F223FA0h }
    __asm { jmp eax }
}
__declspec(naked) SCheckedAbilitySlot_Asac* SCheckedAbilitySlot_Asac::Assign(CAgent*)
{
    __asm { mov eax, 06F216180h }
    __asm { jmp eax }
}
__declspec(naked) unsigned int GetFourCC_6F1D3D80()
{
    __asm { mov eax, 06F1D3D80h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Asud::SCheckedAbilitySlot_Asud(CAgent*)
{
    __asm { mov eax, 06F224180h }
    __asm { jmp eax }
}
__declspec(naked) SCheckedAbilitySlot_Asud* SCheckedAbilitySlot_Asud::Assign(CAgent*)
{
    __asm { mov eax, 06F216400h }
    __asm { jmp eax }
}
__declspec(naked) unsigned int GetFourCC_6F0287C0()
{
    __asm { mov eax, 06F0287C0h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Aslo::SCheckedAbilitySlot_Aslo(CAgent*)
{
    __asm { mov eax, 06F224300h }
    __asm { jmp eax }
}
__declspec(naked) SCheckedAbilitySlot_Aslo* SCheckedAbilitySlot_Aslo::Assign(CAgent*)
{
    __asm { mov eax, 06F216600h }
    __asm { jmp eax }
}
__declspec(naked) unsigned int GetFourCC_6F1D6450()
{
    __asm { mov eax, 06F1D6450h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Aspi::SCheckedAbilitySlot_Aspi(CAgent*)
{
    __asm { mov eax, 06F2245A0h }
    __asm { jmp eax }
}
__declspec(naked) SCheckedAbilitySlot_Aspi* SCheckedAbilitySlot_Aspi::Assign(CAgent*)
{
    __asm { mov eax, 06F216980h }
    __asm { jmp eax }
}
__declspec(naked) unsigned int GetFourCC_6F1D7210()
{
    __asm { mov eax, 06F1D7210h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Asta::SCheckedAbilitySlot_Asta(CAgent*)
{
    __asm { mov eax, 06F2246C0h }
    __asm { jmp eax }
}
__declspec(naked) SCheckedAbilitySlot_Asta* SCheckedAbilitySlot_Asta::Assign(CAgent*)
{
    __asm { mov eax, 06F216B00h }
    __asm { jmp eax }
}
__declspec(naked) unsigned int GetFourCC_6F1D8150()
{
    __asm { mov eax, 06F1D8150h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Atdp::SCheckedAbilitySlot_Atdp(CAgent*)
{
    __asm { mov eax, 06F224900h }
    __asm { jmp eax }
}
__declspec(naked) SCheckedAbilitySlot_Atdp* SCheckedAbilitySlot_Atdp::Assign(CAgent*)
{
    __asm { mov eax, 06F216E00h }
    __asm { jmp eax }
}
__declspec(naked) unsigned int GetFourCC_6F1564E0()
{
    __asm { mov eax, 06F1564E0h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Attu::SCheckedAbilitySlot_Attu(CAgent*)
{
    __asm { mov eax, 06F224A20h }
    __asm { jmp eax }
}
__declspec(naked) SCheckedAbilitySlot_Attu* SCheckedAbilitySlot_Attu::Assign(CAgent*)
{
    __asm { mov eax, 06F216F80h }
    __asm { jmp eax }
}
__declspec(naked) unsigned int GetFourCC_6F156660()
{
    __asm { mov eax, 06F156660h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Auco::SCheckedAbilitySlot_Auco(CAgent*)
{
    __asm { mov eax, 06F224C60h }
    __asm { jmp eax }
}
__declspec(naked) SCheckedAbilitySlot_Auco* SCheckedAbilitySlot_Auco::Assign(CAgent*)
{
    __asm { mov eax, 06F217280h }
    __asm { jmp eax }
}
__declspec(naked) unsigned int GetFourCC_6F1D4540()
{
    __asm { mov eax, 06F1D4540h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Aven::SCheckedAbilitySlot_Aven(CAgent*)
{
    __asm { mov eax, 06F224D20h }
    __asm { jmp eax }
}
__declspec(naked) SCheckedAbilitySlot_Aven* SCheckedAbilitySlot_Aven::Assign(CAgent*)
{
    __asm { mov eax, 06F217380h }
    __asm { jmp eax }
}
__declspec(naked) unsigned int GetFourCC_6F113A50()
{
    __asm { mov eax, 06F113A50h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Awha::SCheckedAbilitySlot_Awha(CAgent*)
{
    __asm { mov eax, 06F092340h }
    __asm { jmp eax }
}
__declspec(naked) SCheckedAbilitySlot_Awha* SCheckedAbilitySlot_Awha::Assign(CAgent*)
{
    __asm { mov eax, 06F0845F0h }
    __asm { jmp eax }
}
__declspec(naked) unsigned int GetFourCC_6F15A310()
{
    __asm { mov eax, 06F15A310h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_AEbl::SCheckedAbilitySlot_AEbl(CAgent*)
{
    __asm { mov eax, 06F2254A0h }
    __asm { jmp eax }
}
__declspec(naked) SCheckedAbilitySlot_AEbl* SCheckedAbilitySlot_AEbl::Assign(CAgent*)
{
    __asm { mov eax, 06F217D80h }
    __asm { jmp eax }
}
__declspec(naked) unsigned int GetFourCC_6F197690()
{
    __asm { mov eax, 06F197690h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_AHhb::SCheckedAbilitySlot_AHhb(CAgent*)
{
    __asm { mov eax, 06F226340h }
    __asm { jmp eax }
}
__declspec(naked) SCheckedAbilitySlot_AHhb* SCheckedAbilitySlot_AHhb::Assign(CAgent*)
{
    __asm { mov eax, 06F219100h }
    __asm { jmp eax }
}
__declspec(naked) unsigned int GetFourCC_6F082AF0()
{
    __asm { mov eax, 06F082AF0h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_AOre::SCheckedAbilitySlot_AOre(CAgent*)
{
    __asm { mov eax, 06F2269A0h }
    __asm { jmp eax }
}
__declspec(naked) SCheckedAbilitySlot_AOre* SCheckedAbilitySlot_AOre::Assign(CAgent*)
{
    __asm { mov eax, 06F219980h }
    __asm { jmp eax }
}
__declspec(naked) unsigned int GetFourCC_6F1D2FA0()
{
    __asm { mov eax, 06F1D2FA0h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Atsp::SCheckedAbilitySlot_Atsp(CAgent*)
{
    __asm { mov eax, 06F227240h }
    __asm { jmp eax }
}
__declspec(naked) SCheckedAbilitySlot_Atsp* SCheckedAbilitySlot_Atsp::Assign(CAgent*)
{
    __asm { mov eax, 06F21A500h }
    __asm { jmp eax }
}
__declspec(naked) unsigned int GetFourCC_6F158240()
{
    __asm { mov eax, 06F158240h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_ANlm::SCheckedAbilitySlot_ANlm(CAgent*)
{
    __asm { mov eax, 06F227960h }
    __asm { jmp eax }
}
__declspec(naked) SCheckedAbilitySlot_ANlm* SCheckedAbilitySlot_ANlm::Assign(CAgent*)
{
    __asm { mov eax, 06F21AE80h }
    __asm { jmp eax }
}
__declspec(naked) unsigned int GetFourCC_6F080680()
{
    __asm { mov eax, 06F080680h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Apo2::SCheckedAbilitySlot_Apo2(CAgent*)
{
    __asm { mov eax, 06F0EC240h }
    __asm { jmp eax }
}
__declspec(naked) SCheckedAbilitySlot_Apo2* SCheckedAbilitySlot_Apo2::Assign(CAgent*)
{
    __asm { mov eax, 06F0E07C0h }
    __asm { jmp eax }
}
__declspec(naked) unsigned int GetFourCC_6F0DBF00()
{
    __asm { mov eax, 06F0DBF00h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_AIrd::SCheckedAbilitySlot_AIrd(CAgent*)
{
    __asm { mov eax, 06F228E60h }
    __asm { jmp eax }
}
__declspec(naked) SCheckedAbilitySlot_AIrd* SCheckedAbilitySlot_AIrd::Assign(CAgent*)
{
    __asm { mov eax, 06F21CA80h }
    __asm { jmp eax }
}
__declspec(naked) unsigned int GetFourCC_6F0DE7C0()
{
    __asm { mov eax, 06F0DE7C0h }
    __asm { jmp eax }
}
