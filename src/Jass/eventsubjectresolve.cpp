//============================================================================
//  0x6F285F00 - CScriptEventData::ResolveSubjectUnit.
//
//  The same "resolve an embedded SOptionalHandleRef through
//  QueryHandleField0x54, then hand the answer to SCheckedUnitSlot's own
//  converting constructor and return its m_value" shape
//  Unit/cunit_agent4_optionalrefresolveunit.cpp's ResolveOptionalUnitRef
//  already establishes for this family, plus one extra step: whatever
//  QueryHandleField0x54 hands back is revalidated a second time through
//  IsAgentHandleLive (Agent/agenthandlelive.cpp) - the same "handle still
//  resolves, still carries '+agl', still not busy" check that function is
//  - before it is trusted as the slot's candidate.  A first-resolve success
//  that fails the second check falls back to a *second*, always-null
//  SCheckedUnitSlot rather than reusing the first slot's own (already
//  invalid) value, which is what the shipped body's two constructor calls
//  and the `bl` flag selecting which one's `m_value` to read are: two
//  scoped locals, only one of which is ever actually assigned, both
//  destructed (via each one's own inlined Release()) before the function
//  returns.
//
//  Own translation unit purely because /Ob2 would otherwise inline this
//  into whichever native happened to be compiled next to it; every native
//  that calls it issues a genuine `call`.
//============================================================================
#include "jassevents.h"
#include "agenttypedslots.h"
#include "game.h"

//  0x6F0418A0 - Agent/agenthandlelive.cpp.
extern int __fastcall IsAgentHandleLive(const void* agent);

void* CScriptEventData::ResolveSubjectUnit()
{
    void* candidate = 0;
    if ((m_ownerRef.m_typeTag & m_ownerRef.m_handle) != -1)
        candidate = QueryHandleField0x54(&m_ownerRef);

    SCheckedUnitSlot resolved((CAgent*)candidate);

    void* result;
    if (resolved.m_value && IsAgentHandleLive(resolved.m_value))
    {
        result = resolved.m_value;
    }
    else
    {
        SCheckedUnitSlot empty(0);
        result = empty.m_value;
    }

    return result;
}
