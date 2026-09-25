//============================================================================
//  Thirty-eight members of the "resolve one fixed AGILE_TYPE agent and hand
//  back a borrowed pointer" family, reached from CUnit's own vtable-closure
//  walk (BFS depth 1-2, cunit_agent5_worklist.md).  Every one is the same
//  shape as CAgentRegistrySingleton::FinishSingleton's own tail
//  (agentregistrysingleton.cpp) and SCheckedRectSlot::MakeRect
//  (jassrectmake.cpp), just with the "made" object held as a plain scoped
//  local rather than as `this` or a member:
//
//      SCheckedSlot_<ctor> made(0);
//      AGILE_TYPE_ID id(GetConstant_<getter>());   // the type-id leaf,
//                                                    // called *twice* -
//                                                    // once for the
//                                                    // registry lookup,
//                                                    // once to fill the
//                                                    // request block in -
//                                                    // the shipped code,
//                                                    // not a stylistic
//                                                    // choice (see
//                                                    // jassrectmake.cpp).
//      CGameData* game = g_pGameData;
//      unsigned int hash = id.Hash();
//      AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
//      SWidgetAgentQuery query;
//      InitWidgetAgentQuery(&query, GetConstant_<getter>(), type->m_allocator);
//      made.Assign(((SMadeAgent54*)SubmitWidgetAgentQuery(&query,1,1))->m_pAgent);
//      return made.m_value;
//
//  `query.m_reserved24` is never written here (unlike MakeRect's `kind`-
//  derived one) - these take no `kind` argument at all, so InitWidgetAgentQuery's
//  own default (-1, widgetagentquery.h) stands.
//
//  Each address's own constructor/getter/Assign is a naked `mov eax,<addr>
//  / jmp eax` thunk into the real, unhooked shipped body - none of the
//  114 addresses below has a dump anywhere in this batch's worklist (they
//  are the BFS *next* layer out, one hop past what cunit_agent5_worklist.md
//  actually carries text for), so a real reconstruction is future work, not
//  a guess made now.  Each thunk's own `retn` byte count is read straight
//  out of agent_worktrees/funcs/<addr>.json's own raw_asm, not inferred:
//  every getter ends in a bare `retn` (0 bytes, matching the __fastcall
//  0-argument declaration below) and every constructor/Assign ends
//  `retn 4` (one CAgent* stack argument, matching the `CAgent* candidate`
//  parameter both take).  Getting either wrong here would be exactly the
//  live-crash class thunk_abi_audit.py exists to catch once any of these
//  ever gets hooked - so the batch pays that lookup even though nothing
//  today calls these thunks from outside this file.
//
//  Behaviour verdict for every wrapper: DIFFERS.  The `made` local's
//  destructor needs an SEH frame this repo's fixed /EHs-c- build cannot
//  reproduce - the exact, already-documented gap
//  CAgentRegistrySingleton::FinishSingleton's own BEHAVIOUR entry describes
//  ("the unreproducible __except_handler4-shaped SEH frame ... and the
//  4-to-12-byte stack-slot displacement that frame's extra words put on
//  every [esp+N] after it"), not a new one - every real call, its real
//  arguments and their real order match.
//============================================================================
#include "agiletype.h"
#include "widgetagentquery.h"
#include "game.h"
#include "agent.h"

//  What SubmitWidgetAgentQuery's answer holds; only the CAgent* at +0x54
//  is ever reached - the same shape every other member of the checked-slot
//  family reads through (jassrectmake.cpp, agentregistrysingleton.cpp,
//  checkedslot_batch26.cpp, item_createatposition.cpp, ...).
namespace {
struct SMadeAgent54
{
    char    m_reserved00[0x54];
    CAgent* m_pAgent;   // +0x54
};
}


//----------------------------------------------------------------------------
//  0x6F238340.  ctor 0x6F220B80 (retn 4) / type-id leaf 0x6F1106B0 (retn 0) /
//  Assign 0x6F211C00 (retn 4) - all three naked thunks, no dump body for
//  any of them in this batch.
//----------------------------------------------------------------------------
namespace {
struct SCheckedSlot_0x6F220B80
{
    SCheckedSlot_0x6F220B80(CAgent* candidate);
    SCheckedSlot_0x6F220B80* Assign(CAgent* candidate);
    ~SCheckedSlot_0x6F220B80() { Release(); }
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
    CAgent* m_value;
};
}

__declspec(naked) SCheckedSlot_0x6F220B80::SCheckedSlot_0x6F220B80(CAgent*)
{
    __asm { mov eax, 06F220B80h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedSlot_0x6F220B80* SCheckedSlot_0x6F220B80::Assign(CAgent*)
{
    __asm { mov eax, 06F211C00h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int __fastcall GetConstant_0x6F1106B0()
{
    __asm { mov eax, 06F1106B0h }
    __asm { jmp eax }
}

CAgent* QueryAgentSingleton_0x6F238340()
{
    SCheckedSlot_0x6F220B80 made(0);

    AGILE_TYPE_ID id(GetConstant_0x6F1106B0());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetConstant_0x6F1106B0(), type->m_allocator);

    made.Assign(((SMadeAgent54*)SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);
    return made.m_value;
}


//----------------------------------------------------------------------------
//  0x6F238A90.  ctor 0x6F220EE0 (retn 4) / type-id leaf 0x6F0281F0 (retn 0) /
//  Assign 0x6F212080 (retn 4) - all three naked thunks, no dump body for
//  any of them in this batch.
//----------------------------------------------------------------------------
namespace {
struct SCheckedSlot_0x6F220EE0
{
    SCheckedSlot_0x6F220EE0(CAgent* candidate);
    SCheckedSlot_0x6F220EE0* Assign(CAgent* candidate);
    ~SCheckedSlot_0x6F220EE0() { Release(); }
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
    CAgent* m_value;
};
}

__declspec(naked) SCheckedSlot_0x6F220EE0::SCheckedSlot_0x6F220EE0(CAgent*)
{
    __asm { mov eax, 06F220EE0h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedSlot_0x6F220EE0* SCheckedSlot_0x6F220EE0::Assign(CAgent*)
{
    __asm { mov eax, 06F212080h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int __fastcall GetConstant_0x6F0281F0()
{
    __asm { mov eax, 06F0281F0h }
    __asm { jmp eax }
}

CAgent* QueryAgentSingleton_0x6F238A90()
{
    SCheckedSlot_0x6F220EE0 made(0);

    AGILE_TYPE_ID id(GetConstant_0x6F0281F0());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetConstant_0x6F0281F0(), type->m_allocator);

    made.Assign(((SMadeAgent54*)SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);
    return made.m_value;
}


//----------------------------------------------------------------------------
//  0x6F2391E0.  ctor 0x6F221240 (retn 4) / type-id leaf 0x6F197400 (retn 0) /
//  Assign 0x6F212500 (retn 4) - all three naked thunks, no dump body for
//  any of them in this batch.
//----------------------------------------------------------------------------
namespace {
struct SCheckedSlot_0x6F221240
{
    SCheckedSlot_0x6F221240(CAgent* candidate);
    SCheckedSlot_0x6F221240* Assign(CAgent* candidate);
    ~SCheckedSlot_0x6F221240() { Release(); }
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
    CAgent* m_value;
};
}

__declspec(naked) SCheckedSlot_0x6F221240::SCheckedSlot_0x6F221240(CAgent*)
{
    __asm { mov eax, 06F221240h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedSlot_0x6F221240* SCheckedSlot_0x6F221240::Assign(CAgent*)
{
    __asm { mov eax, 06F212500h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int __fastcall GetConstant_0x6F197400()
{
    __asm { mov eax, 06F197400h }
    __asm { jmp eax }
}

CAgent* QueryAgentSingleton_0x6F2391E0()
{
    SCheckedSlot_0x6F221240 made(0);

    AGILE_TYPE_ID id(GetConstant_0x6F197400());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetConstant_0x6F197400(), type->m_allocator);

    made.Assign(((SMadeAgent54*)SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);
    return made.m_value;
}


//----------------------------------------------------------------------------
//  0x6F239860.  ctor 0x6F221480 (retn 4) / type-id leaf 0x6F15BA20 (retn 0) /
//  Assign 0x6F212800 (retn 4) - all three naked thunks, no dump body for
//  any of them in this batch.
//----------------------------------------------------------------------------
namespace {
struct SCheckedSlot_0x6F221480
{
    SCheckedSlot_0x6F221480(CAgent* candidate);
    SCheckedSlot_0x6F221480* Assign(CAgent* candidate);
    ~SCheckedSlot_0x6F221480() { Release(); }
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
    CAgent* m_value;
};
}

__declspec(naked) SCheckedSlot_0x6F221480::SCheckedSlot_0x6F221480(CAgent*)
{
    __asm { mov eax, 06F221480h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedSlot_0x6F221480* SCheckedSlot_0x6F221480::Assign(CAgent*)
{
    __asm { mov eax, 06F212800h }
    __asm { jmp eax }
}

//  checksum-round2-G (2026-09-13): real body now, defined in its own TU
//  (getconstant_0x6f15ba20.cpp - see that file's own note for why it is
//  split out rather than defined in either caller) - declared `extern`
//  here rather than redefined, the one-shipped-address-one-C++-name rule
//  every other redirect in this repo follows.
extern unsigned int __fastcall GetConstant_0x6F15BA20();

CAgent* QueryAgentSingleton_0x6F239860()
{
    SCheckedSlot_0x6F221480 made(0);

    AGILE_TYPE_ID id(GetConstant_0x6F15BA20());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetConstant_0x6F15BA20(), type->m_allocator);

    made.Assign(((SMadeAgent54*)SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);
    return made.m_value;
}


//----------------------------------------------------------------------------
//  0x6F239FB0.  ctor 0x6F16A2B0 (retn 4) / type-id leaf 0x6F15B010 (retn 0) /
//  Assign 0x6F15DA50 (retn 4) - all three naked thunks, no dump body for
//  any of them in this batch.
//----------------------------------------------------------------------------
namespace {
struct SCheckedSlot_0x6F16A2B0
{
    SCheckedSlot_0x6F16A2B0(CAgent* candidate);
    SCheckedSlot_0x6F16A2B0* Assign(CAgent* candidate);
    ~SCheckedSlot_0x6F16A2B0() { Release(); }
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
    CAgent* m_value;
};
}

__declspec(naked) SCheckedSlot_0x6F16A2B0::SCheckedSlot_0x6F16A2B0(CAgent*)
{
    __asm { mov eax, 06F16A2B0h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedSlot_0x6F16A2B0* SCheckedSlot_0x6F16A2B0::Assign(CAgent*)
{
    __asm { mov eax, 06F15DA50h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int __fastcall GetConstant_0x6F15B010()
{
    __asm { mov eax, 06F15B010h }
    __asm { jmp eax }
}

CAgent* QueryAgentSingleton_0x6F239FB0()
{
    SCheckedSlot_0x6F16A2B0 made(0);

    AGILE_TYPE_ID id(GetConstant_0x6F15B010());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetConstant_0x6F15B010(), type->m_allocator);

    made.Assign(((SMadeAgent54*)SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);
    return made.m_value;
}


//----------------------------------------------------------------------------
//  0x6F23A630.  ctor 0x6F221960 (retn 4) / type-id leaf 0x6F157020 (retn 0) /
//  Assign 0x6F212E80 (retn 4) - all three naked thunks, no dump body for
//  any of them in this batch.
//----------------------------------------------------------------------------
namespace {
struct SCheckedSlot_0x6F221960
{
    SCheckedSlot_0x6F221960(CAgent* candidate);
    SCheckedSlot_0x6F221960* Assign(CAgent* candidate);
    ~SCheckedSlot_0x6F221960() { Release(); }
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
    CAgent* m_value;
};
}

__declspec(naked) SCheckedSlot_0x6F221960::SCheckedSlot_0x6F221960(CAgent*)
{
    __asm { mov eax, 06F221960h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedSlot_0x6F221960* SCheckedSlot_0x6F221960::Assign(CAgent*)
{
    __asm { mov eax, 06F212E80h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int __fastcall GetConstant_0x6F157020()
{
    __asm { mov eax, 06F157020h }
    __asm { jmp eax }
}

CAgent* QueryAgentSingleton_0x6F23A630()
{
    SCheckedSlot_0x6F221960 made(0);

    AGILE_TYPE_ID id(GetConstant_0x6F157020());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetConstant_0x6F157020(), type->m_allocator);

    made.Assign(((SMadeAgent54*)SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);
    return made.m_value;
}


//----------------------------------------------------------------------------
//  0x6F23AF20.  ctor 0x6F0D4C40 (retn 4) / type-id leaf 0x6F024890 (retn 0) /
//  Assign 0x6F0D2E50 (retn 4) - all three naked thunks, no dump body for
//  any of them in this batch.
//----------------------------------------------------------------------------
namespace {
struct SCheckedSlot_0x6F0D4C40
{
    SCheckedSlot_0x6F0D4C40(CAgent* candidate);
    SCheckedSlot_0x6F0D4C40* Assign(CAgent* candidate);
    ~SCheckedSlot_0x6F0D4C40() { Release(); }
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
    CAgent* m_value;
};
}

__declspec(naked) SCheckedSlot_0x6F0D4C40::SCheckedSlot_0x6F0D4C40(CAgent*)
{
    __asm { mov eax, 06F0D4C40h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedSlot_0x6F0D4C40* SCheckedSlot_0x6F0D4C40::Assign(CAgent*)
{
    __asm { mov eax, 06F0D2E50h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int __fastcall GetConstant_0x6F024890()
{
    __asm { mov eax, 06F024890h }
    __asm { jmp eax }
}

CAgent* QueryAgentSingleton_0x6F23AF20()
{
    SCheckedSlot_0x6F0D4C40 made(0);

    AGILE_TYPE_ID id(GetConstant_0x6F024890());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetConstant_0x6F024890(), type->m_allocator);

    made.Assign(((SMadeAgent54*)SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);
    return made.m_value;
}


//----------------------------------------------------------------------------
//  0x6F23B5A0.  ctor 0x6F221FC0 (retn 4) / type-id leaf 0x6F07EB60 (retn 0) /
//  Assign 0x6F213700 (retn 4) - all three naked thunks, no dump body for
//  any of them in this batch.
//----------------------------------------------------------------------------
namespace {
struct SCheckedSlot_0x6F221FC0
{
    SCheckedSlot_0x6F221FC0(CAgent* candidate);
    SCheckedSlot_0x6F221FC0* Assign(CAgent* candidate);
    ~SCheckedSlot_0x6F221FC0() { Release(); }
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
    CAgent* m_value;
};
}

__declspec(naked) SCheckedSlot_0x6F221FC0::SCheckedSlot_0x6F221FC0(CAgent*)
{
    __asm { mov eax, 06F221FC0h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedSlot_0x6F221FC0* SCheckedSlot_0x6F221FC0::Assign(CAgent*)
{
    __asm { mov eax, 06F213700h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int __fastcall GetConstant_0x6F07EB60()
{
    __asm { mov eax, 06F07EB60h }
    __asm { jmp eax }
}

CAgent* QueryAgentSingleton_0x6F23B5A0()
{
    SCheckedSlot_0x6F221FC0 made(0);

    AGILE_TYPE_ID id(GetConstant_0x6F07EB60());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetConstant_0x6F07EB60(), type->m_allocator);

    made.Assign(((SMadeAgent54*)SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);
    return made.m_value;
}


//----------------------------------------------------------------------------
//  0x6F23BC20.  ctor 0x6F2222C0 (retn 4) / type-id leaf 0x6F07FC10 (retn 0) /
//  Assign 0x6F213B00 (retn 4) - all three naked thunks, no dump body for
//  any of them in this batch.
//----------------------------------------------------------------------------
namespace {
struct SCheckedSlot_0x6F2222C0
{
    SCheckedSlot_0x6F2222C0(CAgent* candidate);
    SCheckedSlot_0x6F2222C0* Assign(CAgent* candidate);
    ~SCheckedSlot_0x6F2222C0() { Release(); }
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
    CAgent* m_value;
};
}

__declspec(naked) SCheckedSlot_0x6F2222C0::SCheckedSlot_0x6F2222C0(CAgent*)
{
    __asm { mov eax, 06F2222C0h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedSlot_0x6F2222C0* SCheckedSlot_0x6F2222C0::Assign(CAgent*)
{
    __asm { mov eax, 06F213B00h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int __fastcall GetConstant_0x6F07FC10()
{
    __asm { mov eax, 06F07FC10h }
    __asm { jmp eax }
}

CAgent* QueryAgentSingleton_0x6F23BC20()
{
    SCheckedSlot_0x6F2222C0 made(0);

    AGILE_TYPE_ID id(GetConstant_0x6F07FC10());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetConstant_0x6F07FC10(), type->m_allocator);

    made.Assign(((SMadeAgent54*)SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);
    return made.m_value;
}


//----------------------------------------------------------------------------
//  0x6F23E4C0.  ctor 0x6F2232E0 (retn 4) / type-id leaf 0x6F0268F0 (retn 0) /
//  Assign 0x6F215080 (retn 4) - all three naked thunks, no dump body for
//  any of them in this batch.
//----------------------------------------------------------------------------
namespace {
struct SCheckedSlot_0x6F2232E0
{
    SCheckedSlot_0x6F2232E0(CAgent* candidate);
    SCheckedSlot_0x6F2232E0* Assign(CAgent* candidate);
    ~SCheckedSlot_0x6F2232E0() { Release(); }
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
    CAgent* m_value;
};
}

__declspec(naked) SCheckedSlot_0x6F2232E0::SCheckedSlot_0x6F2232E0(CAgent*)
{
    __asm { mov eax, 06F2232E0h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedSlot_0x6F2232E0* SCheckedSlot_0x6F2232E0::Assign(CAgent*)
{
    __asm { mov eax, 06F215080h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int __fastcall GetConstant_0x6F0268F0()
{
    __asm { mov eax, 06F0268F0h }
    __asm { jmp eax }
}

CAgent* QueryAgentSingleton_0x6F23E4C0()
{
    SCheckedSlot_0x6F2232E0 made(0);

    AGILE_TYPE_ID id(GetConstant_0x6F0268F0());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetConstant_0x6F0268F0(), type->m_allocator);

    made.Assign(((SMadeAgent54*)SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);
    return made.m_value;
}


//----------------------------------------------------------------------------
//  0x6F240C90.  ctor 0x6F2243C0 (retn 4) / type-id leaf 0x6F1D6A80 (retn 0) /
//  Assign 0x6F216700 (retn 4) - all three naked thunks, no dump body for
//  any of them in this batch.
//----------------------------------------------------------------------------
namespace {
struct SCheckedSlot_0x6F2243C0
{
    SCheckedSlot_0x6F2243C0(CAgent* candidate);
    SCheckedSlot_0x6F2243C0* Assign(CAgent* candidate);
    ~SCheckedSlot_0x6F2243C0() { Release(); }
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
    CAgent* m_value;
};
}

__declspec(naked) SCheckedSlot_0x6F2243C0::SCheckedSlot_0x6F2243C0(CAgent*)
{
    __asm { mov eax, 06F2243C0h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedSlot_0x6F2243C0* SCheckedSlot_0x6F2243C0::Assign(CAgent*)
{
    __asm { mov eax, 06F216700h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int __fastcall GetConstant_0x6F1D6A80()
{
    __asm { mov eax, 06F1D6A80h }
    __asm { jmp eax }
}

CAgent* QueryAgentSingleton_0x6F240C90()
{
    SCheckedSlot_0x6F2243C0 made(0);

    AGILE_TYPE_ID id(GetConstant_0x6F1D6A80());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetConstant_0x6F1D6A80(), type->m_allocator);

    made.Assign(((SMadeAgent54*)SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);
    return made.m_value;
}


//----------------------------------------------------------------------------
//  0x6F2429D0.  ctor 0x6F224FC0 (retn 4) / type-id leaf 0x6F195C20 (retn 0) /
//  Assign 0x6F217700 (retn 4) - all three naked thunks, no dump body for
//  any of them in this batch.
//----------------------------------------------------------------------------
namespace {
struct SCheckedSlot_0x6F224FC0
{
    SCheckedSlot_0x6F224FC0(CAgent* candidate);
    SCheckedSlot_0x6F224FC0* Assign(CAgent* candidate);
    ~SCheckedSlot_0x6F224FC0() { Release(); }
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
    CAgent* m_value;
};
}

__declspec(naked) SCheckedSlot_0x6F224FC0::SCheckedSlot_0x6F224FC0(CAgent*)
{
    __asm { mov eax, 06F224FC0h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedSlot_0x6F224FC0* SCheckedSlot_0x6F224FC0::Assign(CAgent*)
{
    __asm { mov eax, 06F217700h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int __fastcall GetConstant_0x6F195C20()
{
    __asm { mov eax, 06F195C20h }
    __asm { jmp eax }
}

CAgent* QueryAgentSingleton_0x6F2429D0()
{
    SCheckedSlot_0x6F224FC0 made(0);

    AGILE_TYPE_ID id(GetConstant_0x6F195C20());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetConstant_0x6F195C20(), type->m_allocator);

    made.Assign(((SMadeAgent54*)SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);
    return made.m_value;
}


//----------------------------------------------------------------------------
//  0x6F243050.  ctor 0x6F2252C0 (retn 4) / type-id leaf 0x6F196BE0 (retn 0) /
//  Assign 0x6F217B00 (retn 4) - all three naked thunks, no dump body for
//  any of them in this batch.
//----------------------------------------------------------------------------
namespace {
struct SCheckedSlot_0x6F2252C0
{
    SCheckedSlot_0x6F2252C0(CAgent* candidate);
    SCheckedSlot_0x6F2252C0* Assign(CAgent* candidate);
    ~SCheckedSlot_0x6F2252C0() { Release(); }
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
    CAgent* m_value;
};
}

__declspec(naked) SCheckedSlot_0x6F2252C0::SCheckedSlot_0x6F2252C0(CAgent*)
{
    __asm { mov eax, 06F2252C0h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedSlot_0x6F2252C0* SCheckedSlot_0x6F2252C0::Assign(CAgent*)
{
    __asm { mov eax, 06F217B00h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int __fastcall GetConstant_0x6F196BE0()
{
    __asm { mov eax, 06F196BE0h }
    __asm { jmp eax }
}

CAgent* QueryAgentSingleton_0x6F243050()
{
    SCheckedSlot_0x6F2252C0 made(0);

    AGILE_TYPE_ID id(GetConstant_0x6F196BE0());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetConstant_0x6F196BE0(), type->m_allocator);

    made.Assign(((SMadeAgent54*)SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);
    return made.m_value;
}


//----------------------------------------------------------------------------
//  0x6F2436D0.  ctor 0x6F2255C0 (retn 4) / type-id leaf 0x6F0DE640 (retn 0) /
//  Assign 0x6F217F00 (retn 4) - all three naked thunks, no dump body for
//  any of them in this batch.
//----------------------------------------------------------------------------
namespace {
struct SCheckedSlot_0x6F2255C0
{
    SCheckedSlot_0x6F2255C0(CAgent* candidate);
    SCheckedSlot_0x6F2255C0* Assign(CAgent* candidate);
    ~SCheckedSlot_0x6F2255C0() { Release(); }
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
    CAgent* m_value;
};
}

__declspec(naked) SCheckedSlot_0x6F2255C0::SCheckedSlot_0x6F2255C0(CAgent*)
{
    __asm { mov eax, 06F2255C0h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedSlot_0x6F2255C0* SCheckedSlot_0x6F2255C0::Assign(CAgent*)
{
    __asm { mov eax, 06F217F00h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int __fastcall GetConstant_0x6F0DE640()
{
    __asm { mov eax, 06F0DE640h }
    __asm { jmp eax }
}

CAgent* QueryAgentSingleton_0x6F2436D0()
{
    SCheckedSlot_0x6F2255C0 made(0);

    AGILE_TYPE_ID id(GetConstant_0x6F0DE640());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetConstant_0x6F0DE640(), type->m_allocator);

    made.Assign(((SMadeAgent54*)SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);
    return made.m_value;
}


//----------------------------------------------------------------------------
//  0x6F243D50.  ctor 0x6F225860 (retn 4) / type-id leaf 0x6F1953E0 (retn 0) /
//  Assign 0x6F218280 (retn 4) - all three naked thunks, no dump body for
//  any of them in this batch.
//----------------------------------------------------------------------------
namespace {
struct SCheckedSlot_0x6F225860
{
    SCheckedSlot_0x6F225860(CAgent* candidate);
    SCheckedSlot_0x6F225860* Assign(CAgent* candidate);
    ~SCheckedSlot_0x6F225860() { Release(); }
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
    CAgent* m_value;
};
}

__declspec(naked) SCheckedSlot_0x6F225860::SCheckedSlot_0x6F225860(CAgent*)
{
    __asm { mov eax, 06F225860h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedSlot_0x6F225860* SCheckedSlot_0x6F225860::Assign(CAgent*)
{
    __asm { mov eax, 06F218280h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int __fastcall GetConstant_0x6F1953E0()
{
    __asm { mov eax, 06F1953E0h }
    __asm { jmp eax }
}

CAgent* QueryAgentSingleton_0x6F243D50()
{
    SCheckedSlot_0x6F225860 made(0);

    AGILE_TYPE_ID id(GetConstant_0x6F1953E0());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetConstant_0x6F1953E0(), type->m_allocator);

    made.Assign(((SMadeAgent54*)SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);
    return made.m_value;
}


//----------------------------------------------------------------------------
//  0x6F2443D0.  ctor 0x6F225B60 (retn 4) / type-id leaf 0x6F07D280 (retn 0) /
//  Assign 0x6F218680 (retn 4) - all three naked thunks, no dump body for
//  any of them in this batch.
//----------------------------------------------------------------------------
namespace {
struct SCheckedSlot_0x6F225B60
{
    SCheckedSlot_0x6F225B60(CAgent* candidate);
    SCheckedSlot_0x6F225B60* Assign(CAgent* candidate);
    ~SCheckedSlot_0x6F225B60() { Release(); }
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
    CAgent* m_value;
};
}

__declspec(naked) SCheckedSlot_0x6F225B60::SCheckedSlot_0x6F225B60(CAgent*)
{
    __asm { mov eax, 06F225B60h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedSlot_0x6F225B60* SCheckedSlot_0x6F225B60::Assign(CAgent*)
{
    __asm { mov eax, 06F218680h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int __fastcall GetConstant_0x6F07D280()
{
    __asm { mov eax, 06F07D280h }
    __asm { jmp eax }
}

CAgent* QueryAgentSingleton_0x6F2443D0()
{
    SCheckedSlot_0x6F225B60 made(0);

    AGILE_TYPE_ID id(GetConstant_0x6F07D280());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetConstant_0x6F07D280(), type->m_allocator);

    made.Assign(((SMadeAgent54*)SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);
    return made.m_value;
}


//----------------------------------------------------------------------------
//  0x6F244A50.  ctor 0x6F225E60 (retn 4) / type-id leaf 0x6F07FD90 (retn 0) /
//  Assign 0x6F218A80 (retn 4) - all three naked thunks, no dump body for
//  any of them in this batch.
//----------------------------------------------------------------------------
namespace {
struct SCheckedSlot_0x6F225E60
{
    SCheckedSlot_0x6F225E60(CAgent* candidate);
    SCheckedSlot_0x6F225E60* Assign(CAgent* candidate);
    ~SCheckedSlot_0x6F225E60() { Release(); }
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
    CAgent* m_value;
};
}

__declspec(naked) SCheckedSlot_0x6F225E60::SCheckedSlot_0x6F225E60(CAgent*)
{
    __asm { mov eax, 06F225E60h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedSlot_0x6F225E60* SCheckedSlot_0x6F225E60::Assign(CAgent*)
{
    __asm { mov eax, 06F218A80h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int __fastcall GetConstant_0x6F07FD90()
{
    __asm { mov eax, 06F07FD90h }
    __asm { jmp eax }
}

CAgent* QueryAgentSingleton_0x6F244A50()
{
    SCheckedSlot_0x6F225E60 made(0);

    AGILE_TYPE_ID id(GetConstant_0x6F07FD90());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetConstant_0x6F07FD90(), type->m_allocator);

    made.Assign(((SMadeAgent54*)SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);
    return made.m_value;
}


//----------------------------------------------------------------------------
//  0x6F2450D0.  ctor 0x6F226160 (retn 4) / type-id leaf 0x6F082100 (retn 0) /
//  Assign 0x6F218E80 (retn 4) - all three naked thunks, no dump body for
//  any of them in this batch.
//----------------------------------------------------------------------------
namespace {
struct SCheckedSlot_0x6F226160
{
    SCheckedSlot_0x6F226160(CAgent* candidate);
    SCheckedSlot_0x6F226160* Assign(CAgent* candidate);
    ~SCheckedSlot_0x6F226160() { Release(); }
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
    CAgent* m_value;
};
}

__declspec(naked) SCheckedSlot_0x6F226160::SCheckedSlot_0x6F226160(CAgent*)
{
    __asm { mov eax, 06F226160h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedSlot_0x6F226160* SCheckedSlot_0x6F226160::Assign(CAgent*)
{
    __asm { mov eax, 06F218E80h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int __fastcall GetConstant_0x6F082100()
{
    __asm { mov eax, 06F082100h }
    __asm { jmp eax }
}

CAgent* QueryAgentSingleton_0x6F2450D0()
{
    SCheckedSlot_0x6F226160 made(0);

    AGILE_TYPE_ID id(GetConstant_0x6F082100());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetConstant_0x6F082100(), type->m_allocator);

    made.Assign(((SMadeAgent54*)SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);
    return made.m_value;
}


//----------------------------------------------------------------------------
//  0x6F245750.  ctor 0x6F226460 (retn 4) / type-id leaf 0x6F10EB00 (retn 0) /
//  Assign 0x6F219280 (retn 4) - all three naked thunks, no dump body for
//  any of them in this batch.
//----------------------------------------------------------------------------
namespace {
struct SCheckedSlot_0x6F226460
{
    SCheckedSlot_0x6F226460(CAgent* candidate);
    SCheckedSlot_0x6F226460* Assign(CAgent* candidate);
    ~SCheckedSlot_0x6F226460() { Release(); }
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
    CAgent* m_value;
};
}

__declspec(naked) SCheckedSlot_0x6F226460::SCheckedSlot_0x6F226460(CAgent*)
{
    __asm { mov eax, 06F226460h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedSlot_0x6F226460* SCheckedSlot_0x6F226460::Assign(CAgent*)
{
    __asm { mov eax, 06F219280h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int __fastcall GetConstant_0x6F10EB00()
{
    __asm { mov eax, 06F10EB00h }
    __asm { jmp eax }
}

CAgent* QueryAgentSingleton_0x6F245750()
{
    SCheckedSlot_0x6F226460 made(0);

    AGILE_TYPE_ID id(GetConstant_0x6F10EB00());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetConstant_0x6F10EB00(), type->m_allocator);

    made.Assign(((SMadeAgent54*)SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);
    return made.m_value;
}


//----------------------------------------------------------------------------
//  0x6F245EA0.  ctor 0x6F2267C0 (retn 4) / type-id leaf 0x6F112700 (retn 0) /
//  Assign 0x6F219700 (retn 4) - all three naked thunks, no dump body for
//  any of them in this batch.
//----------------------------------------------------------------------------
namespace {
struct SCheckedSlot_0x6F2267C0
{
    SCheckedSlot_0x6F2267C0(CAgent* candidate);
    SCheckedSlot_0x6F2267C0* Assign(CAgent* candidate);
    ~SCheckedSlot_0x6F2267C0() { Release(); }
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
    CAgent* m_value;
};
}

__declspec(naked) SCheckedSlot_0x6F2267C0::SCheckedSlot_0x6F2267C0(CAgent*)
{
    __asm { mov eax, 06F2267C0h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedSlot_0x6F2267C0* SCheckedSlot_0x6F2267C0::Assign(CAgent*)
{
    __asm { mov eax, 06F219700h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int __fastcall GetConstant_0x6F112700()
{
    __asm { mov eax, 06F112700h }
    __asm { jmp eax }
}

CAgent* QueryAgentSingleton_0x6F245EA0()
{
    SCheckedSlot_0x6F2267C0 made(0);

    AGILE_TYPE_ID id(GetConstant_0x6F112700());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetConstant_0x6F112700(), type->m_allocator);

    made.Assign(((SMadeAgent54*)SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);
    return made.m_value;
}


//----------------------------------------------------------------------------
//  0x6F246520.  ctor 0x6F226AC0 (retn 4) / type-id leaf 0x6F1D4F00 (retn 0) /
//  Assign 0x6F219B00 (retn 4) - all three naked thunks, no dump body for
//  any of them in this batch.
//----------------------------------------------------------------------------
namespace {
struct SCheckedSlot_0x6F226AC0
{
    SCheckedSlot_0x6F226AC0(CAgent* candidate);
    SCheckedSlot_0x6F226AC0* Assign(CAgent* candidate);
    ~SCheckedSlot_0x6F226AC0() { Release(); }
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
    CAgent* m_value;
};
}

__declspec(naked) SCheckedSlot_0x6F226AC0::SCheckedSlot_0x6F226AC0(CAgent*)
{
    __asm { mov eax, 06F226AC0h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedSlot_0x6F226AC0* SCheckedSlot_0x6F226AC0::Assign(CAgent*)
{
    __asm { mov eax, 06F219B00h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int __fastcall GetConstant_0x6F1D4F00()
{
    __asm { mov eax, 06F1D4F00h }
    __asm { jmp eax }
}

CAgent* QueryAgentSingleton_0x6F246520()
{
    SCheckedSlot_0x6F226AC0 made(0);

    AGILE_TYPE_ID id(GetConstant_0x6F1D4F00());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetConstant_0x6F1D4F00(), type->m_allocator);

    made.Assign(((SMadeAgent54*)SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);
    return made.m_value;
}


//----------------------------------------------------------------------------
//  0x6F246BA0.  ctor 0x6F226DC0 (retn 4) / type-id leaf 0x6F1587E0 (retn 0) /
//  Assign 0x6F219F00 (retn 4) - all three naked thunks, no dump body for
//  any of them in this batch.
//----------------------------------------------------------------------------
namespace {
struct SCheckedSlot_0x6F226DC0
{
    SCheckedSlot_0x6F226DC0(CAgent* candidate);
    SCheckedSlot_0x6F226DC0* Assign(CAgent* candidate);
    ~SCheckedSlot_0x6F226DC0() { Release(); }
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
    CAgent* m_value;
};
}

__declspec(naked) SCheckedSlot_0x6F226DC0::SCheckedSlot_0x6F226DC0(CAgent*)
{
    __asm { mov eax, 06F226DC0h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedSlot_0x6F226DC0* SCheckedSlot_0x6F226DC0::Assign(CAgent*)
{
    __asm { mov eax, 06F219F00h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int __fastcall GetConstant_0x6F1587E0()
{
    __asm { mov eax, 06F1587E0h }
    __asm { jmp eax }
}

CAgent* QueryAgentSingleton_0x6F246BA0()
{
    SCheckedSlot_0x6F226DC0 made(0);

    AGILE_TYPE_ID id(GetConstant_0x6F1587E0());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetConstant_0x6F1587E0(), type->m_allocator);

    made.Assign(((SMadeAgent54*)SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);
    return made.m_value;
}


//----------------------------------------------------------------------------
//  0x6F247220.  ctor 0x6F2270C0 (retn 4) / type-id leaf 0x6F156F20 (retn 0) /
//  Assign 0x6F21A300 (retn 4) - all three naked thunks, no dump body for
//  any of them in this batch.
//----------------------------------------------------------------------------
namespace {
struct SCheckedSlot_0x6F2270C0
{
    SCheckedSlot_0x6F2270C0(CAgent* candidate);
    SCheckedSlot_0x6F2270C0* Assign(CAgent* candidate);
    ~SCheckedSlot_0x6F2270C0() { Release(); }
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
    CAgent* m_value;
};
}

__declspec(naked) SCheckedSlot_0x6F2270C0::SCheckedSlot_0x6F2270C0(CAgent*)
{
    __asm { mov eax, 06F2270C0h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedSlot_0x6F2270C0* SCheckedSlot_0x6F2270C0::Assign(CAgent*)
{
    __asm { mov eax, 06F21A300h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int __fastcall GetConstant_0x6F156F20()
{
    __asm { mov eax, 06F156F20h }
    __asm { jmp eax }
}

CAgent* QueryAgentSingleton_0x6F247220()
{
    SCheckedSlot_0x6F2270C0 made(0);

    AGILE_TYPE_ID id(GetConstant_0x6F156F20());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetConstant_0x6F156F20(), type->m_allocator);

    made.Assign(((SMadeAgent54*)SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);
    return made.m_value;
}


//----------------------------------------------------------------------------
//  0x6F247B10.  ctor 0x6F2274E0 (retn 4) / type-id leaf 0x6F15A190 (retn 0) /
//  Assign 0x6F21A880 (retn 4) - all three naked thunks, no dump body for
//  any of them in this batch.
//----------------------------------------------------------------------------
namespace {
struct SCheckedSlot_0x6F2274E0
{
    SCheckedSlot_0x6F2274E0(CAgent* candidate);
    SCheckedSlot_0x6F2274E0* Assign(CAgent* candidate);
    ~SCheckedSlot_0x6F2274E0() { Release(); }
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
    CAgent* m_value;
};
}

__declspec(naked) SCheckedSlot_0x6F2274E0::SCheckedSlot_0x6F2274E0(CAgent*)
{
    __asm { mov eax, 06F2274E0h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedSlot_0x6F2274E0* SCheckedSlot_0x6F2274E0::Assign(CAgent*)
{
    __asm { mov eax, 06F21A880h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int __fastcall GetConstant_0x6F15A190()
{
    __asm { mov eax, 06F15A190h }
    __asm { jmp eax }
}

CAgent* QueryAgentSingleton_0x6F247B10()
{
    SCheckedSlot_0x6F2274E0 made(0);

    AGILE_TYPE_ID id(GetConstant_0x6F15A190());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetConstant_0x6F15A190(), type->m_allocator);

    made.Assign(((SMadeAgent54*)SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);
    return made.m_value;
}


//----------------------------------------------------------------------------
//  0x6F248190.  ctor 0x6F227780 (retn 4) / type-id leaf 0x6F157480 (retn 0) /
//  Assign 0x6F21AC00 (retn 4) - all three naked thunks, no dump body for
//  any of them in this batch.
//----------------------------------------------------------------------------
namespace {
struct SCheckedSlot_0x6F227780
{
    SCheckedSlot_0x6F227780(CAgent* candidate);
    SCheckedSlot_0x6F227780* Assign(CAgent* candidate);
    ~SCheckedSlot_0x6F227780() { Release(); }
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
    CAgent* m_value;
};
}

__declspec(naked) SCheckedSlot_0x6F227780::SCheckedSlot_0x6F227780(CAgent*)
{
    __asm { mov eax, 06F227780h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedSlot_0x6F227780* SCheckedSlot_0x6F227780::Assign(CAgent*)
{
    __asm { mov eax, 06F21AC00h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int __fastcall GetConstant_0x6F157480()
{
    __asm { mov eax, 06F157480h }
    __asm { jmp eax }
}

CAgent* QueryAgentSingleton_0x6F248190()
{
    SCheckedSlot_0x6F227780 made(0);

    AGILE_TYPE_ID id(GetConstant_0x6F157480());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetConstant_0x6F157480(), type->m_allocator);

    made.Assign(((SMadeAgent54*)SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);
    return made.m_value;
}


//----------------------------------------------------------------------------
//  0x6F248810.  ctor 0x6F227A80 (retn 4) / type-id leaf 0x6F0DC9A0 (retn 0) /
//  Assign 0x6F21B000 (retn 4) - all three naked thunks, no dump body for
//  any of them in this batch.
//----------------------------------------------------------------------------
namespace {
struct SCheckedSlot_0x6F227A80
{
    SCheckedSlot_0x6F227A80(CAgent* candidate);
    SCheckedSlot_0x6F227A80* Assign(CAgent* candidate);
    ~SCheckedSlot_0x6F227A80() { Release(); }
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
    CAgent* m_value;
};
}

__declspec(naked) SCheckedSlot_0x6F227A80::SCheckedSlot_0x6F227A80(CAgent*)
{
    __asm { mov eax, 06F227A80h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedSlot_0x6F227A80* SCheckedSlot_0x6F227A80::Assign(CAgent*)
{
    __asm { mov eax, 06F21B000h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int __fastcall GetConstant_0x6F0DC9A0()
{
    __asm { mov eax, 06F0DC9A0h }
    __asm { jmp eax }
}

CAgent* QueryAgentSingleton_0x6F248810()
{
    SCheckedSlot_0x6F227A80 made(0);

    AGILE_TYPE_ID id(GetConstant_0x6F0DC9A0());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetConstant_0x6F0DC9A0(), type->m_allocator);

    made.Assign(((SMadeAgent54*)SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);
    return made.m_value;
}


//----------------------------------------------------------------------------
//  0x6F248E90.  ctor 0x6F227D80 (retn 4) / type-id leaf 0x6F0DC200 (retn 0) /
//  Assign 0x6F21B400 (retn 4) - all three naked thunks, no dump body for
//  any of them in this batch.
//----------------------------------------------------------------------------
namespace {
struct SCheckedSlot_0x6F227D80
{
    SCheckedSlot_0x6F227D80(CAgent* candidate);
    SCheckedSlot_0x6F227D80* Assign(CAgent* candidate);
    ~SCheckedSlot_0x6F227D80() { Release(); }
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
    CAgent* m_value;
};
}

__declspec(naked) SCheckedSlot_0x6F227D80::SCheckedSlot_0x6F227D80(CAgent*)
{
    __asm { mov eax, 06F227D80h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedSlot_0x6F227D80* SCheckedSlot_0x6F227D80::Assign(CAgent*)
{
    __asm { mov eax, 06F21B400h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int __fastcall GetConstant_0x6F0DC200()
{
    __asm { mov eax, 06F0DC200h }
    __asm { jmp eax }
}

CAgent* QueryAgentSingleton_0x6F248E90()
{
    SCheckedSlot_0x6F227D80 made(0);

    AGILE_TYPE_ID id(GetConstant_0x6F0DC200());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetConstant_0x6F0DC200(), type->m_allocator);

    made.Assign(((SMadeAgent54*)SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);
    return made.m_value;
}


//----------------------------------------------------------------------------
//  0x6F249510.  ctor 0x6F228080 (retn 4) / type-id leaf 0x6F07FED0 (retn 0) /
//  Assign 0x6F21B800 (retn 4) - all three naked thunks, no dump body for
//  any of them in this batch.
//----------------------------------------------------------------------------
namespace {
struct SCheckedSlot_0x6F228080
{
    SCheckedSlot_0x6F228080(CAgent* candidate);
    SCheckedSlot_0x6F228080* Assign(CAgent* candidate);
    ~SCheckedSlot_0x6F228080() { Release(); }
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
    CAgent* m_value;
};
}

__declspec(naked) SCheckedSlot_0x6F228080::SCheckedSlot_0x6F228080(CAgent*)
{
    __asm { mov eax, 06F228080h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedSlot_0x6F228080* SCheckedSlot_0x6F228080::Assign(CAgent*)
{
    __asm { mov eax, 06F21B800h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int __fastcall GetConstant_0x6F07FED0()
{
    __asm { mov eax, 06F07FED0h }
    __asm { jmp eax }
}

CAgent* QueryAgentSingleton_0x6F249510()
{
    SCheckedSlot_0x6F228080 made(0);

    AGILE_TYPE_ID id(GetConstant_0x6F07FED0());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetConstant_0x6F07FED0(), type->m_allocator);

    made.Assign(((SMadeAgent54*)SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);
    return made.m_value;
}


//----------------------------------------------------------------------------
//  0x6F249B90.  ctor 0x6F228380 (retn 4) / type-id leaf 0x6F0DBB80 (retn 0) /
//  Assign 0x6F21BC00 (retn 4) - all three naked thunks, no dump body for
//  any of them in this batch.
//----------------------------------------------------------------------------
namespace {
struct SCheckedSlot_0x6F228380
{
    SCheckedSlot_0x6F228380(CAgent* candidate);
    SCheckedSlot_0x6F228380* Assign(CAgent* candidate);
    ~SCheckedSlot_0x6F228380() { Release(); }
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
    CAgent* m_value;
};
}

__declspec(naked) SCheckedSlot_0x6F228380::SCheckedSlot_0x6F228380(CAgent*)
{
    __asm { mov eax, 06F228380h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedSlot_0x6F228380* SCheckedSlot_0x6F228380::Assign(CAgent*)
{
    __asm { mov eax, 06F21BC00h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int __fastcall GetConstant_0x6F0DBB80()
{
    __asm { mov eax, 06F0DBB80h }
    __asm { jmp eax }
}

CAgent* QueryAgentSingleton_0x6F249B90()
{
    SCheckedSlot_0x6F228380 made(0);

    AGILE_TYPE_ID id(GetConstant_0x6F0DBB80());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetConstant_0x6F0DBB80(), type->m_allocator);

    made.Assign(((SMadeAgent54*)SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);
    return made.m_value;
}


//----------------------------------------------------------------------------
//  0x6F24A210.  ctor 0x6F228680 (retn 4) / type-id leaf 0x6F0DCDA0 (retn 0) /
//  Assign 0x6F21C000 (retn 4) - all three naked thunks, no dump body for
//  any of them in this batch.
//----------------------------------------------------------------------------
namespace {
struct SCheckedSlot_0x6F228680
{
    SCheckedSlot_0x6F228680(CAgent* candidate);
    SCheckedSlot_0x6F228680* Assign(CAgent* candidate);
    ~SCheckedSlot_0x6F228680() { Release(); }
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
    CAgent* m_value;
};
}

__declspec(naked) SCheckedSlot_0x6F228680::SCheckedSlot_0x6F228680(CAgent*)
{
    __asm { mov eax, 06F228680h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedSlot_0x6F228680* SCheckedSlot_0x6F228680::Assign(CAgent*)
{
    __asm { mov eax, 06F21C000h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int __fastcall GetConstant_0x6F0DCDA0()
{
    __asm { mov eax, 06F0DCDA0h }
    __asm { jmp eax }
}

CAgent* QueryAgentSingleton_0x6F24A210()
{
    SCheckedSlot_0x6F228680 made(0);

    AGILE_TYPE_ID id(GetConstant_0x6F0DCDA0());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetConstant_0x6F0DCDA0(), type->m_allocator);

    made.Assign(((SMadeAgent54*)SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);
    return made.m_value;
}


//----------------------------------------------------------------------------
//  0x6F24A960.  ctor 0x6F228980 (retn 4) / type-id leaf 0x6F195010 (retn 0) /
//  Assign 0x6F21C400 (retn 4) - all three naked thunks, no dump body for
//  any of them in this batch.
//----------------------------------------------------------------------------
namespace {
struct SCheckedSlot_0x6F228980
{
    SCheckedSlot_0x6F228980(CAgent* candidate);
    SCheckedSlot_0x6F228980* Assign(CAgent* candidate);
    ~SCheckedSlot_0x6F228980() { Release(); }
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
    CAgent* m_value;
};
}

__declspec(naked) SCheckedSlot_0x6F228980::SCheckedSlot_0x6F228980(CAgent*)
{
    __asm { mov eax, 06F228980h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedSlot_0x6F228980* SCheckedSlot_0x6F228980::Assign(CAgent*)
{
    __asm { mov eax, 06F21C400h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int __fastcall GetConstant_0x6F195010()
{
    __asm { mov eax, 06F195010h }
    __asm { jmp eax }
}

CAgent* QueryAgentSingleton_0x6F24A960()
{
    SCheckedSlot_0x6F228980 made(0);

    AGILE_TYPE_ID id(GetConstant_0x6F195010());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetConstant_0x6F195010(), type->m_allocator);

    made.Assign(((SMadeAgent54*)SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);
    return made.m_value;
}


//----------------------------------------------------------------------------
//  0x6F24AFE0.  ctor 0x6F228C80 (retn 4) / type-id leaf 0x6F0DAA80 (retn 0) /
//  Assign 0x6F21C800 (retn 4) - all three naked thunks, no dump body for
//  any of them in this batch.
//----------------------------------------------------------------------------
namespace {
struct SCheckedSlot_0x6F228C80
{
    SCheckedSlot_0x6F228C80(CAgent* candidate);
    SCheckedSlot_0x6F228C80* Assign(CAgent* candidate);
    ~SCheckedSlot_0x6F228C80() { Release(); }
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
    CAgent* m_value;
};
}

__declspec(naked) SCheckedSlot_0x6F228C80::SCheckedSlot_0x6F228C80(CAgent*)
{
    __asm { mov eax, 06F228C80h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedSlot_0x6F228C80* SCheckedSlot_0x6F228C80::Assign(CAgent*)
{
    __asm { mov eax, 06F21C800h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int __fastcall GetConstant_0x6F0DAA80()
{
    __asm { mov eax, 06F0DAA80h }
    __asm { jmp eax }
}

CAgent* QueryAgentSingleton_0x6F24AFE0()
{
    SCheckedSlot_0x6F228C80 made(0);

    AGILE_TYPE_ID id(GetConstant_0x6F0DAA80());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetConstant_0x6F0DAA80(), type->m_allocator);

    made.Assign(((SMadeAgent54*)SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);
    return made.m_value;
}


//----------------------------------------------------------------------------
//  0x6F24B660.  ctor 0x6F228F80 (retn 4) / type-id leaf 0x6F1D30A0 (retn 0) /
//  Assign 0x6F21CC00 (retn 4) - all three naked thunks, no dump body for
//  any of them in this batch.
//----------------------------------------------------------------------------
namespace {
struct SCheckedSlot_0x6F228F80
{
    SCheckedSlot_0x6F228F80(CAgent* candidate);
    SCheckedSlot_0x6F228F80* Assign(CAgent* candidate);
    ~SCheckedSlot_0x6F228F80() { Release(); }
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
    CAgent* m_value;
};
}

__declspec(naked) SCheckedSlot_0x6F228F80::SCheckedSlot_0x6F228F80(CAgent*)
{
    __asm { mov eax, 06F228F80h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedSlot_0x6F228F80* SCheckedSlot_0x6F228F80::Assign(CAgent*)
{
    __asm { mov eax, 06F21CC00h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int __fastcall GetConstant_0x6F1D30A0()
{
    __asm { mov eax, 06F1D30A0h }
    __asm { jmp eax }
}

CAgent* QueryAgentSingleton_0x6F24B660()
{
    SCheckedSlot_0x6F228F80 made(0);

    AGILE_TYPE_ID id(GetConstant_0x6F1D30A0());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetConstant_0x6F1D30A0(), type->m_allocator);

    made.Assign(((SMadeAgent54*)SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);
    return made.m_value;
}


//----------------------------------------------------------------------------
//  0x6F24BCE0.  ctor 0x6F229280 (retn 4) / type-id leaf 0x6F10F680 (retn 0) /
//  Assign 0x6F21D000 (retn 4) - all three naked thunks, no dump body for
//  any of them in this batch.
//----------------------------------------------------------------------------
namespace {
struct SCheckedSlot_0x6F229280
{
    SCheckedSlot_0x6F229280(CAgent* candidate);
    SCheckedSlot_0x6F229280* Assign(CAgent* candidate);
    ~SCheckedSlot_0x6F229280() { Release(); }
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
    CAgent* m_value;
};
}

__declspec(naked) SCheckedSlot_0x6F229280::SCheckedSlot_0x6F229280(CAgent*)
{
    __asm { mov eax, 06F229280h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedSlot_0x6F229280* SCheckedSlot_0x6F229280::Assign(CAgent*)
{
    __asm { mov eax, 06F21D000h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int __fastcall GetConstant_0x6F10F680()
{
    __asm { mov eax, 06F10F680h }
    __asm { jmp eax }
}

CAgent* QueryAgentSingleton_0x6F24BCE0()
{
    SCheckedSlot_0x6F229280 made(0);

    AGILE_TYPE_ID id(GetConstant_0x6F10F680());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetConstant_0x6F10F680(), type->m_allocator);

    made.Assign(((SMadeAgent54*)SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);
    return made.m_value;
}


//----------------------------------------------------------------------------
//  0x6F24C360.  ctor 0x6F229520 (retn 4) / type-id leaf 0x6F024D10 (retn 0) /
//  Assign 0x6F21D380 (retn 4) - all three naked thunks, no dump body for
//  any of them in this batch.
//----------------------------------------------------------------------------
namespace {
struct SCheckedSlot_0x6F229520
{
    SCheckedSlot_0x6F229520(CAgent* candidate);
    SCheckedSlot_0x6F229520* Assign(CAgent* candidate);
    ~SCheckedSlot_0x6F229520() { Release(); }
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
    CAgent* m_value;
};
}

__declspec(naked) SCheckedSlot_0x6F229520::SCheckedSlot_0x6F229520(CAgent*)
{
    __asm { mov eax, 06F229520h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedSlot_0x6F229520* SCheckedSlot_0x6F229520::Assign(CAgent*)
{
    __asm { mov eax, 06F21D380h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int __fastcall GetConstant_0x6F024D10()
{
    __asm { mov eax, 06F024D10h }
    __asm { jmp eax }
}

CAgent* QueryAgentSingleton_0x6F24C360()
{
    SCheckedSlot_0x6F229520 made(0);

    AGILE_TYPE_ID id(GetConstant_0x6F024D10());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetConstant_0x6F024D10(), type->m_allocator);

    made.Assign(((SMadeAgent54*)SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);
    return made.m_value;
}


//----------------------------------------------------------------------------
//  0x6F24C9E0.  ctor 0x6F229820 (retn 4) / type-id leaf 0x6F0DDC20 (retn 0) /
//  Assign 0x6F21D780 (retn 4) - all three naked thunks, no dump body for
//  any of them in this batch.
//----------------------------------------------------------------------------
namespace {
struct SCheckedSlot_0x6F229820
{
    SCheckedSlot_0x6F229820(CAgent* candidate);
    SCheckedSlot_0x6F229820* Assign(CAgent* candidate);
    ~SCheckedSlot_0x6F229820() { Release(); }
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
    CAgent* m_value;
};
}

__declspec(naked) SCheckedSlot_0x6F229820::SCheckedSlot_0x6F229820(CAgent*)
{
    __asm { mov eax, 06F229820h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedSlot_0x6F229820* SCheckedSlot_0x6F229820::Assign(CAgent*)
{
    __asm { mov eax, 06F21D780h }
    __asm { jmp eax }
}

__declspec(naked) unsigned int __fastcall GetConstant_0x6F0DDC20()
{
    __asm { mov eax, 06F0DDC20h }
    __asm { jmp eax }
}

CAgent* QueryAgentSingleton_0x6F24C9E0()
{
    SCheckedSlot_0x6F229820 made(0);

    AGILE_TYPE_ID id(GetConstant_0x6F0DDC20());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetConstant_0x6F0DDC20(), type->m_allocator);

    made.Assign(((SMadeAgent54*)SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);
    return made.m_value;
}


//----------------------------------------------------------------------------
//  0x6F29AAD0.  ctor 0x6F280D80 (retn 4) / type-id leaf 0x6F0D9120 (retn 0) /
//  Assign 0x6F275530 (retn 4) - all three naked thunks, no dump body for
//  any of them in this batch.
//
//  0x6F280D80 turned out to already be cunit_agent4's own worklist item,
//  properly claimed and scored there as a real converting-constructor
//  reconstruction (SAgent4CheckedSlot_6F280D80, tools/funcmap.py) - better
//  than the naked passthrough thunk below, which carries no funcmap row of
//  its own and exists only so QueryAgentSingleton_0x6F29AAD0 compiles.
//----------------------------------------------------------------------------
namespace {
struct SCheckedSlot_0x6F280D80
{
    SCheckedSlot_0x6F280D80(CAgent* candidate);
    SCheckedSlot_0x6F280D80* Assign(CAgent* candidate);
    ~SCheckedSlot_0x6F280D80() { Release(); }
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
    CAgent* m_value;
};
}

__declspec(naked) SCheckedSlot_0x6F280D80::SCheckedSlot_0x6F280D80(CAgent*)
{
    __asm { mov eax, 06F280D80h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedSlot_0x6F280D80* SCheckedSlot_0x6F280D80::Assign(CAgent*)
{
    __asm { mov eax, 06F275530h }
    __asm { jmp eax }
}

//  0x6F0D9120 turned out to already be cunit_agent7's own worklist item,
//  properly claimed and scored there as a real reconstruction
//  (GetConstant_6F0D9120, tools/funcmap.py) - better than the naked
//  passthrough thunk below, which carries no funcmap row of its own and
//  exists only so QueryAgentSingleton_0x6F29AAD0 compiles.
__declspec(naked) unsigned int __fastcall GetConstant_0x6F0D9120()
{
    __asm { mov eax, 06F0D9120h }
    __asm { jmp eax }
}

CAgent* QueryAgentSingleton_0x6F29AAD0()
{
    SCheckedSlot_0x6F280D80 made(0);

    AGILE_TYPE_ID id(GetConstant_0x6F0D9120());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetConstant_0x6F0D9120(), type->m_allocator);

    made.Assign(((SMadeAgent54*)SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);
    return made.m_value;
}


//----------------------------------------------------------------------------
//  0x6F29B150.  ctor 0x6F281080 (retn 4) / Assign 0x6F275930 (retn 4) -
//  naked thunks, no dump body for either in this batch.  The type-id leaf
//  at 0x6F028740 is already a real reconstruction elsewhere
//  (GetFourCC_6F028740, misc_trivial_getters.cpp) - grepped before adding
//  a second thunk for the same address, per CLAUDE.md's own rule.
//
//  0x6F281080 turned out to already be cunit_agent4's own worklist item,
//  properly claimed and scored there as a real converting-constructor
//  reconstruction (SAgent4CheckedSlot_6F281080, tools/funcmap.py) - better
//  than the naked passthrough thunk below, which carries no funcmap row of
//  its own and exists only so QueryAgentSingleton_0x6F29B150 compiles.
//----------------------------------------------------------------------------
namespace {
struct SCheckedSlot_0x6F281080
{
    SCheckedSlot_0x6F281080(CAgent* candidate);
    SCheckedSlot_0x6F281080* Assign(CAgent* candidate);
    ~SCheckedSlot_0x6F281080() { Release(); }
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
    CAgent* m_value;
};
}

__declspec(naked) SCheckedSlot_0x6F281080::SCheckedSlot_0x6F281080(CAgent*)
{
    __asm { mov eax, 06F281080h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedSlot_0x6F281080* SCheckedSlot_0x6F281080::Assign(CAgent*)
{
    __asm { mov eax, 06F275930h }
    __asm { jmp eax }
}

//  0x6F028740 - already real (misc_trivial_getters.cpp).
unsigned int GetFourCC_6F028740();

CAgent* QueryAgentSingleton_0x6F29B150()
{
    SCheckedSlot_0x6F281080 made(0);

    AGILE_TYPE_ID id(GetFourCC_6F028740());
    CGameData* game = g_pGameData;
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F028740(), type->m_allocator);

    made.Assign(((SMadeAgent54*)SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);
    return made.m_value;
}
