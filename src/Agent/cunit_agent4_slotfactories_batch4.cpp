//============================================================================
//  cunit_agent4 batch: 8 instances of the agile-type-agent factory
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
//  0x6F24A890 - MakeAgent_6F24A890: an 'AIva'-tagged agile-type
//  factory, the same template as MakeOrderAgent (0x6F294A40,
//  unitordermake.cpp).  Own translation unit: the callers of this whole
//  family call each instance for real.
//----------------------------------------------------------------------------
struct SAgent4Slot_6F228920
{
    //  0x6F228920 - out of scope for this agent, see file header.
    SAgent4Slot_6F228920(CAgent* candidate);

    //  0x6F21C380 - out of scope for this agent, see file header.
    SAgent4Slot_6F228920* Assign(CAgent* candidate);

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

    ~SAgent4Slot_6F228920() { Release(); }

    CAgent* m_value;
};

AGENT4_THUNK(SAgent4Slot_6F228920::SAgent4Slot_6F228920(CAgent*), 0x6F228920)
AGENT4_THUNK(SAgent4Slot_6F228920* SAgent4Slot_6F228920::Assign(CAgent*), 0x6F21C380)

//  0x6F0DD8A0 - 'AIva', MSB-first.  Out of scope for this agent
//  (see file header); redirected rather than reconstructed.
AGENT4_THUNK(unsigned int GetFixedRawcode_6F0DD8A0(), 0x6F0DD8A0)

//  What the maker hands back - only the CAgent* at +0x54 is reached, the
//  same shape every other member of this family reads through
//  (unitordermake.cpp, jassrelationagent.cpp, checkedslot_batch26.cpp, ...).
struct SMadeAgent_6F24A890
{
    char    m_reserved00[0x54];
    CAgent* m_pAgent;   // +0x54
};

CAgent* __fastcall MakeAgent_6F24A890()
{
    //  Constructed empty and assigned in one step - SAgent4Slot_6F228920's
    //  own converting constructor.
    SAgent4Slot_6F228920 held(0);

    unsigned int typeId = GetFixedRawcode_6F0DD8A0();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    //  The getter called a second time here rather than kept in a register -
    //  the shipped stream has two real calls to it, not one, the same shape
    //  MakeOrderAgent's own header describes for the location factory.
    InitWidgetAgentQuery(&query, GetFixedRawcode_6F0DD8A0(), type->m_allocator);
    //  Stored again after Init has already stored it - the shipped code has
    //  both stores, and dropping this one loses an instruction (same note
    //  as MakeOrderAgent's own).
    query.m_reserved24 = 0xFFFFFFFF;

    held.Assign(((SMadeAgent_6F24A890*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}

//----------------------------------------------------------------------------
//  0x6F24AF10 - MakeAgent_6F24AF10: an 'AIhe'-tagged agile-type
//  factory, the same template as MakeOrderAgent (0x6F294A40,
//  unitordermake.cpp).  Own translation unit: the callers of this whole
//  family call each instance for real.
//----------------------------------------------------------------------------
struct SAgent4Slot_6F228C20
{
    //  0x6F228C20 - out of scope for this agent, see file header.
    SAgent4Slot_6F228C20(CAgent* candidate);

    //  0x6F21C780 - out of scope for this agent, see file header.
    SAgent4Slot_6F228C20* Assign(CAgent* candidate);

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

    ~SAgent4Slot_6F228C20() { Release(); }

    CAgent* m_value;
};

AGENT4_THUNK(SAgent4Slot_6F228C20::SAgent4Slot_6F228C20(CAgent*), 0x6F228C20)
AGENT4_THUNK(SAgent4Slot_6F228C20* SAgent4Slot_6F228C20::Assign(CAgent*), 0x6F21C780)

//  0x6F0DAA00 - 'AIhe', MSB-first.  Out of scope for this agent
//  (see file header); redirected rather than reconstructed.
AGENT4_THUNK(unsigned int GetFixedRawcode_6F0DAA00(), 0x6F0DAA00)

//  What the maker hands back - only the CAgent* at +0x54 is reached, the
//  same shape every other member of this family reads through
//  (unitordermake.cpp, jassrelationagent.cpp, checkedslot_batch26.cpp, ...).
struct SMadeAgent_6F24AF10
{
    char    m_reserved00[0x54];
    CAgent* m_pAgent;   // +0x54
};

CAgent* __fastcall MakeAgent_6F24AF10()
{
    //  Constructed empty and assigned in one step - SAgent4Slot_6F228C20's
    //  own converting constructor.
    SAgent4Slot_6F228C20 held(0);

    unsigned int typeId = GetFixedRawcode_6F0DAA00();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    //  The getter called a second time here rather than kept in a register -
    //  the shipped stream has two real calls to it, not one, the same shape
    //  MakeOrderAgent's own header describes for the location factory.
    InitWidgetAgentQuery(&query, GetFixedRawcode_6F0DAA00(), type->m_allocator);
    //  Stored again after Init has already stored it - the shipped code has
    //  both stores, and dropping this one loses an instruction (same note
    //  as MakeOrderAgent's own).
    query.m_reserved24 = 0xFFFFFFFF;

    held.Assign(((SMadeAgent_6F24AF10*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}

//----------------------------------------------------------------------------
//  0x6F24B590 - MakeAgent_6F24B590: an 'AIrm'-tagged agile-type
//  factory, the same template as MakeOrderAgent (0x6F294A40,
//  unitordermake.cpp).  Own translation unit: the callers of this whole
//  family call each instance for real.
//----------------------------------------------------------------------------
struct SAgent4Slot_6F228F20
{
    //  0x6F228F20 - out of scope for this agent, see file header.
    SAgent4Slot_6F228F20(CAgent* candidate);

    //  0x6F21CB80 - out of scope for this agent, see file header.
    SAgent4Slot_6F228F20* Assign(CAgent* candidate);

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

    ~SAgent4Slot_6F228F20() { Release(); }

    CAgent* m_value;
};

AGENT4_THUNK(SAgent4Slot_6F228F20::SAgent4Slot_6F228F20(CAgent*), 0x6F228F20)
AGENT4_THUNK(SAgent4Slot_6F228F20* SAgent4Slot_6F228F20::Assign(CAgent*), 0x6F21CB80)

//  0x6F0DD020 - 'AIrm', MSB-first.  Out of scope for this agent
//  (see file header); redirected rather than reconstructed.
AGENT4_THUNK(unsigned int GetFixedRawcode_6F0DD020(), 0x6F0DD020)

//  What the maker hands back - only the CAgent* at +0x54 is reached, the
//  same shape every other member of this family reads through
//  (unitordermake.cpp, jassrelationagent.cpp, checkedslot_batch26.cpp, ...).
struct SMadeAgent_6F24B590
{
    char    m_reserved00[0x54];
    CAgent* m_pAgent;   // +0x54
};

CAgent* __fastcall MakeAgent_6F24B590()
{
    //  Constructed empty and assigned in one step - SAgent4Slot_6F228F20's
    //  own converting constructor.
    SAgent4Slot_6F228F20 held(0);

    unsigned int typeId = GetFixedRawcode_6F0DD020();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    //  The getter called a second time here rather than kept in a register -
    //  the shipped stream has two real calls to it, not one, the same shape
    //  MakeOrderAgent's own header describes for the location factory.
    InitWidgetAgentQuery(&query, GetFixedRawcode_6F0DD020(), type->m_allocator);
    //  Stored again after Init has already stored it - the shipped code has
    //  both stores, and dropping this one loses an instruction (same note
    //  as MakeOrderAgent's own).
    query.m_reserved24 = 0xFFFFFFFF;

    held.Assign(((SMadeAgent_6F24B590*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}

//----------------------------------------------------------------------------
//  0x6F24BC10 - MakeAgent_6F24BC10: an 'AItp'-tagged agile-type
//  factory, the same template as MakeOrderAgent (0x6F294A40,
//  unitordermake.cpp).  Own translation unit: the callers of this whole
//  family call each instance for real.
//----------------------------------------------------------------------------
struct SAgent4Slot_6F229220
{
    //  0x6F229220 - out of scope for this agent, see file header.
    SAgent4Slot_6F229220(CAgent* candidate);

    //  0x6F21CF80 - out of scope for this agent, see file header.
    SAgent4Slot_6F229220* Assign(CAgent* candidate);

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

    ~SAgent4Slot_6F229220() { Release(); }

    CAgent* m_value;
};

AGENT4_THUNK(SAgent4Slot_6F229220::SAgent4Slot_6F229220(CAgent*), 0x6F229220)
AGENT4_THUNK(SAgent4Slot_6F229220* SAgent4Slot_6F229220::Assign(CAgent*), 0x6F21CF80)

//  0x6F026050 - 'AItp', MSB-first.  Out of scope for this agent
//  (see file header); redirected rather than reconstructed.
AGENT4_THUNK(unsigned int GetFixedRawcode_6F026050(), 0x6F026050)

//  What the maker hands back - only the CAgent* at +0x54 is reached, the
//  same shape every other member of this family reads through
//  (unitordermake.cpp, jassrelationagent.cpp, checkedslot_batch26.cpp, ...).
struct SMadeAgent_6F24BC10
{
    char    m_reserved00[0x54];
    CAgent* m_pAgent;   // +0x54
};

CAgent* __fastcall MakeAgent_6F24BC10()
{
    //  Constructed empty and assigned in one step - SAgent4Slot_6F229220's
    //  own converting constructor.
    SAgent4Slot_6F229220 held(0);

    unsigned int typeId = GetFixedRawcode_6F026050();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    //  The getter called a second time here rather than kept in a register -
    //  the shipped stream has two real calls to it, not one, the same shape
    //  MakeOrderAgent's own header describes for the location factory.
    InitWidgetAgentQuery(&query, GetFixedRawcode_6F026050(), type->m_allocator);
    //  Stored again after Init has already stored it - the shipped code has
    //  both stores, and dropping this one loses an instruction (same note
    //  as MakeOrderAgent's own).
    query.m_reserved24 = 0xFFFFFFFF;

    held.Assign(((SMadeAgent_6F24BC10*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}

//----------------------------------------------------------------------------
//  0x6F24C290 - MakeAgent_6F24C290: an 'AIlu'-tagged agile-type
//  factory, the same template as MakeOrderAgent (0x6F294A40,
//  unitordermake.cpp).  Own translation unit: the callers of this whole
//  family call each instance for real.
//----------------------------------------------------------------------------
struct SAgent4Slot_6F2294C0
{
    //  0x6F2294C0 - out of scope for this agent, see file header.
    SAgent4Slot_6F2294C0(CAgent* candidate);

    //  0x6F21D300 - out of scope for this agent, see file header.
    SAgent4Slot_6F2294C0* Assign(CAgent* candidate);

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

    ~SAgent4Slot_6F2294C0() { Release(); }

    CAgent* m_value;
};

AGENT4_THUNK(SAgent4Slot_6F2294C0::SAgent4Slot_6F2294C0(CAgent*), 0x6F2294C0)
AGENT4_THUNK(SAgent4Slot_6F2294C0* SAgent4Slot_6F2294C0::Assign(CAgent*), 0x6F21D300)

//  0x6F0DD420 - 'AIlu', MSB-first.  Out of scope for this agent
//  (see file header); redirected rather than reconstructed.
AGENT4_THUNK(unsigned int GetFixedRawcode_6F0DD420(), 0x6F0DD420)

//  What the maker hands back - only the CAgent* at +0x54 is reached, the
//  same shape every other member of this family reads through
//  (unitordermake.cpp, jassrelationagent.cpp, checkedslot_batch26.cpp, ...).
struct SMadeAgent_6F24C290
{
    char    m_reserved00[0x54];
    CAgent* m_pAgent;   // +0x54
};

CAgent* __fastcall MakeAgent_6F24C290()
{
    //  Constructed empty and assigned in one step - SAgent4Slot_6F2294C0's
    //  own converting constructor.
    SAgent4Slot_6F2294C0 held(0);

    unsigned int typeId = GetFixedRawcode_6F0DD420();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    //  The getter called a second time here rather than kept in a register -
    //  the shipped stream has two real calls to it, not one, the same shape
    //  MakeOrderAgent's own header describes for the location factory.
    InitWidgetAgentQuery(&query, GetFixedRawcode_6F0DD420(), type->m_allocator);
    //  Stored again after Init has already stored it - the shipped code has
    //  both stores, and dropping this one loses an instruction (same note
    //  as MakeOrderAgent's own).
    query.m_reserved24 = 0xFFFFFFFF;

    held.Assign(((SMadeAgent_6F24C290*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}

//----------------------------------------------------------------------------
//  0x6F24C910 - MakeAgent_6F24C910: an 'AIct'-tagged agile-type
//  factory, the same template as MakeOrderAgent (0x6F294A40,
//  unitordermake.cpp).  Own translation unit: the callers of this whole
//  family call each instance for real.
//----------------------------------------------------------------------------
struct SAgent4Slot_6F2297C0
{
    //  0x6F2297C0 - out of scope for this agent, see file header.
    SAgent4Slot_6F2297C0(CAgent* candidate);

    //  0x6F21D700 - out of scope for this agent, see file header.
    SAgent4Slot_6F2297C0* Assign(CAgent* candidate);

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

    ~SAgent4Slot_6F2297C0() { Release(); }

    CAgent* m_value;
};

AGENT4_THUNK(SAgent4Slot_6F2297C0::SAgent4Slot_6F2297C0(CAgent*), 0x6F2297C0)
AGENT4_THUNK(SAgent4Slot_6F2297C0* SAgent4Slot_6F2297C0::Assign(CAgent*), 0x6F21D700)

//  0x6F0DD920 - 'AIct', MSB-first.  Out of scope for this agent
//  (see file header); redirected rather than reconstructed.
AGENT4_THUNK(unsigned int GetFixedRawcode_6F0DD920(), 0x6F0DD920)

//  What the maker hands back - only the CAgent* at +0x54 is reached, the
//  same shape every other member of this family reads through
//  (unitordermake.cpp, jassrelationagent.cpp, checkedslot_batch26.cpp, ...).
struct SMadeAgent_6F24C910
{
    char    m_reserved00[0x54];
    CAgent* m_pAgent;   // +0x54
};

CAgent* __fastcall MakeAgent_6F24C910()
{
    //  Constructed empty and assigned in one step - SAgent4Slot_6F2297C0's
    //  own converting constructor.
    SAgent4Slot_6F2297C0 held(0);

    unsigned int typeId = GetFixedRawcode_6F0DD920();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    //  The getter called a second time here rather than kept in a register -
    //  the shipped stream has two real calls to it, not one, the same shape
    //  MakeOrderAgent's own header describes for the location factory.
    InitWidgetAgentQuery(&query, GetFixedRawcode_6F0DD920(), type->m_allocator);
    //  Stored again after Init has already stored it - the shipped code has
    //  both stores, and dropping this one loses an instruction (same note
    //  as MakeOrderAgent's own).
    query.m_reserved24 = 0xFFFFFFFF;

    held.Assign(((SMadeAgent_6F24C910*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}

//----------------------------------------------------------------------------
//  0x6F29AA00 - MakeAgent_6F29AA00: an 'AHbu'-tagged agile-type
//  factory, the same template as MakeOrderAgent (0x6F294A40,
//  unitordermake.cpp).  Own translation unit: the callers of this whole
//  family call each instance for real.
//----------------------------------------------------------------------------
struct SAgent4Slot_6F280D20
{
    //  0x6F280D20 - out of scope for this agent, see file header.
    SAgent4Slot_6F280D20(CAgent* candidate);

    //  0x6F2754B0 - out of scope for this agent, see file header.
    SAgent4Slot_6F280D20* Assign(CAgent* candidate);

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

    ~SAgent4Slot_6F280D20() { Release(); }

    CAgent* m_value;
};

AGENT4_THUNK(SAgent4Slot_6F280D20::SAgent4Slot_6F280D20(CAgent*), 0x6F280D20)
AGENT4_THUNK(SAgent4Slot_6F280D20* SAgent4Slot_6F280D20::Assign(CAgent*), 0x6F2754B0)

//  0x6F0D9020 - 'AHbu', MSB-first.  Out of scope for this agent
//  (see file header); redirected rather than reconstructed.
AGENT4_THUNK(unsigned int GetFixedRawcode_6F0D9020(), 0x6F0D9020)

//  What the maker hands back - only the CAgent* at +0x54 is reached, the
//  same shape every other member of this family reads through
//  (unitordermake.cpp, jassrelationagent.cpp, checkedslot_batch26.cpp, ...).
struct SMadeAgent_6F29AA00
{
    char    m_reserved00[0x54];
    CAgent* m_pAgent;   // +0x54
};

CAgent* __fastcall MakeAgent_6F29AA00()
{
    //  Constructed empty and assigned in one step - SAgent4Slot_6F280D20's
    //  own converting constructor.
    SAgent4Slot_6F280D20 held(0);

    unsigned int typeId = GetFixedRawcode_6F0D9020();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    //  The getter called a second time here rather than kept in a register -
    //  the shipped stream has two real calls to it, not one, the same shape
    //  MakeOrderAgent's own header describes for the location factory.
    InitWidgetAgentQuery(&query, GetFixedRawcode_6F0D9020(), type->m_allocator);
    //  Stored again after Init has already stored it - the shipped code has
    //  both stores, and dropping this one loses an instruction (same note
    //  as MakeOrderAgent's own).
    query.m_reserved24 = 0xFFFFFFFF;

    held.Assign(((SMadeAgent_6F29AA00*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}

//----------------------------------------------------------------------------
//  0x6F29B080 - MakeAgent_6F29B080: an 'Asei'-tagged agile-type
//  factory, the same template as MakeOrderAgent (0x6F294A40,
//  unitordermake.cpp).  Own translation unit: the callers of this whole
//  family call each instance for real.
//----------------------------------------------------------------------------
struct SAgent4Slot_6F281020
{
    //  0x6F281020 - out of scope for this agent, see file header.
    SAgent4Slot_6F281020(CAgent* candidate);

    //  0x6F2758B0 - out of scope for this agent, see file header.
    SAgent4Slot_6F281020* Assign(CAgent* candidate);

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

    ~SAgent4Slot_6F281020() { Release(); }

    CAgent* m_value;
};

AGENT4_THUNK(SAgent4Slot_6F281020::SAgent4Slot_6F281020(CAgent*), 0x6F281020)
AGENT4_THUNK(SAgent4Slot_6F281020* SAgent4Slot_6F281020::Assign(CAgent*), 0x6F2758B0)

//  0x6F028840 - 'Asei', MSB-first.  Out of scope for this agent
//  (see file header); redirected rather than reconstructed.
AGENT4_THUNK(unsigned int GetFixedRawcode_6F028840(), 0x6F028840)

//  What the maker hands back - only the CAgent* at +0x54 is reached, the
//  same shape every other member of this family reads through
//  (unitordermake.cpp, jassrelationagent.cpp, checkedslot_batch26.cpp, ...).
struct SMadeAgent_6F29B080
{
    char    m_reserved00[0x54];
    CAgent* m_pAgent;   // +0x54
};

CAgent* __fastcall MakeAgent_6F29B080()
{
    //  Constructed empty and assigned in one step - SAgent4Slot_6F281020's
    //  own converting constructor.
    SAgent4Slot_6F281020 held(0);

    unsigned int typeId = GetFixedRawcode_6F028840();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    //  The getter called a second time here rather than kept in a register -
    //  the shipped stream has two real calls to it, not one, the same shape
    //  MakeOrderAgent's own header describes for the location factory.
    InitWidgetAgentQuery(&query, GetFixedRawcode_6F028840(), type->m_allocator);
    //  Stored again after Init has already stored it - the shipped code has
    //  both stores, and dropping this one loses an instruction (same note
    //  as MakeOrderAgent's own).
    query.m_reserved24 = 0xFFFFFFFF;

    held.Assign(((SMadeAgent_6F29B080*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}
