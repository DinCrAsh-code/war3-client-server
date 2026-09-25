//============================================================================
//  The event-context unit/item getter family, 0x6F3C2xxx - one module, one
//  translation unit: twenty JASS natives that all read a different field
//  out of the current trigger event's own CScriptEvent/CScriptEventData
//  (jassevents.h), then, for the natives that hand back an object rather
//  than a raw integer, register whatever they resolved through
//  g_pItemSlotHost->GetSlotTable()->Register() - the same
//  "GetSlotTable then Register" tail jassunititeminslot_native.cpp already
//  establishes.
//
//  Three shapes recur throughout, matched to each native's own body below
//  rather than factored into a shared function - the shipped bodies really
//  do repeat these instructions inline at every call site, so factoring
//  them out would insert calls the dump does not have:
//
//   * two-id "if/else": test the (biased) event id against one constant,
//     then against a second one `0x2C` or `0x11`/`0x25` past it, resolving
//     a different CScriptEventData ref for each and returning 0 for
//     neither;
//   * dense switch (MSVC's byte-map/jump-table lowering) over a wider id
//     range, several ids sharing one resolve-and-register arm;
//   * a "raw field" pair (GetTrainedUnitType, GetLearnedSkill,
//     GetResearched) that returns whatever CScriptEventData already holds
//     at a fixed offset directly - a type or skill id, not an object -
//     and never touches the agent registry at all.
//
//  Whether the leading `GetCurrentScriptEvent() != 0` guard is present, and
//  whether the arm that follows re-tests `GetCurrentScriptEventData()`'s
//  own result before using it, is not consistent across the twenty - each
//  native below matches its own dump exactly rather than a rule inferred
//  from its neighbours.
//============================================================================
#include "jassevents.h"
#include "agentregistry.h"
#include "itemhandlemain.h"

//  0x6F0418A0 - Agent/agenthandlelive.cpp.
extern int __fastcall IsAgentHandleLive(const void* agent);

//  0x6F3A1650 - IndexedArrayHolder_6F3A1650::At (Misc/misc_field_getters.cpp),
//  redeclared to match that definition exactly, the way every other caller
//  in this repo does (see e.g. Agent/relationconstant.cpp) - NOT wrapped in
//  an anonymous namespace, which would mangle differently and leave the
//  call unresolved against the real definition.
struct IndexedArrayHolder_6F3A1650
{
    char pad[0x58];
    int  m_data[1];
    int  At(unsigned int index);
};

//----------------------------------------------------------------------------
//  0x6F3C20F0 - GetAttacker.  ids 0x80212 (attack) / 0x8023E (kill, the
//  first + 0x2C).
//----------------------------------------------------------------------------
int __cdecl GetAttacker()
{
    if (!GetCurrentScriptEvent())
        return 0;

    void* resolved;

    switch (GetCurrentEventId() + kEventIdBias)
    {
    case 0x80212:
        resolved = GetCurrentScriptEventData()->GetOrderRefForAttackEvent();
        break;

    case 0x8023E:
        resolved = GetCurrentScriptEventData()->GetAbilityRefForKillEvent();
        break;

    default:
        return 0;
    }

    CAgentRegistry* registry = (CAgentRegistry*)g_pItemSlotHost->GetSlotTable();
    return (int)registry->Register((CUnitAgent*)resolved, 0);
}

//----------------------------------------------------------------------------
//  0x6F3C23F0 - GetKillingUnit.  No leading event guard; both arms
//  re-test GetCurrentScriptEventData()'s own result.  ids 0x80214 (kill) /
//  0x80235 (the first + 0x21).
//----------------------------------------------------------------------------
int __cdecl GetKillingUnit()
{
    void* resolved;

    switch (GetCurrentEventId() + kEventIdBias)
    {
    case 0x80214:
    {
        CScriptEventData* data = GetCurrentScriptEventData();
        if (!data)
            return 0;
        resolved = data->GetOrderRefForAttackEvent();
        break;
    }

    case 0x80235:
    {
        CScriptEventData* data = GetCurrentScriptEventData();
        if (!data)
            return 0;
        resolved = data->GetAbilityRefForKillEvent();
        break;
    }

    default:
        return 0;
    }

    CAgentRegistry* registry = (CAgentRegistry*)g_pItemSlotHost->GetSlotTable();
    return (int)registry->Register((CUnitAgent*)resolved, 0);
}

//----------------------------------------------------------------------------
//  0x6F3C2390 - GetRescuer.  ids 0x80213 (rescue) / 0x8023F (the first +
//  0x2C).
//----------------------------------------------------------------------------
int __cdecl GetRescuer()
{
    if (!GetCurrentScriptEvent())
        return 0;

    void* resolved;

    switch (GetCurrentEventId() + kEventIdBias)
    {
    case 0x80213:
        resolved = GetCurrentScriptEventData()->GetOrderRefForAttackEvent();
        break;

    case 0x8023F:
        resolved = GetCurrentScriptEventData()->GetAbilityRefForKillEvent();
        break;

    default:
        return 0;
    }

    CAgentRegistry* registry = (CAgentRegistry*)g_pItemSlotHost->GetSlotTable();
    return (int)registry->Register((CUnitAgent*)resolved, 0);
}

//----------------------------------------------------------------------------
//  0x6F3C2450 - GetTrainedUnitType.  A dense switch, no leading guard, and
//  no agent registration at all: two id pairs return CScriptEventData's
//  own +0x44/+0x38 fields directly (a type id, not an object), the other
//  two resolve a ref and read *its* own +0x30.
//----------------------------------------------------------------------------
int __cdecl GetTrainedUnitType()
{
    switch (GetCurrentEventId() + kEventIdBias)
    {
    case 0x80220: case 0x80221:
    {
        CScriptEventData* data = GetCurrentScriptEventData();
        if (!data)
            return 0;
        return data->m_targetHandle;
    }

    case 0x80245: case 0x80246:
    {
        CScriptEventData* data = GetCurrentScriptEventData();
        if (!data)
            return 0;
        return data->m_orderHandle;
    }

    case 0x80222:
    {
        CScriptEventData* data = GetCurrentScriptEventData();
        if (!data)
            return 0;
        void* order = data->GetOrderRefForAttackEvent();
        if (!order)
            return 0;
        return *(int*)((char*)order + 0x30);
    }

    case 0x80247:
    {
        CScriptEventData* data = GetCurrentScriptEventData();
        if (!data)
            return 0;
        void* ability = data->GetAbilityRefForKillEvent();
        if (!ability)
            return 0;
        return *(int*)((char*)ability + 0x30);
    }

    default:
        return 0;
    }
}

//----------------------------------------------------------------------------
//  0x6F3C2680 - GetChangingUnit.  ids 0x8030E (change) / 0x8031F (the
//  first + 0x11).
//----------------------------------------------------------------------------
int __cdecl GetChangingUnit()
{
    if (!GetCurrentScriptEvent())
        return 0;

    void* resolved;

    switch (GetCurrentEventId() + kEventIdBias)
    {
    case 0x8030E:
    {
        CScriptEventData* data = GetCurrentScriptEventData();
        if (!data)
            return 0;
        resolved = data->GetAbilityRefForTrainEvent();
        break;
    }

    case 0x8031F:
    {
        CScriptEventData* data = GetCurrentScriptEventData();
        if (!data)
            return 0;
        resolved = data->ResolveSubjectUnit();
        break;
    }

    default:
        return 0;
    }

    CAgentRegistry* registry = (CAgentRegistry*)g_pItemSlotHost->GetSlotTable();
    return (int)registry->Register((CUnitAgent*)resolved, 0);
}

//----------------------------------------------------------------------------
//  0x6F3C26E0 - GetChangingUnitPrevOwner.  No leading guard.  ids 0x8030E
//  (change) / 0x8031F: not a handle ref at all, but a 16-bit *player
//  index* (CScriptEventData's own +0x44 for the first id, +0x2C for the
//  second) fed to the acting-player-table lookup
//  (IndexedArrayHolder_6F3A1650::At) every other JASS native in this repo
//  that reaches g_unk6FAB65F4 uses, then revalidated through
//  IsAgentHandleLive before registration - the same second-check shape
//  ResolveSubjectUnit (eventsubjectresolve.cpp) folds into its own scoped
//  slot, spelled out here instead because this native never constructs an
//  SCheckedUnitSlot around it.
//----------------------------------------------------------------------------
int __cdecl GetChangingUnitPrevOwner()
{
    unsigned int index;

    switch (GetCurrentEventId() + kEventIdBias)
    {
    case 0x8030E:
    {
        CScriptEventData* data = GetCurrentScriptEventData();
        if (!data)
            return 0;
        index = *(unsigned short*)((char*)data + 0x44);
        break;
    }

    case 0x8031F:
    {
        CScriptEventData* data = GetCurrentScriptEventData();
        if (!data)
            return 0;
        index = *(unsigned short*)((char*)data + 0x2C);
        break;
    }

    default:
        return 0;
    }

    IndexedArrayHolder_6F3A1650* world =
        (IndexedArrayHolder_6F3A1650*)g_unk6FAB65F4;
    void* record = (void*)world->At(index);
    if (!record)
        return 0;

    if (!IsAgentHandleLive(record))
        return 0;

    CAgentRegistry* registry = (CAgentRegistry*)g_pItemSlotHost->GetSlotTable();
    return (int)registry->Register((CUnitAgent*)record, 0);
}

//----------------------------------------------------------------------------
//  0x6F3C21B0 - GetLearningUnit.  ids 0x8022A (learn) / 0x8024F (the
//  first + 0x25).
//----------------------------------------------------------------------------
int __cdecl GetLearningUnit()
{
    if (!GetCurrentScriptEvent())
        return 0;

    void* resolved;

    switch (GetCurrentEventId() + kEventIdBias)
    {
    case 0x8022A:
        resolved = GetCurrentScriptEventData()->GetAbilityRefForTrainEvent();
        break;

    case 0x8024F:
        resolved = GetCurrentScriptEventData()->ResolveSubjectUnit();
        break;

    default:
        return 0;
    }

    CAgentRegistry* registry = (CAgentRegistry*)g_pItemSlotHost->GetSlotTable();
    return (int)registry->Register((CUnitAgent*)resolved, 0);
}

//----------------------------------------------------------------------------
//  0x6F3C2150 - GetLevelingUnit.  ids 0x80229 (level) / 0x8024E (the
//  first + 0x25).
//----------------------------------------------------------------------------
int __cdecl GetLevelingUnit()
{
    if (!GetCurrentScriptEvent())
        return 0;

    void* resolved;

    switch (GetCurrentEventId() + kEventIdBias)
    {
    case 0x80229:
        resolved = GetCurrentScriptEventData()->GetAbilityRefForTrainEvent();
        break;

    case 0x8024E:
        resolved = GetCurrentScriptEventData()->ResolveSubjectUnit();
        break;

    default:
        return 0;
    }

    CAgentRegistry* registry = (CAgentRegistry*)g_pItemSlotHost->GetSlotTable();
    return (int)registry->Register((CUnitAgent*)resolved, 0);
}

//----------------------------------------------------------------------------
//  0x6F3C2A00 - GetLoadedUnit.  ids 0x80233 (load) / 0x80258 (the first +
//  0x25); unlike GetLearningUnit/GetLevelingUnit above, both arms re-test
//  GetCurrentScriptEventData()'s own result even though the leading guard
//  already ran.
//----------------------------------------------------------------------------
int __cdecl GetLoadedUnit()
{
    if (!GetCurrentScriptEvent())
        return 0;

    void* resolved;

    switch (GetCurrentEventId() + kEventIdBias)
    {
    case 0x80233:
    {
        CScriptEventData* data = GetCurrentScriptEventData();
        if (!data)
            return 0;
        resolved = data->GetAbilityRefForTrainEvent();
        break;
    }

    case 0x80258:
    {
        CScriptEventData* data = GetCurrentScriptEventData();
        if (!data)
            return 0;
        resolved = data->ResolveSubjectUnit();
        break;
    }

    default:
        return 0;
    }

    CAgentRegistry* registry = (CAgentRegistry*)g_pItemSlotHost->GetSlotTable();
    return (int)registry->Register((CUnitAgent*)resolved, 0);
}

//----------------------------------------------------------------------------
//  0x6F3C2A60 - GetManipulatingUnit.  No leading guard.  ids
//  0x80230-0x80232 and 0x80265-0x80267, two 3-id groups sharing one
//  resolve-and-register arm each.
//----------------------------------------------------------------------------
int __cdecl GetManipulatingUnit()
{
    void* resolved;
    unsigned int id = GetCurrentEventId() + kEventIdBias;

    switch (id)
    {
    case 0x80230: case 0x80231: case 0x80232:
    {
        CScriptEventData* data = GetCurrentScriptEventData();
        if (!data)
            return 0;
        resolved = data->GetAbilityRefForTrainEvent();
        break;
    }

    case 0x80265: case 0x80266: case 0x80267:
    {
        CScriptEventData* data = GetCurrentScriptEventData();
        if (!data)
            return 0;
        resolved = data->ResolveSubjectUnit();
        break;
    }

    default:
        return 0;
    }

    CAgentRegistry* registry = (CAgentRegistry*)g_pItemSlotHost->GetSlotTable();
    return (int)registry->Register((CUnitAgent*)resolved, 0);
}

//----------------------------------------------------------------------------
//  0x6F3C2B00 - GetManipulatedItem.  Same two id groups as
//  GetManipulatingUnit, resolving the +0x44 and +0x2C refs instead.
//----------------------------------------------------------------------------
int __cdecl GetManipulatedItem()
{
    void* resolved;
    unsigned int id = GetCurrentEventId() + kEventIdBias;

    switch (id)
    {
    case 0x80230: case 0x80231: case 0x80232:
    {
        CScriptEventData* data = GetCurrentScriptEventData();
        if (!data)
            return 0;
        resolved = data->GetTargetRefForManipulateEvent();
        break;
    }

    case 0x80265: case 0x80266: case 0x80267:
    {
        CScriptEventData* data = GetCurrentScriptEventData();
        if (!data)
            return 0;
        resolved = data->GetAbilityRefForManipulateEvent();
        break;
    }

    default:
        return 0;
    }

    CAgentRegistry* registry = (CAgentRegistry*)g_pItemSlotHost->GetSlotTable();
    return (int)registry->Register((CUnitAgent*)resolved, 0);
}

//----------------------------------------------------------------------------
//  0x6F3C22F0 - GetRevivingUnit.  Leading guard present; a dense switch
//  over ids 0x8022C-0x8022E and 0x80251-0x80253, neither arm re-testing
//  GetCurrentScriptEventData()'s own result.
//----------------------------------------------------------------------------
int __cdecl GetRevivingUnit()
{
    if (!GetCurrentScriptEvent())
        return 0;

    void* resolved;

    switch (GetCurrentEventId() + kEventIdBias)
    {
    case 0x8022C: case 0x8022D: case 0x8022E:
        resolved = GetCurrentScriptEventData()->GetAbilityRefForTrainEvent();
        break;

    case 0x80251: case 0x80252: case 0x80253:
        resolved = GetCurrentScriptEventData()->ResolveSubjectUnit();
        break;

    default:
        return 0;
    }

    CAgentRegistry* registry = (CAgentRegistry*)g_pItemSlotHost->GetSlotTable();
    return (int)registry->Register((CUnitAgent*)resolved, 0);
}

//----------------------------------------------------------------------------
//  0x6F3C2560 - GetSellingUnit.  No leading guard.  ids 0x8030D/0x8030F/
//  0x80315 and 0x8031E/0x80320/0x80326, both arms re-testing
//  GetCurrentScriptEventData()'s own result.
//----------------------------------------------------------------------------
int __cdecl GetSellingUnit()
{
    void* resolved;

    switch (GetCurrentEventId() + kEventIdBias)
    {
    case 0x8030D: case 0x8030F: case 0x80315:
    {
        CScriptEventData* data = GetCurrentScriptEventData();
        if (!data)
            return 0;
        resolved = data->GetAbilityRefForTrainEvent();
        break;
    }

    case 0x8031E: case 0x80320: case 0x80326:
    {
        CScriptEventData* data = GetCurrentScriptEventData();
        if (!data)
            return 0;
        resolved = data->ResolveSubjectUnit();
        break;
    }

    default:
        return 0;
    }

    CAgentRegistry* registry = (CAgentRegistry*)g_pItemSlotHost->GetSlotTable();
    return (int)registry->Register((CUnitAgent*)resolved, 0);
}

//----------------------------------------------------------------------------
//  0x6F3C25F0 - GetSoldItem.  No leading guard.  ids 0x8030F/0x80315 and
//  0x80320/0x80326, both arms re-testing GetCurrentScriptEventData()'s own
//  result.
//----------------------------------------------------------------------------
int __cdecl GetSoldItem()
{
    void* resolved;

    switch (GetCurrentEventId() + kEventIdBias)
    {
    case 0x8030F: case 0x80315:
    {
        CScriptEventData* data = GetCurrentScriptEventData();
        if (!data)
            return 0;
        resolved = data->GetTargetRefForSellEvent();
        break;
    }

    case 0x80320: case 0x80326:
    {
        CScriptEventData* data = GetCurrentScriptEventData();
        if (!data)
            return 0;
        resolved = data->GetOrderRefForSellEvent();
        break;
    }

    default:
        return 0;
    }

    CAgentRegistry* registry = (CAgentRegistry*)g_pItemSlotHost->GetSlotTable();
    return (int)registry->Register((CUnitAgent*)resolved, 0);
}

//----------------------------------------------------------------------------
//  0x6F3C28F0 - GetSummoningUnit.  Leading guard present; both arms
//  re-test GetCurrentScriptEventData()'s own result.  ids 0x8022F
//  (summon) / 0x80254 (the first + 0x25).
//----------------------------------------------------------------------------
int __cdecl GetSummoningUnit()
{
    if (!GetCurrentScriptEvent())
        return 0;

    void* resolved;

    switch (GetCurrentEventId() + kEventIdBias)
    {
    case 0x8022F:
    {
        CScriptEventData* data = GetCurrentScriptEventData();
        if (!data)
            return 0;
        resolved = data->GetOrderRefForAttackEvent();
        break;
    }

    case 0x80254:
    {
        CScriptEventData* data = GetCurrentScriptEventData();
        if (!data)
            return 0;
        resolved = data->ResolveSubjectUnit();
        break;
    }

    default:
        return 0;
    }

    CAgentRegistry* registry = (CAgentRegistry*)g_pItemSlotHost->GetSlotTable();
    return (int)registry->Register((CUnitAgent*)resolved, 0);
}

//----------------------------------------------------------------------------
//  0x6F3C2950 - GetSummonedUnit.  Leading guard present; ids 0x8022F and
//  0x80254 both fall into the same single resolve arm (the shipped code's
//  second `jz` target and its sequential fallthrough land on the same
//  instruction), unlike its GetSummoningUnit twin above.
//----------------------------------------------------------------------------
int __cdecl GetSummonedUnit()
{
    if (!GetCurrentScriptEvent())
        return 0;

    unsigned int id = GetCurrentEventId() + kEventIdBias;
    if (id != 0x8022F && id != 0x80254)
        return 0;

    CScriptEventData* data = GetCurrentScriptEventData();
    if (!data)
        return 0;

    void* resolved = data->GetAbilityRefForTrainEvent();

    CAgentRegistry* registry = (CAgentRegistry*)g_pItemSlotHost->GetSlotTable();
    return (int)registry->Register((CUnitAgent*)resolved, 0);
}

//----------------------------------------------------------------------------
//  0x6F3C29A0 - GetTransportUnit.  Leading guard present; both arms
//  re-test GetCurrentScriptEventData()'s own result.  ids 0x80233
//  (transport) / 0x80258 (the first + 0x25).
//----------------------------------------------------------------------------
int __cdecl GetTransportUnit()
{
    if (!GetCurrentScriptEvent())
        return 0;

    void* resolved;

    switch (GetCurrentEventId() + kEventIdBias)
    {
    case 0x80233:
    {
        CScriptEventData* data = GetCurrentScriptEventData();
        if (!data)
            return 0;
        resolved = data->GetOrderRefForAttackEvent();
        break;
    }

    case 0x80258:
    {
        CScriptEventData* data = GetCurrentScriptEventData();
        if (!data)
            return 0;
        resolved = data->GetAbilityRefForTransportEvent();
        break;
    }

    default:
        return 0;
    }

    CAgentRegistry* registry = (CAgentRegistry*)g_pItemSlotHost->GetSlotTable();
    return (int)registry->Register((CUnitAgent*)resolved, 0);
}

//----------------------------------------------------------------------------
//  0x6F3C2210 - GetLearnedSkill.  Leading guard present, with an explicit
//  `xor eax,eax` fail (not the implicit-zero reuse most of this family
//  uses) shared by all three refusals.  ids 0x8022A (learn) / 0x8024F
//  (the first + 0x25); no agent registration, CScriptEventData's own
//  +0x44/+0x2C returned directly.
//----------------------------------------------------------------------------
int __cdecl GetLearnedSkill()
{
    if (!GetCurrentScriptEvent())
        return 0;

    switch (GetCurrentEventId() + kEventIdBias)
    {
    case 0x8022A:
    {
        CScriptEventData* data = GetCurrentScriptEventData();
        if (!data)
            return 0;
        return data->m_targetHandle;
    }

    case 0x8024F:
    {
        CScriptEventData* data = GetCurrentScriptEventData();
        if (!data)
            return 0;
        return data->m_abilityHandle;
    }

    default:
        return 0;
    }
}

//----------------------------------------------------------------------------
//  0x6F3C2870 - GetResearched.  A dense switch, no leading guard, no
//  agent registration: two id triples return CScriptEventData's own
//  +0x44/+0x2C fields directly.
//----------------------------------------------------------------------------
int __cdecl GetResearched()
{
    switch (GetCurrentEventId() + kEventIdBias)
    {
    case 0x80223: case 0x80224: case 0x80225:
    {
        CScriptEventData* data = GetCurrentScriptEventData();
        if (!data)
            return 0;
        return data->m_targetHandle;
    }

    case 0x80268: case 0x80269: case 0x8026A:
    {
        CScriptEventData* data = GetCurrentScriptEventData();
        if (!data)
            return 0;
        return data->m_abilityHandle;
    }

    default:
        return 0;
    }
}

//----------------------------------------------------------------------------
//  0x6F3C27B0 - GetBuyingUnit.  No leading guard.  A dense switch over
//  ids 0x8030D, 0x8030F/0x80315, 0x8031E and 0x80320/0x80326, every arm
//  re-testing GetCurrentScriptEventData()'s own result and sharing one
//  register tail.
//----------------------------------------------------------------------------
int __cdecl GetBuyingUnit()
{
    void* resolved;

    switch (GetCurrentEventId() + kEventIdBias)
    {
    case 0x8030D:
    {
        CScriptEventData* data = GetCurrentScriptEventData();
        if (!data)
            return 0;
        resolved = data->GetTargetRef();
        break;
    }

    case 0x8030F: case 0x80315:
    {
        CScriptEventData* data = GetCurrentScriptEventData();
        if (!data)
            return 0;
        resolved = data->GetOrderRefForAttackEvent();
        break;
    }

    case 0x8031E:
    {
        CScriptEventData* data = GetCurrentScriptEventData();
        if (!data)
            return 0;
        resolved = data->GetOrderRefForBuyEvent();
        break;
    }

    case 0x80320: case 0x80326:
    {
        CScriptEventData* data = GetCurrentScriptEventData();
        if (!data)
            return 0;
        resolved = data->GetAbilityRefForKillEvent();
        break;
    }

    default:
        return 0;
    }

    CAgentRegistry* registry = (CAgentRegistry*)g_pItemSlotHost->GetSlotTable();
    return (int)registry->Register((CUnitAgent*)resolved, 0);
}
