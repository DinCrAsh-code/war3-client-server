//============================================================================
//  CCreepSpot's own two members from this closure slice.  The class name is
//  not a guess: dword_6FAB4EC0's own allocation tag names the element type
//  `CAgentPtr<CCreepSpot>` (creepcampregistry.cpp), and CCreepSpot is the
//  receiver `this` both of these bind to.
//
//  Both operate on the same 12-slot occupant table at +0x2C (0x0C bytes
//  per slot - the first 8 of which are an SOptionalHandleRef, +0x24 the
//  live occupant count) but are not the same shape at all once the loop
//  body is read closely, so each gets its own header below rather than
//  one shared account.
//============================================================================
#include "agent.h"
#include "agiletype.h"
#include "game.h"                //  CHandleObject, LookupHandle
#include "itemhandleresolve.h"   //  SOptionalHandleRef, QueryHandleField0x54
#include "agentregistry.h"       //  kAgileHandleTag
#include "widget.h"
#include "missile_thunderbolt.h" //  CMissileThunderBoltRefs::SetObserverRegistration

//  0x6F2AB3E0 - the real sibling registration entry point, reconstructed by
//  a different parallel agent as a standalone leaf (Misc/misc_notifyorforward_
//  event.cpp, EXACT) rather than as a CMissileThunderBoltRefs member; called
//  through this locally-redeclared dispatch struct, same convention as that
//  file's own Misc/cunit_agent8_batch2.cpp caller.
struct SEventNotifyOrForwardHost
{
    void NotifyOrForwardEvent(void* target, int flag);
};

//  One occupant slot: an SOptionalHandleRef plus one untouched trailing
//  dword neither member below reads or writes.
struct SCreepSpotOccupant
{
    SOptionalHandleRef m_ref;          // +0x00 (m_handle/+0x00, m_typeTag/+0x04)
    unsigned int        m_reserved08;  // +0x08
};

struct CCreepSpot
{
    char                m_reserved00[0x24];
    unsigned int        m_occupantCount;      // +0x24
    char                m_reserved28[0x2C - 0x28];
    SCreepSpotOccupant  m_occupants[12];       // +0x2C

    void AddOccupant(void* unit);              //  0x6F2E1AC0
    CAgent* FindOccupantOfType();               //  0x6F2E1B70
};

//  0x6F26C1C0 - out of this group's own worklist slice, but already a
//  separately-reconstructed EXACT leaf under its own name
//  (Misc/misc_rawcode_getters.cpp): a fixed rawcode getter ('u3w+',
//  0x2B773375), the same family as GetFourCC_6F24F510 etc.
//  FindOccupantOfType below is its one caller here.
extern unsigned int GetUnitTypeFourCC();

//----------------------------------------------------------------------------
//  0x6F2E1AC0 - CCreepSpot::AddOccupant.  Tell `unit` about this spot
//  through its own two observer-registration slots
//  (NotifyOrForwardEvent then CWidget::PostAgentEvent2, both already
//  reconstructed elsewhere and reused as-is), then find the first of the
//  12 occupant slots whose own SOptionalHandleRef either reads {-1,-1}
//  (never used) or no longer resolves through QueryHandleField0x54 (a
//  stale occupant), and write `unit`'s own handle pair into it - or, if
//  `unit` is null or its own LookupHandle(+0x0C, +0x10) does not resolve
//  to a `kAgileHandleTag`-kinded object, write the {-1,-1} empty sentinel
//  instead.  Either way, +0x24 (the occupant count) is bumped.  If no
//  slot is free (all 12 resolve), the function is a no-op.
//
//  `unit`'s own m_handle/m_typeTag are read at +0x0C/+0x10 - a different
//  pair of offsets than CAgentField's own +0x08/+0x0C (agentfield_core.cpp),
//  so `unit` is genuinely a different, unnamed class here; kept as a raw
//  `void*` with raw offsets rather than guessed at.
//----------------------------------------------------------------------------
void CCreepSpot::AddOccupant(void* unit)
{
    ((SEventNotifyOrForwardHost*)unit)->NotifyOrForwardEvent(this, 1);
    ((CWidget*)unit)->PostAgentEvent2(this, 1);

    unsigned int i;
    for (i = 0; i < 12; i++)
    {
        SCreepSpotOccupant* slot = &m_occupants[i];
        if ((slot->m_ref.m_handle & slot->m_ref.m_typeTag) == (unsigned int)-1)
            break;
        if (QueryHandleField0x54(&slot->m_ref) == 0)
            break;
    }

    if (i == 12)
        return;

    SCreepSpotOccupant* slot = &m_occupants[i];

    CHandleObject* resolved = 0;
    if (unit)
    {
        int typeTag = *(int*)((char*)unit + 0x10);
        unsigned int handle = *(unsigned int*)((char*)unit + 0x0C);
        CHandleObject* obj = LookupHandle(handle, typeTag);
        if (obj)
        {
            unsigned int mask = (unsigned int)(obj->m_kindTag != (int)kAgileHandleTag) - 1;
            resolved = (CHandleObject*)((unsigned int)obj & mask);
        }
    }

    if (resolved)
    {
        slot->m_ref.m_handle = resolved->m_flags;
        slot->m_ref.m_typeTag = resolved->m_typeTag;
    }
    else
    {
        slot->m_ref.m_handle = (unsigned int)-1;
        slot->m_ref.m_typeTag = -1;
    }

    m_occupantCount++;
}

//----------------------------------------------------------------------------
//  0x6F2E1B70 - CCreepSpot::FindOccupantOfType.  Walk the same 12 slots
//  looking for one that (a) resolves through QueryHandleField0x54 and
//  (b) whose resolved CAgent's own GetAgileTypeId() is derived from
//  whatever GetUnitTypeFourCC() (0x6F26C1C0) answers, and return
//  that CAgent* - transiently addref'd and immediately released again
//  (net no ownership change), the same borrow-without-owning shape a
//  caller that only wants the pointer value, not a held reference, gets
//  elsewhere in this codebase.  Returns null if none of the 12 match.
//----------------------------------------------------------------------------
CAgent* CCreepSpot::FindOccupantOfType()
{
    CAgent* found = 0;

    for (unsigned int i = 0; i < 12; i++)
    {
        SCreepSpotOccupant* slot = &m_occupants[i];

        CAgent* candidate;
        if ((slot->m_ref.m_handle & slot->m_ref.m_typeTag) == (unsigned int)-1)
            candidate = 0;
        else
            candidate = (CAgent*)QueryHandleField0x54(&slot->m_ref);

        if (candidate)
        {
            unsigned int wanted = GetUnitTypeFourCC();
            if (AgileTypeIsDerivedFrom(candidate->GetAgileTypeId(), wanted))
            {
                candidate->m_refcount++;
                found = candidate;
            }
        }

        if (found)
            break;
    }

    if (found)
    {
        if (--found->m_refcount == 0)
            found->ReleaseSelf();
    }

    return found;
}
