//============================================================================
//  0x6F40B620 and 0x6F40B640 - the event-data ref at +0x44 of whatever the
//  event's own owner ref names.  Two identical bodies 0x20 apart, one per
//  native family; see jassevents.h for why they are two classes here.
//
//  The "not set" test is inlined rather than a call to
//  QueryHandleField0x54IfSet (0x6F021A00, handleref_field54_guarded.cpp):
//  the shipped bodies test the pair themselves and *tail jump* into
//  QueryHandleField0x54, which is what writing the guard out here produces.
//
//  Five more addresses in this same 0x6F40B6xx module, added with the
//  event-unit-getter family (jasseventunitgetters.cpp): three more
//  duplicates of CScriptEventData::GetAbility/GetOrder's own body
//  (eventfieldresolve.cpp has the other five, in the 0x6F26FCxx module),
//  and the first two addresses that reach CScriptEventData's third
//  optional ref, at +0x44/+0x48 - a field neither GetAbility nor GetOrder
//  already names.
//============================================================================
#include "jassevents.h"

void* SOrderEventOwner::GetEventData()
{
    if ((m_eventDataTypeTag & m_eventDataHandle) == -1)
        return 0;

    return QueryHandleField0x54((SOptionalHandleRef*)&m_eventDataHandle);
}

void* SSpellEventOwner::GetEventData()
{
    if ((m_eventDataTypeTag & m_eventDataHandle) == -1)
        return 0;

    return QueryHandleField0x54((SOptionalHandleRef*)&m_eventDataHandle);
}

//----------------------------------------------------------------------------
//  0x6F40B6A0 - the +0x2C ref.  GetLearningUnit/GetLevelingUnit/
//  GetSummonedUnit/GetChangingUnit's first id each, GetLoadedUnit's and
//  GetManipulatingUnit's own switch cases.
//----------------------------------------------------------------------------
void* CScriptEventData::GetAbilityRefForTrainEvent()
{
    if ((m_abilityTypeTag & m_abilityHandle) == -1)
        return 0;

    return QueryHandleField0x54((SOptionalHandleRef*)&m_abilityHandle);
}

//----------------------------------------------------------------------------
//  0x6F40B6C0 - the +0x38 ref.  GetAttacker/GetKillingUnit/GetRescuer's
//  first id range, GetBuyingUnit's second case, GetSummoningUnit's and
//  GetTransportUnit's first id, GetTrainedUnitType's own switch.
//----------------------------------------------------------------------------
void* CScriptEventData::GetOrderRefForAttackEvent()
{
    if ((m_orderTypeTag & m_orderHandle) == -1)
        return 0;

    return QueryHandleField0x54((SOptionalHandleRef*)&m_orderHandle);
}

//----------------------------------------------------------------------------
//  0x6F40B6E0 - the +0x44 ref.  GetBuyingUnit's first case; the closest
//  thing this batch has to a canonical use of the field.
//----------------------------------------------------------------------------
void* CScriptEventData::GetTargetRef()
{
    if ((m_targetTypeTag & m_targetHandle) == -1)
        return 0;

    return QueryHandleField0x54((SOptionalHandleRef*)&m_targetHandle);
}

//----------------------------------------------------------------------------
//  0x6F40B700 - the +0x44 ref.  GetSoldItem's first case.
//----------------------------------------------------------------------------
void* CScriptEventData::GetTargetRefForSellEvent()
{
    if ((m_targetTypeTag & m_targetHandle) == -1)
        return 0;

    return QueryHandleField0x54((SOptionalHandleRef*)&m_targetHandle);
}

//----------------------------------------------------------------------------
//  0x6F40B720 - the +0x44 ref.  GetManipulatedItem's first case.
//----------------------------------------------------------------------------
void* CScriptEventData::GetTargetRefForManipulateEvent()
{
    if ((m_targetTypeTag & m_targetHandle) == -1)
        return 0;

    return QueryHandleField0x54((SOptionalHandleRef*)&m_targetHandle);
}
