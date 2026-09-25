//============================================================================
//  Five duplicate copies of CScriptEventData::GetAbility/GetOrder's own
//  eight-instruction body (scripteventpoint.cpp, scripteventorder.cpp),
//  one module: 0x6F26FCxx.  The image is linked without /OPT:ICF
//  (agentdefaults.cpp), so an identical body re-emitted at a distinct
//  address stays a distinct symbol - and each is reached from a different
//  spread of event ids in jasseventunitgetters.cpp, so each keeps its own
//  name rather than aliasing GetAbility/GetOrder.
//
//  All five: same "not set" test (`(handle & typeTag) == -1`), same tail
//  jump into QueryHandleField0x54, same "form this+offset first, shuffle
//  this into eax" spelling GetAbility/GetOrder already score 5 of 8
//  against - see those two functions' own funcmap.py BEHAVIOUR entries for
//  why the shipped body's own instruction order cannot be reproduced from
//  source.
//============================================================================
#include "jassevents.h"

//----------------------------------------------------------------------------
//  0x6F26FC10 - the +0x2C ref.  GetAttacker/GetKillingUnit/GetRescuer's
//  "kill" id range, GetTrainedUnitType's own switch, GetBuyingUnit's last
//  case.
//----------------------------------------------------------------------------
void* CScriptEventData::GetAbilityRefForKillEvent()
{
    if ((m_abilityTypeTag & m_abilityHandle) == -1)
        return 0;

    return QueryHandleField0x54((SOptionalHandleRef*)&m_abilityHandle);
}

//----------------------------------------------------------------------------
//  0x6F26FC30 - the +0x38 ref.  GetBuyingUnit's own "buy" case.
//----------------------------------------------------------------------------
void* CScriptEventData::GetOrderRefForBuyEvent()
{
    if ((m_orderTypeTag & m_orderHandle) == -1)
        return 0;

    return QueryHandleField0x54((SOptionalHandleRef*)&m_orderHandle);
}

//----------------------------------------------------------------------------
//  0x6F26FC50 - the +0x38 ref.  GetSoldItem's second case.
//----------------------------------------------------------------------------
void* CScriptEventData::GetOrderRefForSellEvent()
{
    if ((m_orderTypeTag & m_orderHandle) == -1)
        return 0;

    return QueryHandleField0x54((SOptionalHandleRef*)&m_orderHandle);
}

//----------------------------------------------------------------------------
//  0x6F26FC70 - the +0x2C ref.  GetManipulatedItem's second case.
//----------------------------------------------------------------------------
void* CScriptEventData::GetAbilityRefForManipulateEvent()
{
    if ((m_abilityTypeTag & m_abilityHandle) == -1)
        return 0;

    return QueryHandleField0x54((SOptionalHandleRef*)&m_abilityHandle);
}

//----------------------------------------------------------------------------
//  0x6F26FC90 - the +0x2C ref.  GetTransportUnit's second id.
//----------------------------------------------------------------------------
void* CScriptEventData::GetAbilityRefForTransportEvent()
{
    if ((m_abilityTypeTag & m_abilityHandle) == -1)
        return 0;

    return QueryHandleField0x54((SOptionalHandleRef*)&m_abilityHandle);
}
