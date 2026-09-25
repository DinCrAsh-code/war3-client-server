//============================================================================
//  cunit_agent4 batch: 10 instances of the agile-type-agent factory
//  family agenttypedslots.h / unitordermake.cpp already document - each one
//  the same eight-step MakeOrderAgent template (look the fixed rawcode up
//  in the game-wide agile type registry, take that type's own allocator,
//  hand pool and type id to the global agent factory through the same
//  0x2C-byte request block, Assign the returned holder's own +0x54 CAgent*
//  into a type-checked scoped slot, return it - the slot's destructor
//  running inline at the one return, exactly as MakeOrderAgent's does).
//
//  Reached from CUnit's own vtable-closure BFS (cunit_agent4_worklist.md),
//  assigned to this agent (cunit-agent4) as part of an eight-way split of
//  480 functions.  Every constructor/Assign/rawcode-getter this file calls
//  is a *different* address from the one being reconstructed here, and none
//  of those callee addresses are in this agent's own 60-address worklist -
//  they belong to whichever sibling slice's own BFS walk reaches them, so
//  they are redirected with a plain naked thunk (the same technique
//  src/Item/abilityfieldflagclosure_thunks.cpp already uses for an
//  out-of-scope callee) rather than reconstructed here.  Every thunk symbol
//  carries an "Agent4" infix precisely so that if a sibling slice's own
//  worklist reaches the very same out-of-scope address independently, the
//  two sessions cannot emit the same mangled name for it.
//
//  None of the "A???"-shaped fixed rawcodes below (AItp/AHds/Aams/... - all
//  read MSB-first, the same convention misc_rawcode_getters.cpp's own
//  table uses) resolve to a member of that file's own catalogued agile-type
//  family, so - matching checkedslot_batch25.cpp's own rule for a constant
//  with no established meaning - both the getter and the slot type keep an
//  address-derived name here rather than a guessed one.
//============================================================================
#include "agent.h"
#include "agiletype.h"
#include "widgetagentquery.h"
#include "game.h"

//  See src/Item/abilityfieldflagclosure_thunks.cpp for the established
//  naked-redirect convention this follows.
#define AGENT4_THUNK(sig, addr)         \
    __declspec(naked) sig               \
    {                                    \
        __asm { mov eax, addr }         \
        __asm { jmp eax }                \
    }


//----------------------------------------------------------------------------
//  0x6F246450 - MakeAgent_6F246450: an 'AEst'-tagged agile-type
//  factory, the same template as MakeOrderAgent (0x6F294A40,
//  unitordermake.cpp).  Own translation unit: the callers of this whole
//  family call each instance for real.
//----------------------------------------------------------------------------
struct SAgent4Slot_6F226A60
{
    //  0x6F226A60 - out of scope for this agent, see file header.
    SAgent4Slot_6F226A60(CAgent* candidate);

    //  0x6F219A80 - out of scope for this agent, see file header.
    SAgent4Slot_6F226A60* Assign(CAgent* candidate);

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            held->m_refcount += -1;
            if (held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    ~SAgent4Slot_6F226A60() { Release(); }

    CAgent* m_value;
};

AGENT4_THUNK(SAgent4Slot_6F226A60::SAgent4Slot_6F226A60(CAgent*), 0x6F226A60)
AGENT4_THUNK(SAgent4Slot_6F226A60* SAgent4Slot_6F226A60::Assign(CAgent*), 0x6F219A80)

//  0x6F1D43F0 - 'AEst', MSB-first.  Out of scope for this agent
//  (see file header); redirected rather than reconstructed.
AGENT4_THUNK(unsigned int GetFixedRawcode_6F1D43F0(), 0x6F1D43F0)

//  What the maker hands back - only the CAgent* at +0x54 is reached, the
//  same shape every other member of this family reads through
//  (unitordermake.cpp, jassrelationagent.cpp, checkedslot_batch26.cpp, ...).
struct SMadeAgent_6F246450
{
    char    m_reserved00[0x54];
    CAgent* m_pAgent;   // +0x54
};

CAgent* __fastcall MakeAgent_6F246450()
{
    //  Constructed empty and assigned in one step - SAgent4Slot_6F226A60's
    //  own converting constructor.
    SAgent4Slot_6F226A60 held(0);

    unsigned int typeId = GetFixedRawcode_6F1D43F0();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    //  The getter called a second time here rather than kept in a register -
    //  the shipped stream has two real calls to it, not one, the same shape
    //  MakeOrderAgent's own header describes for the location factory.
    InitWidgetAgentQuery(&query, GetFixedRawcode_6F1D43F0(), type->m_allocator);
    //  Stored again after Init has already stored it - the shipped code has
    //  both stores, and dropping this one loses an instruction (same note
    //  as MakeOrderAgent's own).
    query.m_reserved24 = 0xFFFFFFFF;

    held.Assign(((SMadeAgent_6F246450*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}

//----------------------------------------------------------------------------
//  0x6F246AD0 - MakeAgent_6F246AD0: an 'ANst'-tagged agile-type
//  factory, the same template as MakeOrderAgent (0x6F294A40,
//  unitordermake.cpp).  Own translation unit: the callers of this whole
//  family call each instance for real.
//----------------------------------------------------------------------------
struct SAgent4Slot_6F226D60
{
    //  0x6F226D60 - out of scope for this agent, see file header.
    SAgent4Slot_6F226D60(CAgent* candidate);

    //  0x6F219E80 - out of scope for this agent, see file header.
    SAgent4Slot_6F226D60* Assign(CAgent* candidate);

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            held->m_refcount += -1;
            if (held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    ~SAgent4Slot_6F226D60() { Release(); }

    CAgent* m_value;
};

AGENT4_THUNK(SAgent4Slot_6F226D60::SAgent4Slot_6F226D60(CAgent*), 0x6F226D60)
AGENT4_THUNK(SAgent4Slot_6F226D60* SAgent4Slot_6F226D60::Assign(CAgent*), 0x6F219E80)

//  0x6F1D7D20 - 'ANst', MSB-first.  Out of scope for this agent
//  (see file header); redirected rather than reconstructed.
AGENT4_THUNK(unsigned int GetFixedRawcode_6F1D7D20(), 0x6F1D7D20)

//  What the maker hands back - only the CAgent* at +0x54 is reached, the
//  same shape every other member of this family reads through
//  (unitordermake.cpp, jassrelationagent.cpp, checkedslot_batch26.cpp, ...).
struct SMadeAgent_6F246AD0
{
    char    m_reserved00[0x54];
    CAgent* m_pAgent;   // +0x54
};

CAgent* __fastcall MakeAgent_6F246AD0()
{
    //  Constructed empty and assigned in one step - SAgent4Slot_6F226D60's
    //  own converting constructor.
    SAgent4Slot_6F226D60 held(0);

    unsigned int typeId = GetFixedRawcode_6F1D7D20();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    //  The getter called a second time here rather than kept in a register -
    //  the shipped stream has two real calls to it, not one, the same shape
    //  MakeOrderAgent's own header describes for the location factory.
    InitWidgetAgentQuery(&query, GetFixedRawcode_6F1D7D20(), type->m_allocator);
    //  Stored again after Init has already stored it - the shipped code has
    //  both stores, and dropping this one loses an instruction (same note
    //  as MakeOrderAgent's own).
    query.m_reserved24 = 0xFFFFFFFF;

    held.Assign(((SMadeAgent_6F246AD0*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}

//----------------------------------------------------------------------------
//  0x6F247150 - MakeAgent_6F247150: an 'AHtb'-tagged agile-type
//  factory, the same template as MakeOrderAgent (0x6F294A40,
//  unitordermake.cpp).  Own translation unit: the callers of this whole
//  family call each instance for real.
//----------------------------------------------------------------------------
struct SAgent4Slot_6F227060
{
    //  0x6F227060 - out of scope for this agent, see file header.
    SAgent4Slot_6F227060(CAgent* candidate);

    //  0x6F21A280 - out of scope for this agent, see file header.
    SAgent4Slot_6F227060* Assign(CAgent* candidate);

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            held->m_refcount += -1;
            if (held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    ~SAgent4Slot_6F227060() { Release(); }

    CAgent* m_value;
};

AGENT4_THUNK(SAgent4Slot_6F227060::SAgent4Slot_6F227060(CAgent*), 0x6F227060)
AGENT4_THUNK(SAgent4Slot_6F227060* SAgent4Slot_6F227060::Assign(CAgent*), 0x6F21A280)

//  0x6F156C40 - 'AHtb', MSB-first.  Out of scope for this agent
//  (see file header); redirected rather than reconstructed.
AGENT4_THUNK(unsigned int GetFixedRawcode_6F156C40(), 0x6F156C40)

//  What the maker hands back - only the CAgent* at +0x54 is reached, the
//  same shape every other member of this family reads through
//  (unitordermake.cpp, jassrelationagent.cpp, checkedslot_batch26.cpp, ...).
struct SMadeAgent_6F247150
{
    char    m_reserved00[0x54];
    CAgent* m_pAgent;   // +0x54
};

CAgent* __fastcall MakeAgent_6F247150()
{
    //  Constructed empty and assigned in one step - SAgent4Slot_6F227060's
    //  own converting constructor.
    SAgent4Slot_6F227060 held(0);

    unsigned int typeId = GetFixedRawcode_6F156C40();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    //  The getter called a second time here rather than kept in a register -
    //  the shipped stream has two real calls to it, not one, the same shape
    //  MakeOrderAgent's own header describes for the location factory.
    InitWidgetAgentQuery(&query, GetFixedRawcode_6F156C40(), type->m_allocator);
    //  Stored again after Init has already stored it - the shipped code has
    //  both stores, and dropping this one loses an instruction (same note
    //  as MakeOrderAgent's own).
    query.m_reserved24 = 0xFFFFFFFF;

    held.Assign(((SMadeAgent_6F247150*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}

//----------------------------------------------------------------------------
//  0x6F247A40 - MakeAgent_6F247A40: an 'AOww'-tagged agile-type
//  factory, the same template as MakeOrderAgent (0x6F294A40,
//  unitordermake.cpp).  Own translation unit: the callers of this whole
//  family call each instance for real.
//----------------------------------------------------------------------------
struct SAgent4Slot_6F227480
{
    //  0x6F227480 - out of scope for this agent, see file header.
    SAgent4Slot_6F227480(CAgent* candidate);

    //  0x6F21A800 - out of scope for this agent, see file header.
    SAgent4Slot_6F227480* Assign(CAgent* candidate);

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            held->m_refcount += -1;
            if (held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    ~SAgent4Slot_6F227480() { Release(); }

    CAgent* m_value;
};

AGENT4_THUNK(SAgent4Slot_6F227480::SAgent4Slot_6F227480(CAgent*), 0x6F227480)
AGENT4_THUNK(SAgent4Slot_6F227480* SAgent4Slot_6F227480::Assign(CAgent*), 0x6F21A800)

//  0x6F159FB0 - 'AOww', MSB-first.  Out of scope for this agent
//  (see file header); redirected rather than reconstructed.
AGENT4_THUNK(unsigned int GetFixedRawcode_6F159FB0(), 0x6F159FB0)

//  What the maker hands back - only the CAgent* at +0x54 is reached, the
//  same shape every other member of this family reads through
//  (unitordermake.cpp, jassrelationagent.cpp, checkedslot_batch26.cpp, ...).
struct SMadeAgent_6F247A40
{
    char    m_reserved00[0x54];
    CAgent* m_pAgent;   // +0x54
};

CAgent* __fastcall MakeAgent_6F247A40()
{
    //  Constructed empty and assigned in one step - SAgent4Slot_6F227480's
    //  own converting constructor.
    SAgent4Slot_6F227480 held(0);

    unsigned int typeId = GetFixedRawcode_6F159FB0();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    //  The getter called a second time here rather than kept in a register -
    //  the shipped stream has two real calls to it, not one, the same shape
    //  MakeOrderAgent's own header describes for the location factory.
    InitWidgetAgentQuery(&query, GetFixedRawcode_6F159FB0(), type->m_allocator);
    //  Stored again after Init has already stored it - the shipped code has
    //  both stores, and dropping this one loses an instruction (same note
    //  as MakeOrderAgent's own).
    query.m_reserved24 = 0xFFFFFFFF;

    held.Assign(((SMadeAgent_6F247A40*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}

//----------------------------------------------------------------------------
//  0x6F2480C0 - MakeAgent_6F2480C0: an 'ANeg'-tagged agile-type
//  factory, the same template as MakeOrderAgent (0x6F294A40,
//  unitordermake.cpp).  Own translation unit: the callers of this whole
//  family call each instance for real.
//----------------------------------------------------------------------------
struct SAgent4Slot_6F227720
{
    //  0x6F227720 - out of scope for this agent, see file header.
    SAgent4Slot_6F227720(CAgent* candidate);

    //  0x6F21AB80 - out of scope for this agent, see file header.
    SAgent4Slot_6F227720* Assign(CAgent* candidate);

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            held->m_refcount += -1;
            if (held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    ~SAgent4Slot_6F227720() { Release(); }

    CAgent* m_value;
};

AGENT4_THUNK(SAgent4Slot_6F227720::SAgent4Slot_6F227720(CAgent*), 0x6F227720)
AGENT4_THUNK(SAgent4Slot_6F227720* SAgent4Slot_6F227720::Assign(CAgent*), 0x6F21AB80)

//  0x6F157780 - 'ANeg', MSB-first.  Out of scope for this agent
//  (see file header); redirected rather than reconstructed.
AGENT4_THUNK(unsigned int GetFixedRawcode_6F157780(), 0x6F157780)

//  What the maker hands back - only the CAgent* at +0x54 is reached, the
//  same shape every other member of this family reads through
//  (unitordermake.cpp, jassrelationagent.cpp, checkedslot_batch26.cpp, ...).
struct SMadeAgent_6F2480C0
{
    char    m_reserved00[0x54];
    CAgent* m_pAgent;   // +0x54
};

CAgent* __fastcall MakeAgent_6F2480C0()
{
    //  Constructed empty and assigned in one step - SAgent4Slot_6F227720's
    //  own converting constructor.
    SAgent4Slot_6F227720 held(0);

    unsigned int typeId = GetFixedRawcode_6F157780();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    //  The getter called a second time here rather than kept in a register -
    //  the shipped stream has two real calls to it, not one, the same shape
    //  MakeOrderAgent's own header describes for the location factory.
    InitWidgetAgentQuery(&query, GetFixedRawcode_6F157780(), type->m_allocator);
    //  Stored again after Init has already stored it - the shipped code has
    //  both stores, and dropping this one loses an instruction (same note
    //  as MakeOrderAgent's own).
    query.m_reserved24 = 0xFFFFFFFF;

    held.Assign(((SMadeAgent_6F2480C0*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}

//----------------------------------------------------------------------------
//  0x6F248740 - MakeAgent_6F248740: an 'ANvc'-tagged agile-type
//  factory, the same template as MakeOrderAgent (0x6F294A40,
//  unitordermake.cpp).  Own translation unit: the callers of this whole
//  family call each instance for real.
//----------------------------------------------------------------------------
struct SAgent4Slot_6F227A20
{
    //  0x6F227A20 - out of scope for this agent, see file header.
    SAgent4Slot_6F227A20(CAgent* candidate);

    //  0x6F21AF80 - out of scope for this agent, see file header.
    SAgent4Slot_6F227A20* Assign(CAgent* candidate);

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            held->m_refcount += -1;
            if (held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    ~SAgent4Slot_6F227A20() { Release(); }

    CAgent* m_value;
};

AGENT4_THUNK(SAgent4Slot_6F227A20::SAgent4Slot_6F227A20(CAgent*), 0x6F227A20)
AGENT4_THUNK(SAgent4Slot_6F227A20* SAgent4Slot_6F227A20::Assign(CAgent*), 0x6F21AF80)

//  0x6F080900 - 'ANvc', MSB-first.  Out of scope for this agent
//  (see file header); redirected rather than reconstructed.
AGENT4_THUNK(unsigned int GetFixedRawcode_6F080900(), 0x6F080900)

//  What the maker hands back - only the CAgent* at +0x54 is reached, the
//  same shape every other member of this family reads through
//  (unitordermake.cpp, jassrelationagent.cpp, checkedslot_batch26.cpp, ...).
struct SMadeAgent_6F248740
{
    char    m_reserved00[0x54];
    CAgent* m_pAgent;   // +0x54
};

CAgent* __fastcall MakeAgent_6F248740()
{
    //  Constructed empty and assigned in one step - SAgent4Slot_6F227A20's
    //  own converting constructor.
    SAgent4Slot_6F227A20 held(0);

    unsigned int typeId = GetFixedRawcode_6F080900();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    //  The getter called a second time here rather than kept in a register -
    //  the shipped stream has two real calls to it, not one, the same shape
    //  MakeOrderAgent's own header describes for the location factory.
    InitWidgetAgentQuery(&query, GetFixedRawcode_6F080900(), type->m_allocator);
    //  Stored again after Init has already stored it - the shipped code has
    //  both stores, and dropping this one loses an instruction (same note
    //  as MakeOrderAgent's own).
    query.m_reserved24 = 0xFFFFFFFF;

    held.Assign(((SMadeAgent_6F248740*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}

//----------------------------------------------------------------------------
//  0x6F248DC0 - MakeAgent_6F248DC0: an 'AIas'-tagged agile-type
//  factory, the same template as MakeOrderAgent (0x6F294A40,
//  unitordermake.cpp).  Own translation unit: the callers of this whole
//  family call each instance for real.
//----------------------------------------------------------------------------
struct SAgent4Slot_6F227D20
{
    //  0x6F227D20 - out of scope for this agent, see file header.
    SAgent4Slot_6F227D20(CAgent* candidate);

    //  0x6F21B380 - out of scope for this agent, see file header.
    SAgent4Slot_6F227D20* Assign(CAgent* candidate);

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            held->m_refcount += -1;
            if (held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    ~SAgent4Slot_6F227D20() { Release(); }

    CAgent* m_value;
};

AGENT4_THUNK(SAgent4Slot_6F227D20::SAgent4Slot_6F227D20(CAgent*), 0x6F227D20)
AGENT4_THUNK(SAgent4Slot_6F227D20* SAgent4Slot_6F227D20::Assign(CAgent*), 0x6F21B380)

//  0x6F0DC8A0 - 'AIas', MSB-first.  Out of scope for this agent
//  (see file header); redirected rather than reconstructed.
AGENT4_THUNK(unsigned int GetFixedRawcode_6F0DC8A0(), 0x6F0DC8A0)

//  What the maker hands back - only the CAgent* at +0x54 is reached, the
//  same shape every other member of this family reads through
//  (unitordermake.cpp, jassrelationagent.cpp, checkedslot_batch26.cpp, ...).
struct SMadeAgent_6F248DC0
{
    char    m_reserved00[0x54];
    CAgent* m_pAgent;   // +0x54
};

CAgent* __fastcall MakeAgent_6F248DC0()
{
    //  Constructed empty and assigned in one step - SAgent4Slot_6F227D20's
    //  own converting constructor.
    SAgent4Slot_6F227D20 held(0);

    unsigned int typeId = GetFixedRawcode_6F0DC8A0();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    //  The getter called a second time here rather than kept in a register -
    //  the shipped stream has two real calls to it, not one, the same shape
    //  MakeOrderAgent's own header describes for the location factory.
    InitWidgetAgentQuery(&query, GetFixedRawcode_6F0DC8A0(), type->m_allocator);
    //  Stored again after Init has already stored it - the shipped code has
    //  both stores, and dropping this one loses an instruction (same note
    //  as MakeOrderAgent's own).
    query.m_reserved24 = 0xFFFFFFFF;

    held.Assign(((SMadeAgent_6F248DC0*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}

//----------------------------------------------------------------------------
//  0x6F249440 - MakeAgent_6F249440: an 'AIfn'-tagged agile-type
//  factory, the same template as MakeOrderAgent (0x6F294A40,
//  unitordermake.cpp).  Own translation unit: the callers of this whole
//  family call each instance for real.
//----------------------------------------------------------------------------
struct SAgent4Slot_6F228020
{
    //  0x6F228020 - out of scope for this agent, see file header.
    SAgent4Slot_6F228020(CAgent* candidate);

    //  0x6F21B780 - out of scope for this agent, see file header.
    SAgent4Slot_6F228020* Assign(CAgent* candidate);

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            held->m_refcount += -1;
            if (held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    ~SAgent4Slot_6F228020() { Release(); }

    CAgent* m_value;
};

AGENT4_THUNK(SAgent4Slot_6F228020::SAgent4Slot_6F228020(CAgent*), 0x6F228020)
AGENT4_THUNK(SAgent4Slot_6F228020* SAgent4Slot_6F228020::Assign(CAgent*), 0x6F21B780)

//  0x6F0DD820 - 'AIfn', MSB-first.  Out of scope for this agent
//  (see file header); redirected rather than reconstructed.
AGENT4_THUNK(unsigned int GetFixedRawcode_6F0DD820(), 0x6F0DD820)

//  What the maker hands back - only the CAgent* at +0x54 is reached, the
//  same shape every other member of this family reads through
//  (unitordermake.cpp, jassrelationagent.cpp, checkedslot_batch26.cpp, ...).
struct SMadeAgent_6F249440
{
    char    m_reserved00[0x54];
    CAgent* m_pAgent;   // +0x54
};

CAgent* __fastcall MakeAgent_6F249440()
{
    //  Constructed empty and assigned in one step - SAgent4Slot_6F228020's
    //  own converting constructor.
    SAgent4Slot_6F228020 held(0);

    unsigned int typeId = GetFixedRawcode_6F0DD820();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    //  The getter called a second time here rather than kept in a register -
    //  the shipped stream has two real calls to it, not one, the same shape
    //  MakeOrderAgent's own header describes for the location factory.
    InitWidgetAgentQuery(&query, GetFixedRawcode_6F0DD820(), type->m_allocator);
    //  Stored again after Init has already stored it - the shipped code has
    //  both stores, and dropping this one loses an instruction (same note
    //  as MakeOrderAgent's own).
    query.m_reserved24 = 0xFFFFFFFF;

    held.Assign(((SMadeAgent_6F249440*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}

//----------------------------------------------------------------------------
//  0x6F249AC0 - MakeAgent_6F249AC0: an 'AIfb'-tagged agile-type
//  factory, the same template as MakeOrderAgent (0x6F294A40,
//  unitordermake.cpp).  Own translation unit: the callers of this whole
//  family call each instance for real.
//----------------------------------------------------------------------------
struct SAgent4Slot_6F228320
{
    //  0x6F228320 - out of scope for this agent, see file header.
    SAgent4Slot_6F228320(CAgent* candidate);

    //  0x6F21BB80 - out of scope for this agent, see file header.
    SAgent4Slot_6F228320* Assign(CAgent* candidate);

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            held->m_refcount += -1;
            if (held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    ~SAgent4Slot_6F228320() { Release(); }

    CAgent* m_value;
};

AGENT4_THUNK(SAgent4Slot_6F228320::SAgent4Slot_6F228320(CAgent*), 0x6F228320)
AGENT4_THUNK(SAgent4Slot_6F228320* SAgent4Slot_6F228320::Assign(CAgent*), 0x6F21BB80)

//  0x6F0DBA80 - 'AIfb', MSB-first.  Out of scope for this agent
//  (see file header); redirected rather than reconstructed.
AGENT4_THUNK(unsigned int GetFixedRawcode_6F0DBA80(), 0x6F0DBA80)

//  What the maker hands back - only the CAgent* at +0x54 is reached, the
//  same shape every other member of this family reads through
//  (unitordermake.cpp, jassrelationagent.cpp, checkedslot_batch26.cpp, ...).
struct SMadeAgent_6F249AC0
{
    char    m_reserved00[0x54];
    CAgent* m_pAgent;   // +0x54
};

CAgent* __fastcall MakeAgent_6F249AC0()
{
    //  Constructed empty and assigned in one step - SAgent4Slot_6F228320's
    //  own converting constructor.
    SAgent4Slot_6F228320 held(0);

    unsigned int typeId = GetFixedRawcode_6F0DBA80();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    //  The getter called a second time here rather than kept in a register -
    //  the shipped stream has two real calls to it, not one, the same shape
    //  MakeOrderAgent's own header describes for the location factory.
    InitWidgetAgentQuery(&query, GetFixedRawcode_6F0DBA80(), type->m_allocator);
    //  Stored again after Init has already stored it - the shipped code has
    //  both stores, and dropping this one loses an instruction (same note
    //  as MakeOrderAgent's own).
    query.m_reserved24 = 0xFFFFFFFF;

    held.Assign(((SMadeAgent_6F249AC0*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}

//----------------------------------------------------------------------------
//  0x6F24A140 - MakeAgent_6F24A140: an 'AImi'-tagged agile-type
//  factory, the same template as MakeOrderAgent (0x6F294A40,
//  unitordermake.cpp).  Own translation unit: the callers of this whole
//  family call each instance for real.
//----------------------------------------------------------------------------
struct SAgent4Slot_6F228620
{
    //  0x6F228620 - out of scope for this agent, see file header.
    SAgent4Slot_6F228620(CAgent* candidate);

    //  0x6F21BF80 - out of scope for this agent, see file header.
    SAgent4Slot_6F228620* Assign(CAgent* candidate);

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            held->m_refcount += -1;
            if (held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }

    ~SAgent4Slot_6F228620() { Release(); }

    CAgent* m_value;
};

AGENT4_THUNK(SAgent4Slot_6F228620::SAgent4Slot_6F228620(CAgent*), 0x6F228620)
AGENT4_THUNK(SAgent4Slot_6F228620* SAgent4Slot_6F228620::Assign(CAgent*), 0x6F21BF80)

//  0x6F0DD220 - 'AImi', MSB-first.  Out of scope for this agent
//  (see file header); redirected rather than reconstructed.
AGENT4_THUNK(unsigned int GetFixedRawcode_6F0DD220(), 0x6F0DD220)

//  What the maker hands back - only the CAgent* at +0x54 is reached, the
//  same shape every other member of this family reads through
//  (unitordermake.cpp, jassrelationagent.cpp, checkedslot_batch26.cpp, ...).
struct SMadeAgent_6F24A140
{
    char    m_reserved00[0x54];
    CAgent* m_pAgent;   // +0x54
};

CAgent* __fastcall MakeAgent_6F24A140()
{
    //  Constructed empty and assigned in one step - SAgent4Slot_6F228620's
    //  own converting constructor.
    SAgent4Slot_6F228620 held(0);

    unsigned int typeId = GetFixedRawcode_6F0DD220();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    //  The getter called a second time here rather than kept in a register -
    //  the shipped stream has two real calls to it, not one, the same shape
    //  MakeOrderAgent's own header describes for the location factory.
    InitWidgetAgentQuery(&query, GetFixedRawcode_6F0DD220(), type->m_allocator);
    //  Stored again after Init has already stored it - the shipped code has
    //  both stores, and dropping this one loses an instruction (same note
    //  as MakeOrderAgent's own).
    query.m_reserved24 = 0xFFFFFFFF;

    held.Assign(((SMadeAgent_6F24A140*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}
