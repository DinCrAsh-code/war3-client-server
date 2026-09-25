//============================================================================
//  0x6F28E3C0 - CTaskActionFields::Init: fill a freshly made task in.
//
//  Four stores and one call.  The tracked target goes through
//  CAgent::SetTrackedTarget (agent.h), whose answer is thrown away - the
//  shipped code overwrites eax with -1 on the very next instruction, which
//  is the two link words being cleared out of one register.  Then the fixed
//  order id and the owner.
//
//  `this` stays in ecx across the call: the shipped body copies it into esi
//  before the call and reads nothing else out of ecx, so no reload is
//  needed afterwards.
//
//  Own translation unit: CUnit::FinishCurrentOrder calls it for real.
//============================================================================
#include "unitorder.h"
#include "agent.h"

void CTaskActionFields::Init(void* trackedTarget, void* owner)
{
    ((CAgent*)this)->SetTrackedTarget(trackedTarget);

    m_next.m_typeTag = -1;
    m_next.m_handle = 0xFFFFFFFF;
    m_orderId = kUnitOrderFinishTask;
    m_pOwner = owner;
}
