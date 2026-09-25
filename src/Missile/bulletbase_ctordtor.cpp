//============================================================================
//  0x6F051370 / 0x6F03E800 / 0x6F051CA0 - CBulletBase's own construction and
//  destruction triplet.  See bulletbase.h for the field layout the
//  constructor establishes.
//
//  Real dtor (0x6F03E800) kept in its own translation unit for the same
//  reason agent_deleteself.cpp keeps ~CAgent() out of DeleteSelf: with the
//  definition visible, this build's /GS- /EHs-c- would happily inline it
//  into DeleteSelf below, and the shipped stream keeps them as two
//  functions with a real `call` between them (no SEH frame here to force
//  that the way ~CAgent's own split is forced - this one is just the
//  shipped module boundary, 0x6F03Exxx vs 0x6F051xxx).
//============================================================================
#include "bulletbase.h"

//----------------------------------------------------------------------------
//  0x6F051370.  Base chain (CAgent::CAgent, then CAgentWar3's/CWar3Image's
//  own m_flags/m_reserved24/m_pSprite zero stores, fully inlined - see
//  war3image.h) runs first automatically; every member below default-
//  constructs in declaration order, which is layout order.
//----------------------------------------------------------------------------
CBulletBase::CBulletBase()
    : m_targetHandle(0xFFFFFFFFu), m_targetTypeTag(-1),
      m_reserved34(0), m_fieldA(0), m_fieldB(0),
      m_fieldC(0), m_fieldD(0), m_fieldE(0)
{
}

//----------------------------------------------------------------------------
//  0x6F03E800.  Empty body: m_timer's own real destructor runs for real
//  (CAgentTimer::~CAgentTimer -> Cancel(), agenttimer.cpp), the three
//  embedded FloatMinis' trivial virtual destructors elide to nothing (same
//  reasoning as the constructor's collapsed vtable stamp), and the compiler
//  auto-chains into CWar3Image's/CAgentWar3's own trivial destructors
//  (eliding down to a single restamp, `??_7CAgentWar3@@6B@`) and then
//  CAgent::~CAgent() (0x6F0017F0, agent_dtor.cpp) for real.
//----------------------------------------------------------------------------
CBulletBase::~CBulletBase()
{
}
