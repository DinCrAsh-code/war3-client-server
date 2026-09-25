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
//  0x6F240880 - MakeAgent_6F240880: an 'Aesn'-tagged agile-type
//  factory, the same template as MakeOrderAgent (0x6F294A40,
//  unitordermake.cpp).  Own translation unit: the callers of this whole
//  family call each instance for real.
//----------------------------------------------------------------------------
struct SAgent4Slot_6F2241E0
{
    //  0x6F2241E0 - out of scope for this agent, see file header.
    SAgent4Slot_6F2241E0(CAgent* candidate);

    //  0x6F216480 - out of scope for this agent, see file header.
    SAgent4Slot_6F2241E0* Assign(CAgent* candidate);

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

    ~SAgent4Slot_6F2241E0() { Release(); }

    CAgent* m_value;
};

AGENT4_THUNK(SAgent4Slot_6F2241E0::SAgent4Slot_6F2241E0(CAgent*), 0x6F2241E0)
AGENT4_THUNK(SAgent4Slot_6F2241E0* SAgent4Slot_6F2241E0::Assign(CAgent*), 0x6F216480)

//  0x6F1D4950 - 'Aesn', MSB-first.  Out of scope for this agent
//  (see file header); redirected rather than reconstructed.
AGENT4_THUNK(unsigned int GetFixedRawcode_6F1D4950(), 0x6F1D4950)

//  What the maker hands back - only the CAgent* at +0x54 is reached, the
//  same shape every other member of this family reads through
//  (unitordermake.cpp, jassrelationagent.cpp, checkedslot_batch26.cpp, ...).
struct SMadeAgent_6F240880
{
    char    m_reserved00[0x54];
    CAgent* m_pAgent;   // +0x54
};

CAgent* __fastcall MakeAgent_6F240880()
{
    //  Constructed empty and assigned in one step - SAgent4Slot_6F2241E0's
    //  own converting constructor.
    SAgent4Slot_6F2241E0 held(0);

    unsigned int typeId = GetFixedRawcode_6F1D4950();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    //  The getter called a second time here rather than kept in a register -
    //  the shipped stream has two real calls to it, not one, the same shape
    //  MakeOrderAgent's own header describes for the location factory.
    InitWidgetAgentQuery(&query, GetFixedRawcode_6F1D4950(), type->m_allocator);
    //  Stored again after Init has already stored it - the shipped code has
    //  both stores, and dropping this one loses an instruction (same note
    //  as MakeOrderAgent's own).
    query.m_reserved24 = 0xFFFFFFFF;

    held.Assign(((SMadeAgent_6F240880*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}

//----------------------------------------------------------------------------
//  0x6F242900 - MakeAgent_6F242900: an 'AHab'-tagged agile-type
//  factory, the same template as MakeOrderAgent (0x6F294A40,
//  unitordermake.cpp).  Own translation unit: the callers of this whole
//  family call each instance for real.
//----------------------------------------------------------------------------
struct SAgent4Slot_6F224F60
{
    //  0x6F224F60 - out of scope for this agent, see file header.
    SAgent4Slot_6F224F60(CAgent* candidate);

    //  0x6F217680 - out of scope for this agent, see file header.
    SAgent4Slot_6F224F60* Assign(CAgent* candidate);

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

    ~SAgent4Slot_6F224F60() { Release(); }

    CAgent* m_value;
};

AGENT4_THUNK(SAgent4Slot_6F224F60::SAgent4Slot_6F224F60(CAgent*), 0x6F224F60)
AGENT4_THUNK(SAgent4Slot_6F224F60* SAgent4Slot_6F224F60::Assign(CAgent*), 0x6F217680)

//  0x6F195AC0 - 'AHab', MSB-first.  Out of scope for this agent
//  (see file header); redirected rather than reconstructed.
AGENT4_THUNK(unsigned int GetFixedRawcode_6F195AC0(), 0x6F195AC0)

//  What the maker hands back - only the CAgent* at +0x54 is reached, the
//  same shape every other member of this family reads through
//  (unitordermake.cpp, jassrelationagent.cpp, checkedslot_batch26.cpp, ...).
struct SMadeAgent_6F242900
{
    char    m_reserved00[0x54];
    CAgent* m_pAgent;   // +0x54
};

CAgent* __fastcall MakeAgent_6F242900()
{
    //  Constructed empty and assigned in one step - SAgent4Slot_6F224F60's
    //  own converting constructor.
    SAgent4Slot_6F224F60 held(0);

    unsigned int typeId = GetFixedRawcode_6F195AC0();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    //  The getter called a second time here rather than kept in a register -
    //  the shipped stream has two real calls to it, not one, the same shape
    //  MakeOrderAgent's own header describes for the location factory.
    InitWidgetAgentQuery(&query, GetFixedRawcode_6F195AC0(), type->m_allocator);
    //  Stored again after Init has already stored it - the shipped code has
    //  both stores, and dropping this one loses an instruction (same note
    //  as MakeOrderAgent's own).
    query.m_reserved24 = 0xFFFFFFFF;

    held.Assign(((SMadeAgent_6F242900*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}

//----------------------------------------------------------------------------
//  0x6F242F80 - MakeAgent_6F242F80: an 'AHav'-tagged agile-type
//  factory, the same template as MakeOrderAgent (0x6F294A40,
//  unitordermake.cpp).  Own translation unit: the callers of this whole
//  family call each instance for real.
//----------------------------------------------------------------------------
struct SAgent4Slot_6F225260
{
    //  0x6F225260 - out of scope for this agent, see file header.
    SAgent4Slot_6F225260(CAgent* candidate);

    //  0x6F217A80 - out of scope for this agent, see file header.
    SAgent4Slot_6F225260* Assign(CAgent* candidate);

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

    ~SAgent4Slot_6F225260() { Release(); }

    CAgent* m_value;
};

AGENT4_THUNK(SAgent4Slot_6F225260::SAgent4Slot_6F225260(CAgent*), 0x6F225260)
AGENT4_THUNK(SAgent4Slot_6F225260* SAgent4Slot_6F225260::Assign(CAgent*), 0x6F217A80)

//  0x6F1969F0 - 'AHav', MSB-first.  Out of scope for this agent
//  (see file header); redirected rather than reconstructed.
AGENT4_THUNK(unsigned int GetFixedRawcode_6F1969F0(), 0x6F1969F0)

//  What the maker hands back - only the CAgent* at +0x54 is reached, the
//  same shape every other member of this family reads through
//  (unitordermake.cpp, jassrelationagent.cpp, checkedslot_batch26.cpp, ...).
struct SMadeAgent_6F242F80
{
    char    m_reserved00[0x54];
    CAgent* m_pAgent;   // +0x54
};

CAgent* __fastcall MakeAgent_6F242F80()
{
    //  Constructed empty and assigned in one step - SAgent4Slot_6F225260's
    //  own converting constructor.
    SAgent4Slot_6F225260 held(0);

    unsigned int typeId = GetFixedRawcode_6F1969F0();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    //  The getter called a second time here rather than kept in a register -
    //  the shipped stream has two real calls to it, not one, the same shape
    //  MakeOrderAgent's own header describes for the location factory.
    InitWidgetAgentQuery(&query, GetFixedRawcode_6F1969F0(), type->m_allocator);
    //  Stored again after Init has already stored it - the shipped code has
    //  both stores, and dropping this one loses an instruction (same note
    //  as MakeOrderAgent's own).
    query.m_reserved24 = 0xFFFFFFFF;

    held.Assign(((SMadeAgent_6F242F80*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}

//----------------------------------------------------------------------------
//  0x6F243600 - MakeAgent_6F243600: an 'ANbf'-tagged agile-type
//  factory, the same template as MakeOrderAgent (0x6F294A40,
//  unitordermake.cpp).  Own translation unit: the callers of this whole
//  family call each instance for real.
//----------------------------------------------------------------------------
struct SAgent4Slot_6F225560
{
    //  0x6F225560 - out of scope for this agent, see file header.
    SAgent4Slot_6F225560(CAgent* candidate);

    //  0x6F217E80 - out of scope for this agent, see file header.
    SAgent4Slot_6F225560* Assign(CAgent* candidate);

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

    ~SAgent4Slot_6F225560() { Release(); }

    CAgent* m_value;
};

AGENT4_THUNK(SAgent4Slot_6F225560::SAgent4Slot_6F225560(CAgent*), 0x6F225560)
AGENT4_THUNK(SAgent4Slot_6F225560* SAgent4Slot_6F225560::Assign(CAgent*), 0x6F217E80)

//  0x6F198320 - 'ANbf', MSB-first.  Out of scope for this agent
//  (see file header); redirected rather than reconstructed.
AGENT4_THUNK(unsigned int GetFixedRawcode_6F198320(), 0x6F198320)

//  What the maker hands back - only the CAgent* at +0x54 is reached, the
//  same shape every other member of this family reads through
//  (unitordermake.cpp, jassrelationagent.cpp, checkedslot_batch26.cpp, ...).
struct SMadeAgent_6F243600
{
    char    m_reserved00[0x54];
    CAgent* m_pAgent;   // +0x54
};

CAgent* __fastcall MakeAgent_6F243600()
{
    //  Constructed empty and assigned in one step - SAgent4Slot_6F225560's
    //  own converting constructor.
    SAgent4Slot_6F225560 held(0);

    unsigned int typeId = GetFixedRawcode_6F198320();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    //  The getter called a second time here rather than kept in a register -
    //  the shipped stream has two real calls to it, not one, the same shape
    //  MakeOrderAgent's own header describes for the location factory.
    InitWidgetAgentQuery(&query, GetFixedRawcode_6F198320(), type->m_allocator);
    //  Stored again after Init has already stored it - the shipped code has
    //  both stores, and dropping this one loses an instruction (same note
    //  as MakeOrderAgent's own).
    query.m_reserved24 = 0xFFFFFFFF;

    held.Assign(((SMadeAgent_6F243600*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}

//----------------------------------------------------------------------------
//  0x6F243C80 - MakeAgent_6F243C80: an 'ANdc'-tagged agile-type
//  factory, the same template as MakeOrderAgent (0x6F294A40,
//  unitordermake.cpp).  Own translation unit: the callers of this whole
//  family call each instance for real.
//----------------------------------------------------------------------------
struct SAgent4Slot_6F225800
{
    //  0x6F225800 - out of scope for this agent, see file header.
    SAgent4Slot_6F225800(CAgent* candidate);

    //  0x6F218200 - out of scope for this agent, see file header.
    SAgent4Slot_6F225800* Assign(CAgent* candidate);

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

    ~SAgent4Slot_6F225800() { Release(); }

    CAgent* m_value;
};

AGENT4_THUNK(SAgent4Slot_6F225800::SAgent4Slot_6F225800(CAgent*), 0x6F225800)
AGENT4_THUNK(SAgent4Slot_6F225800* SAgent4Slot_6F225800::Assign(CAgent*), 0x6F218200)

//  0x6F1D6280 - 'ANdc', MSB-first.  Out of scope for this agent
//  (see file header); redirected rather than reconstructed.
AGENT4_THUNK(unsigned int GetFixedRawcode_6F1D6280(), 0x6F1D6280)

//  What the maker hands back - only the CAgent* at +0x54 is reached, the
//  same shape every other member of this family reads through
//  (unitordermake.cpp, jassrelationagent.cpp, checkedslot_batch26.cpp, ...).
struct SMadeAgent_6F243C80
{
    char    m_reserved00[0x54];
    CAgent* m_pAgent;   // +0x54
};

CAgent* __fastcall MakeAgent_6F243C80()
{
    //  Constructed empty and assigned in one step - SAgent4Slot_6F225800's
    //  own converting constructor.
    SAgent4Slot_6F225800 held(0);

    unsigned int typeId = GetFixedRawcode_6F1D6280();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    //  The getter called a second time here rather than kept in a register -
    //  the shipped stream has two real calls to it, not one, the same shape
    //  MakeOrderAgent's own header describes for the location factory.
    InitWidgetAgentQuery(&query, GetFixedRawcode_6F1D6280(), type->m_allocator);
    //  Stored again after Init has already stored it - the shipped code has
    //  both stores, and dropping this one loses an instruction (same note
    //  as MakeOrderAgent's own).
    query.m_reserved24 = 0xFFFFFFFF;

    held.Assign(((SMadeAgent_6F243C80*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}

//----------------------------------------------------------------------------
//  0x6F244300 - MakeAgent_6F244300: an 'AHds'-tagged agile-type
//  factory, the same template as MakeOrderAgent (0x6F294A40,
//  unitordermake.cpp).  Own translation unit: the callers of this whole
//  family call each instance for real.
//----------------------------------------------------------------------------
struct SAgent4Slot_6F225B00
{
    //  0x6F225B00 - out of scope for this agent, see file header.
    SAgent4Slot_6F225B00(CAgent* candidate);

    //  0x6F218600 - out of scope for this agent, see file header.
    SAgent4Slot_6F225B00* Assign(CAgent* candidate);

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

    ~SAgent4Slot_6F225B00() { Release(); }

    CAgent* m_value;
};

AGENT4_THUNK(SAgent4Slot_6F225B00::SAgent4Slot_6F225B00(CAgent*), 0x6F225B00)
AGENT4_THUNK(SAgent4Slot_6F225B00* SAgent4Slot_6F225B00::Assign(CAgent*), 0x6F218600)

//  0x6F07D130 - 'AHds', MSB-first.  Out of scope for this agent
//  (see file header); redirected rather than reconstructed.
AGENT4_THUNK(unsigned int GetFixedRawcode_6F07D130(), 0x6F07D130)

//  What the maker hands back - only the CAgent* at +0x54 is reached, the
//  same shape every other member of this family reads through
//  (unitordermake.cpp, jassrelationagent.cpp, checkedslot_batch26.cpp, ...).
struct SMadeAgent_6F244300
{
    char    m_reserved00[0x54];
    CAgent* m_pAgent;   // +0x54
};

CAgent* __fastcall MakeAgent_6F244300()
{
    //  Constructed empty and assigned in one step - SAgent4Slot_6F225B00's
    //  own converting constructor.
    SAgent4Slot_6F225B00 held(0);

    unsigned int typeId = GetFixedRawcode_6F07D130();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    //  The getter called a second time here rather than kept in a register -
    //  the shipped stream has two real calls to it, not one, the same shape
    //  MakeOrderAgent's own header describes for the location factory.
    InitWidgetAgentQuery(&query, GetFixedRawcode_6F07D130(), type->m_allocator);
    //  Stored again after Init has already stored it - the shipped code has
    //  both stores, and dropping this one loses an instruction (same note
    //  as MakeOrderAgent's own).
    query.m_reserved24 = 0xFFFFFFFF;

    held.Assign(((SMadeAgent_6F244300*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}

//----------------------------------------------------------------------------
//  0x6F244980 - MakeAgent_6F244980: an 'AEev'-tagged agile-type
//  factory, the same template as MakeOrderAgent (0x6F294A40,
//  unitordermake.cpp).  Own translation unit: the callers of this whole
//  family call each instance for real.
//----------------------------------------------------------------------------
struct SAgent4Slot_6F225E00
{
    //  0x6F225E00 - out of scope for this agent, see file header.
    SAgent4Slot_6F225E00(CAgent* candidate);

    //  0x6F218A00 - out of scope for this agent, see file header.
    SAgent4Slot_6F225E00* Assign(CAgent* candidate);

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

    ~SAgent4Slot_6F225E00() { Release(); }

    CAgent* m_value;
};

AGENT4_THUNK(SAgent4Slot_6F225E00::SAgent4Slot_6F225E00(CAgent*), 0x6F225E00)
AGENT4_THUNK(SAgent4Slot_6F225E00* SAgent4Slot_6F225E00::Assign(CAgent*), 0x6F218A00)

//  0x6F07F700 - 'AEev', MSB-first.  Out of scope for this agent
//  (see file header); redirected rather than reconstructed.
AGENT4_THUNK(unsigned int GetFixedRawcode_6F07F700(), 0x6F07F700)

//  What the maker hands back - only the CAgent* at +0x54 is reached, the
//  same shape every other member of this family reads through
//  (unitordermake.cpp, jassrelationagent.cpp, checkedslot_batch26.cpp, ...).
struct SMadeAgent_6F244980
{
    char    m_reserved00[0x54];
    CAgent* m_pAgent;   // +0x54
};

CAgent* __fastcall MakeAgent_6F244980()
{
    //  Constructed empty and assigned in one step - SAgent4Slot_6F225E00's
    //  own converting constructor.
    SAgent4Slot_6F225E00 held(0);

    unsigned int typeId = GetFixedRawcode_6F07F700();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    //  The getter called a second time here rather than kept in a register -
    //  the shipped stream has two real calls to it, not one, the same shape
    //  MakeOrderAgent's own header describes for the location factory.
    InitWidgetAgentQuery(&query, GetFixedRawcode_6F07F700(), type->m_allocator);
    //  Stored again after Init has already stored it - the shipped code has
    //  both stores, and dropping this one loses an instruction (same note
    //  as MakeOrderAgent's own).
    query.m_reserved24 = 0xFFFFFFFF;

    held.Assign(((SMadeAgent_6F244980*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}

//----------------------------------------------------------------------------
//  0x6F245000 - MakeAgent_6F245000: an 'ANfl'-tagged agile-type
//  factory, the same template as MakeOrderAgent (0x6F294A40,
//  unitordermake.cpp).  Own translation unit: the callers of this whole
//  family call each instance for real.
//----------------------------------------------------------------------------
struct SAgent4Slot_6F226100
{
    //  0x6F226100 - out of scope for this agent, see file header.
    SAgent4Slot_6F226100(CAgent* candidate);

    //  0x6F218E00 - out of scope for this agent, see file header.
    SAgent4Slot_6F226100* Assign(CAgent* candidate);

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

    ~SAgent4Slot_6F226100() { Release(); }

    CAgent* m_value;
};

AGENT4_THUNK(SAgent4Slot_6F226100::SAgent4Slot_6F226100(CAgent*), 0x6F226100)
AGENT4_THUNK(SAgent4Slot_6F226100* SAgent4Slot_6F226100::Assign(CAgent*), 0x6F218E00)

//  0x6F081D20 - 'ANfl', MSB-first.  Out of scope for this agent
//  (see file header); redirected rather than reconstructed.
AGENT4_THUNK(unsigned int GetFixedRawcode_6F081D20(), 0x6F081D20)

//  What the maker hands back - only the CAgent* at +0x54 is reached, the
//  same shape every other member of this family reads through
//  (unitordermake.cpp, jassrelationagent.cpp, checkedslot_batch26.cpp, ...).
struct SMadeAgent_6F245000
{
    char    m_reserved00[0x54];
    CAgent* m_pAgent;   // +0x54
};

CAgent* __fastcall MakeAgent_6F245000()
{
    //  Constructed empty and assigned in one step - SAgent4Slot_6F226100's
    //  own converting constructor.
    SAgent4Slot_6F226100 held(0);

    unsigned int typeId = GetFixedRawcode_6F081D20();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    //  The getter called a second time here rather than kept in a register -
    //  the shipped stream has two real calls to it, not one, the same shape
    //  MakeOrderAgent's own header describes for the location factory.
    InitWidgetAgentQuery(&query, GetFixedRawcode_6F081D20(), type->m_allocator);
    //  Stored again after Init has already stored it - the shipped code has
    //  both stores, and dropping this one loses an instruction (same note
    //  as MakeOrderAgent's own).
    query.m_reserved24 = 0xFFFFFFFF;

    held.Assign(((SMadeAgent_6F245000*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}

//----------------------------------------------------------------------------
//  0x6F245680 - MakeAgent_6F245680: an 'AEim'-tagged agile-type
//  factory, the same template as MakeOrderAgent (0x6F294A40,
//  unitordermake.cpp).  Own translation unit: the callers of this whole
//  family call each instance for real.
//----------------------------------------------------------------------------
struct SAgent4Slot_6F226400
{
    //  0x6F226400 - out of scope for this agent, see file header.
    SAgent4Slot_6F226400(CAgent* candidate);

    //  0x6F219200 - out of scope for this agent, see file header.
    SAgent4Slot_6F226400* Assign(CAgent* candidate);

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

    ~SAgent4Slot_6F226400() { Release(); }

    CAgent* m_value;
};

AGENT4_THUNK(SAgent4Slot_6F226400::SAgent4Slot_6F226400(CAgent*), 0x6F226400)
AGENT4_THUNK(SAgent4Slot_6F226400* SAgent4Slot_6F226400::Assign(CAgent*), 0x6F219200)

//  0x6F10E6A0 - 'AEim', MSB-first.  Out of scope for this agent
//  (see file header); redirected rather than reconstructed.
AGENT4_THUNK(unsigned int GetFixedRawcode_6F10E6A0(), 0x6F10E6A0)

//  What the maker hands back - only the CAgent* at +0x54 is reached, the
//  same shape every other member of this family reads through
//  (unitordermake.cpp, jassrelationagent.cpp, checkedslot_batch26.cpp, ...).
struct SMadeAgent_6F245680
{
    char    m_reserved00[0x54];
    CAgent* m_pAgent;   // +0x54
};

CAgent* __fastcall MakeAgent_6F245680()
{
    //  Constructed empty and assigned in one step - SAgent4Slot_6F226400's
    //  own converting constructor.
    SAgent4Slot_6F226400 held(0);

    unsigned int typeId = GetFixedRawcode_6F10E6A0();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    //  The getter called a second time here rather than kept in a register -
    //  the shipped stream has two real calls to it, not one, the same shape
    //  MakeOrderAgent's own header describes for the location factory.
    InitWidgetAgentQuery(&query, GetFixedRawcode_6F10E6A0(), type->m_allocator);
    //  Stored again after Init has already stored it - the shipped code has
    //  both stores, and dropping this one loses an instruction (same note
    //  as MakeOrderAgent's own).
    query.m_reserved24 = 0xFFFFFFFF;

    held.Assign(((SMadeAgent_6F245680*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}

//----------------------------------------------------------------------------
//  0x6F245DD0 - MakeAgent_6F245DD0: an 'AEme'-tagged agile-type
//  factory, the same template as MakeOrderAgent (0x6F294A40,
//  unitordermake.cpp).  Own translation unit: the callers of this whole
//  family call each instance for real.
//----------------------------------------------------------------------------
struct SAgent4Slot_6F226760
{
    //  0x6F226760 - out of scope for this agent, see file header.
    SAgent4Slot_6F226760(CAgent* candidate);

    //  0x6F219680 - out of scope for this agent, see file header.
    SAgent4Slot_6F226760* Assign(CAgent* candidate);

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

    ~SAgent4Slot_6F226760() { Release(); }

    CAgent* m_value;
};

AGENT4_THUNK(SAgent4Slot_6F226760::SAgent4Slot_6F226760(CAgent*), 0x6F226760)
AGENT4_THUNK(SAgent4Slot_6F226760* SAgent4Slot_6F226760::Assign(CAgent*), 0x6F219680)

//  0x6F111CA0 - 'AEme', MSB-first.  Out of scope for this agent
//  (see file header); redirected rather than reconstructed.
AGENT4_THUNK(unsigned int GetFixedRawcode_6F111CA0(), 0x6F111CA0)

//  What the maker hands back - only the CAgent* at +0x54 is reached, the
//  same shape every other member of this family reads through
//  (unitordermake.cpp, jassrelationagent.cpp, checkedslot_batch26.cpp, ...).
struct SMadeAgent_6F245DD0
{
    char    m_reserved00[0x54];
    CAgent* m_pAgent;   // +0x54
};

CAgent* __fastcall MakeAgent_6F245DD0()
{
    //  Constructed empty and assigned in one step - SAgent4Slot_6F226760's
    //  own converting constructor.
    SAgent4Slot_6F226760 held(0);

    unsigned int typeId = GetFixedRawcode_6F111CA0();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    //  The getter called a second time here rather than kept in a register -
    //  the shipped stream has two real calls to it, not one, the same shape
    //  MakeOrderAgent's own header describes for the location factory.
    InitWidgetAgentQuery(&query, GetFixedRawcode_6F111CA0(), type->m_allocator);
    //  Stored again after Init has already stored it - the shipped code has
    //  both stores, and dropping this one loses an instruction (same note
    //  as MakeOrderAgent's own).
    query.m_reserved24 = 0xFFFFFFFF;

    held.Assign(((SMadeAgent_6F245DD0*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}
