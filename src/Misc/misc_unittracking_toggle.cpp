//============================================================================
//  0x6F309550 - add or remove a candidate unit from a small growable
//  membership list, gated on the candidate being locally owned and carrying
//  classification bit 0x40 of its own +0x248.  `this` in ecx, two stack
//  arguments (`retn 8`): the candidate unit and a non-zero "remove" flag
//  (zero means "ensure present").
//
//  The one caller in this tree (UnitCreationNotifyB_6F333C10,
//  Jass/jassnatives_thunks.cpp) reaches this address with `ecx` =
//  CGameUI::GetCamera()'s own return value, after using *that* object's own
//  +0x0C/+0x10 handle/type pair to resolve and gate-check an unrelated
//  '+agl'-tagged object first.  That would make `this` a CCameraWar3*, and
//  this function's own +0xC0/+0xC4/+0xBC arithmetic is exactly the
//  TSGrowableArray<T> layout (alloc@0/count@4/data@8) - except its own
//  New()/Assign call targets (0x6F0A4730/0x6F02A560) are the CUnit-typed
//  TSGrowableArray<SCheckedUnitSlot> instantiation
//  (Containers/unitagentptrarray.cpp), while GameUI/cameradestructor.cpp
//  independently and directly confirms (off that destructor's own raw
//  bytes) that CCameraWar3's *own* +0xBC is a different, untyped
//  TSGrowableArray<CAgentPtr>, torn down through 0x6F286590 - not this
//  function's New()/Assign pair at all.  That contradiction is flagged
//  here rather than papered over by asserting CCameraWar3; `this` is
//  written out as its own, unidentified receiver with only the one offset
//  this function itself touches, the same way SDualHandleRefHost6F2E37E0
//  (Misc/cunit_agent8_batch2.cpp) leaves its own receiver unnamed.
//
//  Own translation unit: a homeless leaf whose owning class is not
//  established (src/README.md's "Misc/ is a real answer, not a failure").
//============================================================================
#include "widget.h"
#include "agenttypedslots.h"

//  Re-declared to match Widget/cunit_agent3_islocallyowned.cpp's own
//  definition exactly, so this call binds to that body instead of inventing
//  a second symbol (tools/link_check.py's first defect class).
struct CWidgetLocalOwnerCheck : CWidget
{
    int IsOwnedByLocalPlayer();
};

//  Only the one member this function reads on the candidate.
struct SUnitFlag248Host
{
    char         m_reserved00[0x248];
    unsigned int m_flags248;   // +0x248, bit 0x40 gates this function
};

struct SUnitTrackingArrayHost
{
    char                                m_reserved00[0xBC];
    TSGrowableArray<SCheckedUnitSlot>   m_trackedUnits;   // +0xBC

    void __thiscall UpdateTrackedUnitMembership_6F309550(void*, int);
};

void __thiscall SUnitTrackingArrayHost::UpdateTrackedUnitMembership_6F309550(
    void* candidate, int removeFlag)
{
    int owned = ((CWidgetLocalOwnerCheck*)candidate)->IsOwnedByLocalPlayer();
    if (!owned)
        return;
    if (candidate == 0)
        return;
    if (!(((SUnitFlag248Host*)candidate)->m_flags248 & 0x40))
        return;

    if (removeFlag != 0)
    {
        for (int i = (int)m_trackedUnits.m_count - 1; i >= 0; i--)
        {
            SCheckedUnitSlot* slot = &m_trackedUnits.m_data[i];
            if (slot->m_value == (CAgent*)candidate)
            {
                slot->Release();
                slot->m_value = 0;
                return;
            }
        }
        return;
    }

    unsigned int count = m_trackedUnits.m_count;
    if (count != 0)
    {
        SCheckedUnitSlot* data = m_trackedUnits.m_data;
        int freeIndex = -1;
        for (int i = (int)count - 1; i >= 0; i--)
        {
            if (data[i].m_value == 0)
                freeIndex = i;
            if (data[i].m_value == (CAgent*)candidate)
                return;
        }
        if (freeIndex != -1)
        {
            data[freeIndex].Assign((CAgent*)candidate);
            return;
        }
    }

    m_trackedUnits.New()->Assign((CAgent*)candidate);
}
