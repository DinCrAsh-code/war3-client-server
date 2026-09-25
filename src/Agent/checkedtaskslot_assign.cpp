//============================================================================
//  0x6F0DF440 - SCheckedTaskSlot::Assign.  See agenttypedslots.h.
//
//  Same shape as SCheckedTaskActionSlot::Assign (taskactiontypedassign.cpp)
//  and every other member of the type-checked-slot family: checked here
//  against CTask's own base agile type id ('task', GetTaskAgileTypeId,
//  0x6F2717C0, misc_rawcode_getters.cpp) rather than a derived leaf's.  The
//  required id arrives from a real call, not an immediate, so - like the
//  'tskA'/'+w3d' members - it has to happen before the candidate's own
//  virtual slot 7 read, which is what MSVC's right-to-left argument
//  evaluation for AgileTypeIsDerivedFrom gives for free.
//
//  0x6F0E7470 - the converting constructor: a later BFS pass found this
//  address (agenttypedslots.h's own comment used to say "no converting
//  constructor for this one in this dump" - it just had not been reached
//  yet).  Same three statements as SCheckedSlot_6F0E7E30's own ctor
//  (cunit_agent6_convertingctors.cpp) and every other scoped-local member
//  of the family: zero the value, `Assign` inside a `__try`, `Release` in
//  a `__finally` guarded by `AbnormalTermination()`.
//============================================================================
#include "agenttypedslots.h"
#include "agiletype.h"
#include <excpt.h>   // AbnormalTermination()

unsigned int GetTaskAgileTypeId();   //  0x6F2717C0 - 'task'.

SCheckedTaskSlot* SCheckedTaskSlot::Assign(CAgent* candidate)
{
    CAgent* checked;
    if (candidate
        && AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(),
                                  GetTaskAgileTypeId()))
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

SCheckedTaskSlot::SCheckedTaskSlot(CAgent* candidate)
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
