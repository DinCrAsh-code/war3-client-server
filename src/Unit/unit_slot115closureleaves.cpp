//============================================================================
//  Three depth-1/2 closure leaves reached from CUnit's own vtable slots
//  115-117 (Method_0x1CC/0x1D0/0x1D4, unit_slotthunks.cpp), all three
//  permanently SEH-blocked at the root - see docs/targets/CUnit__vtable.md.
//  Reconstructing these does not un-block any of the three slots; they are
//  tracked here because cunit_vtable_closure_worklist.json's own closure
//  walk reaches them and none needs a guessed struct layout.
//============================================================================
#include "unit.h"
#include "storm.h"              // TSGrowableArray
#include "unitsubgrouprefresh.h" // CAgentPtr

//  Forward-declared to match the definition unitsubgrouprefresh.cpp already
//  provides (the same class, same one method, no body here) - see this
//  repo's own precedent for a class split across TUs this way
//  (SCUnitBlockedClosureLeaves et al.).
struct SAbilityAgentCollector
{
    void CollectAgents(TSGrowableArray<CAgentPtr>* out);
};

//----------------------------------------------------------------------------
//  0x6F41F9A0 (`retn 0`) - the raw handle half of the order queue's own
//  tail entry (m_ref1A8, see unitorderappend.cpp's own SOptionalHandleRef
//  cast).  No struct guess involved: it is the established field, read
//  raw rather than through SOptionalHandleRef's own accessor because
//  nothing else in this call tree needs the type reinterpreted.
//----------------------------------------------------------------------------
unsigned int CUnit::GetOrderQueueTailHandleRaw()
{
    return m_ref1A8.m_t;
}

//----------------------------------------------------------------------------
//  0x6F421E30/0x6F421EF0 (`retn 4` each) - /O2's tail call for
//  `((SAbilityAgentCollector*)((char*)this + N))->CollectAgents(out)`, the
//  same "add ecx, N / jmp <method>" shape unit.h's own +0x164 sub-object
//  note documents, on two different fixed offsets rather than the
//  movement sub-object.  Neither offset has an established type beyond
//  "carries a SAbilityAgentCollector" - genuinely unnamed past that, so
//  the method keeps an address-derived name rather than guessing what the
//  rest of the sub-object at +0x1E8/+0x1C4 is for.
//----------------------------------------------------------------------------
void CUnit::CollectAbilityAgentsAt1E8(TSGrowableArray<CAgentPtr>* out)
{
    ((SAbilityAgentCollector*)((char*)this + 0x1E8))->CollectAgents(out);
}

void CUnit::CollectAbilityAgentsAt1C4(TSGrowableArray<CAgentPtr>* out)
{
    ((SAbilityAgentCollector*)((char*)this + 0x1C4))->CollectAgents(out);
}
