//============================================================================
//  See itemtypedassign.h.
//============================================================================
#include "itemtypedassign.h"
#include "itemhandletable.h"
#include "agiletype.h"
#include <excpt.h>   // AbnormalTermination(), for the converting constructor's __finally

//----------------------------------------------------------------------------
//  0x6F038890.  Same __try/__finally shape as CAgentPtr::CAgentPtr(CAgent*)
//  (agent.cpp) and for the identical reason: Assign's vtable call through
//  candidate is the one thing here the compiler cannot prove won't raise a
//  real structured exception, so a __try/__finally wraps it, releasing
//  whatever half-assigned value is left behind if it does not return
//  normally.  m_value is zeroed first so Release() has something
//  well-defined to test even if the exception fires before Assign's own
//  store.
//----------------------------------------------------------------------------
SCheckedItemSlot::SCheckedItemSlot(CAgent* candidate)
{
    m_value = 0;
    __try
    {
        Assign(candidate);
    }
    __finally
    {
        if (AbnormalTermination())
            Release();
    }
}

//----------------------------------------------------------------------------
//  0x6F02A6A0.
//----------------------------------------------------------------------------
SCheckedItemSlot* SCheckedItemSlot::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), GetItemTypeFourCC()))
        checked = candidate;
    else
        checked = 0;

    CAgent* old = m_value;
    if (old != checked)
    {
        if (old)
        {
            if (--old->m_refcount == 0)
                old->ReleaseSelf();
        }
        if (checked)
        {
            if (candidate)
                candidate->m_refcount++;
        }
        m_value = checked;
    }

    return this;
}
