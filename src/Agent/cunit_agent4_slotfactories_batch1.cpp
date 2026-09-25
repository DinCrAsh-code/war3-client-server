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
//  0x6F238270 - MakeAgent_6F238270: an 'Aams'-tagged agile-type
//  factory, the same template as MakeOrderAgent (0x6F294A40,
//  unitordermake.cpp).  Own translation unit: the callers of this whole
//  family call each instance for real.
//----------------------------------------------------------------------------
struct SAgent4Slot_6F220B20
{
    //  0x6F220B20 - out of scope for this agent, see file header.
    SAgent4Slot_6F220B20(CAgent* candidate);

    //  0x6F211B80 - out of scope for this agent, see file header.
    SAgent4Slot_6F220B20* Assign(CAgent* candidate);

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

    ~SAgent4Slot_6F220B20() { Release(); }

    CAgent* m_value;
};

AGENT4_THUNK(SAgent4Slot_6F220B20::SAgent4Slot_6F220B20(CAgent*), 0x6F220B20)
AGENT4_THUNK(SAgent4Slot_6F220B20* SAgent4Slot_6F220B20::Assign(CAgent*), 0x6F211B80)

//  0x6F110630 - 'Aams', MSB-first.  Out of scope for this agent
//  (see file header); redirected rather than reconstructed.
AGENT4_THUNK(unsigned int GetFixedRawcode_6F110630(), 0x6F110630)

//  What the maker hands back - only the CAgent* at +0x54 is reached, the
//  same shape every other member of this family reads through
//  (unitordermake.cpp, jassrelationagent.cpp, checkedslot_batch26.cpp, ...).
struct SMadeAgent_6F238270
{
    char    m_reserved00[0x54];
    CAgent* m_pAgent;   // +0x54
};

CAgent* __fastcall MakeAgent_6F238270()
{
    //  Constructed empty and assigned in one step - SAgent4Slot_6F220B20's
    //  own converting constructor.
    SAgent4Slot_6F220B20 held(0);

    unsigned int typeId = GetFixedRawcode_6F110630();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    //  The getter called a second time here rather than kept in a register -
    //  the shipped stream has two real calls to it, not one, the same shape
    //  MakeOrderAgent's own header describes for the location factory.
    InitWidgetAgentQuery(&query, GetFixedRawcode_6F110630(), type->m_allocator);
    //  Stored again after Init has already stored it - the shipped code has
    //  both stores, and dropping this one loses an instruction (same note
    //  as MakeOrderAgent's own).
    query.m_reserved24 = 0xFFFFFFFF;

    held.Assign(((SMadeAgent_6F238270*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}

//----------------------------------------------------------------------------
//  0x6F2389C0 - MakeAgent_6F2389C0: an 'Aave'-tagged agile-type
//  factory, the same template as MakeOrderAgent (0x6F294A40,
//  unitordermake.cpp).  Own translation unit: the callers of this whole
//  family call each instance for real.
//----------------------------------------------------------------------------
struct SAgent4Slot_6F220E80
{
    //  0x6F220E80 - out of scope for this agent, see file header.
    SAgent4Slot_6F220E80(CAgent* candidate);

    //  0x6F212000 - out of scope for this agent, see file header.
    SAgent4Slot_6F220E80* Assign(CAgent* candidate);

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

    ~SAgent4Slot_6F220E80() { Release(); }

    CAgent* m_value;
};

AGENT4_THUNK(SAgent4Slot_6F220E80::SAgent4Slot_6F220E80(CAgent*), 0x6F220E80)
AGENT4_THUNK(SAgent4Slot_6F220E80* SAgent4Slot_6F220E80::Assign(CAgent*), 0x6F212000)

//  0x6F1D2E80 - 'Aave', MSB-first.  Out of scope for this agent
//  (see file header); redirected rather than reconstructed.
AGENT4_THUNK(unsigned int GetFixedRawcode_6F1D2E80(), 0x6F1D2E80)

//  What the maker hands back - only the CAgent* at +0x54 is reached, the
//  same shape every other member of this family reads through
//  (unitordermake.cpp, jassrelationagent.cpp, checkedslot_batch26.cpp, ...).
struct SMadeAgent_6F2389C0
{
    char    m_reserved00[0x54];
    CAgent* m_pAgent;   // +0x54
};

CAgent* __fastcall MakeAgent_6F2389C0()
{
    //  Constructed empty and assigned in one step - SAgent4Slot_6F220E80's
    //  own converting constructor.
    SAgent4Slot_6F220E80 held(0);

    unsigned int typeId = GetFixedRawcode_6F1D2E80();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    //  The getter called a second time here rather than kept in a register -
    //  the shipped stream has two real calls to it, not one, the same shape
    //  MakeOrderAgent's own header describes for the location factory.
    InitWidgetAgentQuery(&query, GetFixedRawcode_6F1D2E80(), type->m_allocator);
    //  Stored again after Init has already stored it - the shipped code has
    //  both stores, and dropping this one loses an instruction (same note
    //  as MakeOrderAgent's own).
    query.m_reserved24 = 0xFFFFFFFF;

    held.Assign(((SMadeAgent_6F2389C0*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}

//----------------------------------------------------------------------------
//  0x6F239110 - MakeAgent_6F239110: an 'Ablp'-tagged agile-type
//  factory, the same template as MakeOrderAgent (0x6F294A40,
//  unitordermake.cpp).  Own translation unit: the callers of this whole
//  family call each instance for real.
//----------------------------------------------------------------------------
struct SAgent4Slot_6F2211E0
{
    //  0x6F2211E0 - out of scope for this agent, see file header.
    SAgent4Slot_6F2211E0(CAgent* candidate);

    //  0x6F212480 - out of scope for this agent, see file header.
    SAgent4Slot_6F2211E0* Assign(CAgent* candidate);

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

    ~SAgent4Slot_6F2211E0() { Release(); }

    CAgent* m_value;
};

AGENT4_THUNK(SAgent4Slot_6F2211E0::SAgent4Slot_6F2211E0(CAgent*), 0x6F2211E0)
AGENT4_THUNK(SAgent4Slot_6F2211E0* SAgent4Slot_6F2211E0::Assign(CAgent*), 0x6F212480)

//  0x6F023E60 - 'Ablp', MSB-first.  Out of scope for this agent
//  (see file header); redirected rather than reconstructed.
AGENT4_THUNK(unsigned int GetFixedRawcode_6F023E60(), 0x6F023E60)

//  What the maker hands back - only the CAgent* at +0x54 is reached, the
//  same shape every other member of this family reads through
//  (unitordermake.cpp, jassrelationagent.cpp, checkedslot_batch26.cpp, ...).
struct SMadeAgent_6F239110
{
    char    m_reserved00[0x54];
    CAgent* m_pAgent;   // +0x54
};

CAgent* __fastcall MakeAgent_6F239110()
{
    //  Constructed empty and assigned in one step - SAgent4Slot_6F2211E0's
    //  own converting constructor.
    SAgent4Slot_6F2211E0 held(0);

    unsigned int typeId = GetFixedRawcode_6F023E60();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    //  The getter called a second time here rather than kept in a register -
    //  the shipped stream has two real calls to it, not one, the same shape
    //  MakeOrderAgent's own header describes for the location factory.
    InitWidgetAgentQuery(&query, GetFixedRawcode_6F023E60(), type->m_allocator);
    //  Stored again after Init has already stored it - the shipped code has
    //  both stores, and dropping this one loses an instruction (same note
    //  as MakeOrderAgent's own).
    query.m_reserved24 = 0xFFFFFFFF;

    held.Assign(((SMadeAgent_6F239110*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}

//----------------------------------------------------------------------------
//  0x6F239790 - MakeAgent_6F239790: an 'Adri'-tagged agile-type
//  factory, the same template as MakeOrderAgent (0x6F294A40,
//  unitordermake.cpp).  Own translation unit: the callers of this whole
//  family call each instance for real.
//----------------------------------------------------------------------------
struct SAgent4Slot_6F0926A0
{
    //  0x6F0926A0 - out of scope for this agent, see file header.
    SAgent4Slot_6F0926A0(CAgent* candidate);

    //  0x6F084A70 - out of scope for this agent, see file header.
    SAgent4Slot_6F0926A0* Assign(CAgent* candidate);

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

    ~SAgent4Slot_6F0926A0() { Release(); }

    CAgent* m_value;
};

AGENT4_THUNK(SAgent4Slot_6F0926A0::SAgent4Slot_6F0926A0(CAgent*), 0x6F0926A0)
AGENT4_THUNK(SAgent4Slot_6F0926A0* SAgent4Slot_6F0926A0::Assign(CAgent*), 0x6F084A70)

//  0x6F15BAA0 - 'Adri', MSB-first.  Out of scope for this agent
//  (see file header); redirected rather than reconstructed.
AGENT4_THUNK(unsigned int GetFixedRawcode_6F15BAA0(), 0x6F15BAA0)

//  What the maker hands back - only the CAgent* at +0x54 is reached, the
//  same shape every other member of this family reads through
//  (unitordermake.cpp, jassrelationagent.cpp, checkedslot_batch26.cpp, ...).
struct SMadeAgent_6F239790
{
    char    m_reserved00[0x54];
    CAgent* m_pAgent;   // +0x54
};

CAgent* __fastcall MakeAgent_6F239790()
{
    //  Constructed empty and assigned in one step - SAgent4Slot_6F0926A0's
    //  own converting constructor.
    SAgent4Slot_6F0926A0 held(0);

    unsigned int typeId = GetFixedRawcode_6F15BAA0();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    //  The getter called a second time here rather than kept in a register -
    //  the shipped stream has two real calls to it, not one, the same shape
    //  MakeOrderAgent's own header describes for the location factory.
    InitWidgetAgentQuery(&query, GetFixedRawcode_6F15BAA0(), type->m_allocator);
    //  Stored again after Init has already stored it - the shipped code has
    //  both stores, and dropping this one loses an instruction (same note
    //  as MakeOrderAgent's own).
    query.m_reserved24 = 0xFFFFFFFF;

    held.Assign(((SMadeAgent_6F239790*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}

//----------------------------------------------------------------------------
//  0x6F239EE0 - MakeAgent_6F239EE0: an 'Acpf'-tagged agile-type
//  factory, the same template as MakeOrderAgent (0x6F294A40,
//  unitordermake.cpp).  Own translation unit: the callers of this whole
//  family call each instance for real.
//----------------------------------------------------------------------------
struct SAgent4Slot_6F2216C0
{
    //  0x6F2216C0 - out of scope for this agent, see file header.
    SAgent4Slot_6F2216C0(CAgent* candidate);

    //  0x6F212B00 - out of scope for this agent, see file header.
    SAgent4Slot_6F2216C0* Assign(CAgent* candidate);

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

    ~SAgent4Slot_6F2216C0() { Release(); }

    CAgent* m_value;
};

AGENT4_THUNK(SAgent4Slot_6F2216C0::SAgent4Slot_6F2216C0(CAgent*), 0x6F2216C0)
AGENT4_THUNK(SAgent4Slot_6F2216C0* SAgent4Slot_6F2216C0::Assign(CAgent*), 0x6F212B00)

//  0x6F07F4B0 - 'Acpf', MSB-first.  Out of scope for this agent
//  (see file header); redirected rather than reconstructed.
AGENT4_THUNK(unsigned int GetFixedRawcode_6F07F4B0(), 0x6F07F4B0)

//  What the maker hands back - only the CAgent* at +0x54 is reached, the
//  same shape every other member of this family reads through
//  (unitordermake.cpp, jassrelationagent.cpp, checkedslot_batch26.cpp, ...).
struct SMadeAgent_6F239EE0
{
    char    m_reserved00[0x54];
    CAgent* m_pAgent;   // +0x54
};

CAgent* __fastcall MakeAgent_6F239EE0()
{
    //  Constructed empty and assigned in one step - SAgent4Slot_6F2216C0's
    //  own converting constructor.
    SAgent4Slot_6F2216C0 held(0);

    unsigned int typeId = GetFixedRawcode_6F07F4B0();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    //  The getter called a second time here rather than kept in a register -
    //  the shipped stream has two real calls to it, not one, the same shape
    //  MakeOrderAgent's own header describes for the location factory.
    InitWidgetAgentQuery(&query, GetFixedRawcode_6F07F4B0(), type->m_allocator);
    //  Stored again after Init has already stored it - the shipped code has
    //  both stores, and dropping this one loses an instruction (same note
    //  as MakeOrderAgent's own).
    query.m_reserved24 = 0xFFFFFFFF;

    held.Assign(((SMadeAgent_6F239EE0*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}

//----------------------------------------------------------------------------
//  0x6F23A560 - MakeAgent_6F23A560: an 'ACtb'-tagged agile-type
//  factory, the same template as MakeOrderAgent (0x6F294A40,
//  unitordermake.cpp).  Own translation unit: the callers of this whole
//  family call each instance for real.
//----------------------------------------------------------------------------
struct SAgent4Slot_6F221900
{
    //  0x6F221900 - out of scope for this agent, see file header.
    SAgent4Slot_6F221900(CAgent* candidate);

    //  0x6F212E00 - out of scope for this agent, see file header.
    SAgent4Slot_6F221900* Assign(CAgent* candidate);

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

    ~SAgent4Slot_6F221900() { Release(); }

    CAgent* m_value;
};

AGENT4_THUNK(SAgent4Slot_6F221900::SAgent4Slot_6F221900(CAgent*), 0x6F221900)
AGENT4_THUNK(SAgent4Slot_6F221900* SAgent4Slot_6F221900::Assign(CAgent*), 0x6F212E00)

//  0x6F156CC0 - 'ACtb', MSB-first.  Out of scope for this agent
//  (see file header); redirected rather than reconstructed.
AGENT4_THUNK(unsigned int GetFixedRawcode_6F156CC0(), 0x6F156CC0)

//  What the maker hands back - only the CAgent* at +0x54 is reached, the
//  same shape every other member of this family reads through
//  (unitordermake.cpp, jassrelationagent.cpp, checkedslot_batch26.cpp, ...).
struct SMadeAgent_6F23A560
{
    char    m_reserved00[0x54];
    CAgent* m_pAgent;   // +0x54
};

CAgent* __fastcall MakeAgent_6F23A560()
{
    //  Constructed empty and assigned in one step - SAgent4Slot_6F221900's
    //  own converting constructor.
    SAgent4Slot_6F221900 held(0);

    unsigned int typeId = GetFixedRawcode_6F156CC0();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    //  The getter called a second time here rather than kept in a register -
    //  the shipped stream has two real calls to it, not one, the same shape
    //  MakeOrderAgent's own header describes for the location factory.
    InitWidgetAgentQuery(&query, GetFixedRawcode_6F156CC0(), type->m_allocator);
    //  Stored again after Init has already stored it - the shipped code has
    //  both stores, and dropping this one loses an instruction (same note
    //  as MakeOrderAgent's own).
    query.m_reserved24 = 0xFFFFFFFF;

    held.Assign(((SMadeAgent_6F23A560*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}

//----------------------------------------------------------------------------
//  0x6F23AE50 - MakeAgent_6F23AE50: an 'Adta'-tagged agile-type
//  factory, the same template as MakeOrderAgent (0x6F294A40,
//  unitordermake.cpp).  Own translation unit: the callers of this whole
//  family call each instance for real.
//----------------------------------------------------------------------------
struct SAgent4Slot_6F221CC0
{
    //  0x6F221CC0 - out of scope for this agent, see file header.
    SAgent4Slot_6F221CC0(CAgent* candidate);

    //  0x6F213300 - out of scope for this agent, see file header.
    SAgent4Slot_6F221CC0* Assign(CAgent* candidate);

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

    ~SAgent4Slot_6F221CC0() { Release(); }

    CAgent* m_value;
};

AGENT4_THUNK(SAgent4Slot_6F221CC0::SAgent4Slot_6F221CC0(CAgent*), 0x6F221CC0)
AGENT4_THUNK(SAgent4Slot_6F221CC0* SAgent4Slot_6F221CC0::Assign(CAgent*), 0x6F213300)

//  0x6F024B10 - 'Adta', MSB-first.  Out of scope for this agent
//  (see file header); redirected rather than reconstructed.
AGENT4_THUNK(unsigned int GetFixedRawcode_6F024B10(), 0x6F024B10)

//  What the maker hands back - only the CAgent* at +0x54 is reached, the
//  same shape every other member of this family reads through
//  (unitordermake.cpp, jassrelationagent.cpp, checkedslot_batch26.cpp, ...).
struct SMadeAgent_6F23AE50
{
    char    m_reserved00[0x54];
    CAgent* m_pAgent;   // +0x54
};

CAgent* __fastcall MakeAgent_6F23AE50()
{
    //  Constructed empty and assigned in one step - SAgent4Slot_6F221CC0's
    //  own converting constructor.
    SAgent4Slot_6F221CC0 held(0);

    unsigned int typeId = GetFixedRawcode_6F024B10();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    //  The getter called a second time here rather than kept in a register -
    //  the shipped stream has two real calls to it, not one, the same shape
    //  MakeOrderAgent's own header describes for the location factory.
    InitWidgetAgentQuery(&query, GetFixedRawcode_6F024B10(), type->m_allocator);
    //  Stored again after Init has already stored it - the shipped code has
    //  both stores, and dropping this one loses an instruction (same note
    //  as MakeOrderAgent's own).
    query.m_reserved24 = 0xFFFFFFFF;

    held.Assign(((SMadeAgent_6F23AE50*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}

//----------------------------------------------------------------------------
//  0x6F23B4D0 - MakeAgent_6F23B4D0: an 'Aeat'-tagged agile-type
//  factory, the same template as MakeOrderAgent (0x6F294A40,
//  unitordermake.cpp).  Own translation unit: the callers of this whole
//  family call each instance for real.
//----------------------------------------------------------------------------
struct SAgent4Slot_6F221F60
{
    //  0x6F221F60 - out of scope for this agent, see file header.
    SAgent4Slot_6F221F60(CAgent* candidate);

    //  0x6F213680 - out of scope for this agent, see file header.
    SAgent4Slot_6F221F60* Assign(CAgent* candidate);

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

    ~SAgent4Slot_6F221F60() { Release(); }

    CAgent* m_value;
};

AGENT4_THUNK(SAgent4Slot_6F221F60::SAgent4Slot_6F221F60(CAgent*), 0x6F221F60)
AGENT4_THUNK(SAgent4Slot_6F221F60* SAgent4Slot_6F221F60::Assign(CAgent*), 0x6F213680)

//  0x6F07E2A0 - 'Aeat', MSB-first.  Out of scope for this agent
//  (see file header); redirected rather than reconstructed.
AGENT4_THUNK(unsigned int GetFixedRawcode_6F07E2A0(), 0x6F07E2A0)

//  What the maker hands back - only the CAgent* at +0x54 is reached, the
//  same shape every other member of this family reads through
//  (unitordermake.cpp, jassrelationagent.cpp, checkedslot_batch26.cpp, ...).
struct SMadeAgent_6F23B4D0
{
    char    m_reserved00[0x54];
    CAgent* m_pAgent;   // +0x54
};

CAgent* __fastcall MakeAgent_6F23B4D0()
{
    //  Constructed empty and assigned in one step - SAgent4Slot_6F221F60's
    //  own converting constructor.
    SAgent4Slot_6F221F60 held(0);

    unsigned int typeId = GetFixedRawcode_6F07E2A0();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    //  The getter called a second time here rather than kept in a register -
    //  the shipped stream has two real calls to it, not one, the same shape
    //  MakeOrderAgent's own header describes for the location factory.
    InitWidgetAgentQuery(&query, GetFixedRawcode_6F07E2A0(), type->m_allocator);
    //  Stored again after Init has already stored it - the shipped code has
    //  both stores, and dropping this one loses an instruction (same note
    //  as MakeOrderAgent's own).
    query.m_reserved24 = 0xFFFFFFFF;

    held.Assign(((SMadeAgent_6F23B4D0*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}

//----------------------------------------------------------------------------
//  0x6F23BB50 - MakeAgent_6F23BB50: an 'Aexh'-tagged agile-type
//  factory, the same template as MakeOrderAgent (0x6F294A40,
//  unitordermake.cpp).  Own translation unit: the callers of this whole
//  family call each instance for real.
//----------------------------------------------------------------------------
struct SAgent4Slot_6F222260
{
    //  0x6F222260 - out of scope for this agent, see file header.
    SAgent4Slot_6F222260(CAgent* candidate);

    //  0x6F213A80 - out of scope for this agent, see file header.
    SAgent4Slot_6F222260* Assign(CAgent* candidate);

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

    ~SAgent4Slot_6F222260() { Release(); }

    CAgent* m_value;
};

AGENT4_THUNK(SAgent4Slot_6F222260::SAgent4Slot_6F222260(CAgent*), 0x6F222260)
AGENT4_THUNK(SAgent4Slot_6F222260* SAgent4Slot_6F222260::Assign(CAgent*), 0x6F213A80)

//  0x6F07FA90 - 'Aexh', MSB-first.  Out of scope for this agent
//  (see file header); redirected rather than reconstructed.
AGENT4_THUNK(unsigned int GetFixedRawcode_6F07FA90(), 0x6F07FA90)

//  What the maker hands back - only the CAgent* at +0x54 is reached, the
//  same shape every other member of this family reads through
//  (unitordermake.cpp, jassrelationagent.cpp, checkedslot_batch26.cpp, ...).
struct SMadeAgent_6F23BB50
{
    char    m_reserved00[0x54];
    CAgent* m_pAgent;   // +0x54
};

CAgent* __fastcall MakeAgent_6F23BB50()
{
    //  Constructed empty and assigned in one step - SAgent4Slot_6F222260's
    //  own converting constructor.
    SAgent4Slot_6F222260 held(0);

    unsigned int typeId = GetFixedRawcode_6F07FA90();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    //  The getter called a second time here rather than kept in a register -
    //  the shipped stream has two real calls to it, not one, the same shape
    //  MakeOrderAgent's own header describes for the location factory.
    InitWidgetAgentQuery(&query, GetFixedRawcode_6F07FA90(), type->m_allocator);
    //  Stored again after Init has already stored it - the shipped code has
    //  both stores, and dropping this one loses an instruction (same note
    //  as MakeOrderAgent's own).
    query.m_reserved24 = 0xFFFFFFFF;

    held.Assign(((SMadeAgent_6F23BB50*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}

//----------------------------------------------------------------------------
//  0x6F23E3F0 - MakeAgent_6F23E3F0: an 'ANre'-tagged agile-type
//  factory, the same template as MakeOrderAgent (0x6F294A40,
//  unitordermake.cpp).  Own translation unit: the callers of this whole
//  family call each instance for real.
//----------------------------------------------------------------------------
struct SAgent4Slot_6F223280
{
    //  0x6F223280 - out of scope for this agent, see file header.
    SAgent4Slot_6F223280(CAgent* candidate);

    //  0x6F215000 - out of scope for this agent, see file header.
    SAgent4Slot_6F223280* Assign(CAgent* candidate);

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

    ~SAgent4Slot_6F223280() { Release(); }

    CAgent* m_value;
};

AGENT4_THUNK(SAgent4Slot_6F223280::SAgent4Slot_6F223280(CAgent*), 0x6F223280)
AGENT4_THUNK(SAgent4Slot_6F223280* SAgent4Slot_6F223280::Assign(CAgent*), 0x6F215000)

//  0x6F023110 - 'ANre', MSB-first.  Out of scope for this agent
//  (see file header); redirected rather than reconstructed.
AGENT4_THUNK(unsigned int GetFixedRawcode_6F023110(), 0x6F023110)

//  What the maker hands back - only the CAgent* at +0x54 is reached, the
//  same shape every other member of this family reads through
//  (unitordermake.cpp, jassrelationagent.cpp, checkedslot_batch26.cpp, ...).
struct SMadeAgent_6F23E3F0
{
    char    m_reserved00[0x54];
    CAgent* m_pAgent;   // +0x54
};

CAgent* __fastcall MakeAgent_6F23E3F0()
{
    //  Constructed empty and assigned in one step - SAgent4Slot_6F223280's
    //  own converting constructor.
    SAgent4Slot_6F223280 held(0);

    unsigned int typeId = GetFixedRawcode_6F023110();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    //  The getter called a second time here rather than kept in a register -
    //  the shipped stream has two real calls to it, not one, the same shape
    //  MakeOrderAgent's own header describes for the location factory.
    InitWidgetAgentQuery(&query, GetFixedRawcode_6F023110(), type->m_allocator);
    //  Stored again after Init has already stored it - the shipped code has
    //  both stores, and dropping this one loses an instruction (same note
    //  as MakeOrderAgent's own).
    query.m_reserved24 = 0xFFFFFFFF;

    held.Assign(((SMadeAgent_6F23E3F0*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}
