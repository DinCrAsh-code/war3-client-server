//============================================================================
//  0x6F0708B0 - CUnit ability-agent install dispatcher, and 0x6F06F0C0, one
//  of its four handlers - both CUnit batch-19, slot 107's own depth<=2
//  closure (the same worklist slice unit_abilitybyfootprinttype.cpp's own
//  pair comes from).
//
//  0x6F0708B0 (`this` in ecx, no stack arguments, bare `retn`) classifies
//  this unit by UnitTypeField_6F29C1B0(m_footprintType) - widget.h's +0x30,
//  the same fifth AGILE_TYPE_ID probe ability_slots.cpp's own
//  GetPlacementCursorTexture already uses for a race lookup - into one of
//  four buckets (2, 3, 4, or "anything else") and calls the matching
//  MakeXxxAgent factory with no arguments at all, the same parameterless
//  MakeAbilityAgent_XXXX shape cunit_agent8_abilityfactories.cpp's own 59
//  already catalogue.  Only the "anything else" bucket (0x6F06F0C0, rawcode
//  'Afhi') is in this agent's own worklist slice; the other three
//  (0x6F06EFF0/0x6F06EF20/0x6F06EE50) are declared only - the same
//  "somebody else's reconstruction" treatment cunit_agent8_abilityfactories.cpp
//  gives its own out-of-slice ctor/Assign pairs, and for the same
//  collision-avoidance reason: another concurrent agent's own slice may
//  already be reconstructing one of them under a different name.
//
//  Once a handler returns the new CAgent, the dispatcher calls that
//  agent's own vtable slot +0x84 with (newAgent, this) - almost certainly
//  an "attach to owner" of some kind, but there is no committed vtable for
//  whatever concrete CAgent subclass this factory family builds, so - per
//  CLAUDE.md's own guidance for an unresolved indirect call - it is written
//  as a plain untyped function-pointer cast on the call site rather than
//  through a named virtual.  It finishes with the same
//  NotifyAbilityListChanged(manager) tail call
//  unit_abilitybyfootprinttype.cpp's own SUnitAbilityHost struct already
//  names (0x6F079990); the local redeclaration here is that same struct,
//  repeated per this repo's own established convention for a struct one
//  more TU does not want to pull a shared header in for just to reuse.
//
//  What "kind 2/3/4" and "everything else" actually select (a per-race
//  innate ability, going by ResolveSkinValueByRace's own 1-5 race-index
//  convention elsewhere in this tree) is a guess this call tree does not
//  confirm, so the dispatcher keeps a shape-only name rather than a
//  claimed one.
//============================================================================
#include "agent.h"
#include "agiletype.h"
#include "game.h"
#include "widgetagentquery.h"
#include "unit.h"

//  Re-declared per this repo's own convention for a struct a second TU
//  does not want to pull a shared header in for - see
//  unit_abilitybyfootprinttype.cpp's own copy.
struct SUnitAbilityHost
{
    void  NotifyAbilityListChanged(void* manager);   // 0x6F079990
};

//  unittypequeries.cpp - __fastcall, the unit's own footprint-type FourCC
//  (widget.h, +0x30) in ecx.
extern int __fastcall UnitTypeField_6F29C1B0(unsigned int fourCC);

//  What SubmitWidgetAgentQuery hands back: only the CAgent* at +0x54 is
//  reached (same reasoning as cunit_agent8_abilityfactories.cpp's own
//  private, per-TU redeclaration).
namespace {
struct SMadeAgentHolder
{
    char    m_reserved00[0x54];
    CAgent* m_pAgent;   // +0x54
};
}

//  Out-of-slice siblings of MakeAbilityAgent_Afhi below (same dispatcher,
//  the other three classification buckets) - not in this agent's worklist
//  slice.  Each is a real, dump-confirmed function (agent_worktrees has
//  raw_asm/raw_bytes, all `retn 0`) that nobody has reconstructed yet, so
//  - per the same collision-avoidance reasoning as every other out-of-
//  slice callee this session declared - each is a self-redirect thunk to
//  its own real, unhooked body rather than left merely declared (a bare
//  declaration with nothing to define it is a live link failure, not a
//  placeholder; link_check.py caught exactly this).  Not claimed in
//  agent_worktrees: a thunk makes no behavioural claim about the address,
//  so there is nothing here for a future session to overwrite.
CAgent* MakeAbilityAgentByFootprintKind_6F06EFF0();
CAgent* MakeAbilityAgentByFootprintKind_6F06EF20();
CAgent* MakeAbilityAgentByFootprintKind_6F06EE50();

__declspec(naked) CAgent* MakeAbilityAgentByFootprintKind_6F06EFF0()
{
    __asm { mov eax, 06F06EFF0h }
    __asm { jmp eax }
}

__declspec(naked) CAgent* MakeAbilityAgentByFootprintKind_6F06EF20()
{
    __asm { mov eax, 06F06EF20h }
    __asm { jmp eax }
}

__declspec(naked) CAgent* MakeAbilityAgentByFootprintKind_6F06EE50()
{
    __asm { mov eax, 06F06EE50h }
    __asm { jmp eax }
}

//----------------------------------------------------------------------------
//  0x6F06F0C0 - MakeAbilityAgent_Afhi, the "anything else" bucket.  Same
//  shape as cunit_agent8_abilityfactories.cpp's own 59 - type-checked slot
//  ctor (0x6F03F8E0) and Assign (0x6F02D580) two hops out, so declared and
//  self-redirect-thunked rather than defined, same collision-avoidance
//  reasoning as that file's own out-of-slice pairs.
//----------------------------------------------------------------------------
struct SCheckedAbilitySlot_Afhi
{
    SCheckedAbilitySlot_Afhi(CAgent* candidate);          //  0x6F03F8E0
    SCheckedAbilitySlot_Afhi* Assign(CAgent* candidate);  //  0x6F02D580

    void Release()
    {
        CAgent* held = m_value;
        if (held)
        {
            if (--held->m_refcount == 0)
                held->ReleaseSelf();
        }
    }
    ~SCheckedAbilitySlot_Afhi() { Release(); }

    CAgent* m_value;   // +0x00
};

//  A self-redirect thunk, not a real `return 0x...;` body, even though
//  agent_worktrees confirms this is a plain `mov eax, imm32 / retn` leaf:
//  MakeAbilityAgent_Afhi below calls it twice from this same TU, and a
//  real one-line definition here is visible to /O2's own inliner, which
//  folds both calls away and drops the `call` instructions the shipped
//  dump plainly keeps (measured wrong the first time
//  cunit_agent8_abilityfactories.cpp's own 20 added getters tried exactly
//  this - see that file's own comment on it). A naked body is opaque to
//  the optimiser and can never be inlined.
__declspec(naked) unsigned int GetFourCC_6F026CE0()   //  0x6F026CE0 - 'Afhi'
{
    __asm { mov eax, 06F026CE0h }
    __asm { jmp eax }
}

CAgent* MakeAbilityAgent_Afhi()
{
    SCheckedAbilitySlot_Afhi held(0);

    unsigned int typeId = GetFourCC_6F026CE0();
    CGameData* game = g_pGameData;
    AGILE_TYPE_ID id(typeId);
    unsigned int hash = id.Hash();
    AGILE_TYPE_DATA* type = game->m_types.Ptr(hash, id);
    void* pool = type->m_allocator;

    SWidgetAgentQuery query;
    InitWidgetAgentQuery(&query, GetFourCC_6F026CE0(), pool);
    held.Assign(((SMadeAgentHolder*)
                 SubmitWidgetAgentQuery(&query, 1, 1))->m_pAgent);

    return held.m_value;
}

__declspec(naked) SCheckedAbilitySlot_Afhi::SCheckedAbilitySlot_Afhi(CAgent* candidate)
{
    __asm { mov eax, 06F03F8E0h }
    __asm { jmp eax }
}

__declspec(naked) SCheckedAbilitySlot_Afhi* SCheckedAbilitySlot_Afhi::Assign(CAgent* candidate)
{
    __asm { mov eax, 06F02D580h }
    __asm { jmp eax }
}

//----------------------------------------------------------------------------
//  0x6F0708B0 - the dispatcher itself.
//----------------------------------------------------------------------------
typedef void (__thiscall *AttachAbilityAgentToOwnerFn)(CAgent*, CUnit*);

void CUnit::InstallAbilityAgentByFootprintKind()
{
    unsigned int kind = UnitTypeField_6F29C1B0(m_footprintType);

    CAgent* agent;
    switch (kind)
    {
    case 2:  agent = MakeAbilityAgentByFootprintKind_6F06EE50(); break;
    case 3:  agent = MakeAbilityAgentByFootprintKind_6F06EF20(); break;
    case 4:  agent = MakeAbilityAgentByFootprintKind_6F06EFF0(); break;
    default: agent = MakeAbilityAgent_Afhi();                    break;
    }

    ((AttachAbilityAgentToOwnerFn)(*(void***)agent)[0x84 / 4])(agent, this);

    ((SUnitAbilityHost*)this)->NotifyAbilityListChanged(agent);
}
