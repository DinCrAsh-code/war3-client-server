//============================================================================
//  CAbility::~CAbility() - slot 1's own callee.  See ability.h.
//
//  Own translation unit, kept apart from CAbility::DeleteSelf
//  (ability_deleteself.cpp) for exactly the reason agent_dtor.cpp /
//  agent_deleteself.cpp are two files: the shipped DeleteSelf makes a
//  real out-of-line `call` into this destructor rather than inlining it,
//  and this destructor's own body is small enough that this toolchain's
//  optimiser inlines it into DeleteSelf the instant both are visible in
//  the same translation unit - a real, observable four-instruction cost
//  (DeleteSelf loses its `call` and gains the destructor's own three) if
//  the two are ever merged back together.
//============================================================================
#include "ability.h"

//----------------------------------------------------------------------------
//  0x6F038580.  Resets the vtable to CAgentWar3's own on the way out and
//  tail-calls into CAgent::~CAgent() - the same "every trivial destructor
//  between here and CAgent inlines away, one real store and one real
//  call survive" shape agentwar3_dtor.cpp's own CWidget/CSelectable pair
//  documents (CUnitRefList contributes no destructor of its own, so it
//  disappears the same way).  Wrapped in an `__except_handler4`-shaped SEH
//  frame in the shipped image - m_timer's own non-trivial destructor
//  (agenttimer.h) is enough on its own for MSVC to build one, the same
//  "one non-trivial member is enough" finding
//  destructable_dtor.cpp/agentwar3_dtor.cpp's own CDestructable note
//  already made - not reproducible under this build's fixed /GS- /EHs-c-
//  (docs/msvc-vc8-idioms.md, "An `__except_handler4`-shaped frame this
//  toolchain cannot reproduce"). The body past the frame furniture
//  matches: one member destructor call, one vtable store, one base call.
//----------------------------------------------------------------------------
//  Empty body: m_timer's own destructor call and the tail call into the
//  base chain are both the *implicit* per-member/per-base teardown every
//  C++ destructor gets for free - writing `m_timer.~CAgentTimer();`
//  explicitly here would run it a second time on top of that implicit
//  call, which is exactly the double-destruction the shipped body does
//  not have (its own dump calls CAgentTimer::~CAgentTimer exactly once).
CAbility::~CAbility()
{
}
