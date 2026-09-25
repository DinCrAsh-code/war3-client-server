//============================================================================
//  0x6F0017F0 - CAgent::~CAgent().  See agent.h.
//
//  Own translation unit, same reasoning as agent_ctor.cpp: this address
//  sits in the same low 0x6F00xxxx/0x6F001xxx neighbourhood as
//  CAgent::CAgent() (0x6F001730) and TRefCnt::~TRefCnt() (0x6F00C070)
//  rather than agent.cpp's own 0x6F4Cxxxx cluster.
//
//  Same SEH-frame mismatch documented in docs/msvc-vc8-idioms.md for every
//  other CAgent/CObserver constructor/destructor in this family: a base
//  class (CObserver) with its own destructor to run after a call the
//  compiler cannot prove is noexcept is enough on its own for MSVC to
//  build the cookie-XORed, frame-pointer-omitted __CxxFrameHandler3 frame
//  the dump shows, with no explicit __try/__finally in the source.  Not
//  reproducible under this build's fixed /GS- /EHs-c-; the body past the
//  frame furniture (own vtable reset, embedded-member destructor call,
//  implicit base destructor call) matches instruction for instruction.
//============================================================================
#include "agent.h"


CAgent::~CAgent()
{
    ((CObserver*)&m_selfObserverStorage[0])->~CObserver();
}


//============================================================================
//  The rest of this address run: 0x6F001850..0x6F0018A0, five slots that
//  sit immediately after ~CAgent and belong to the same module.
//============================================================================

//----------------------------------------------------------------------------
//  0x6F001850 / 0x6F001860 / 0x6F001870 / 0x6F001880 - slots 16, 17, 18 and
//  19.  All four are bare `retn`s in the shipped image; the argument counts
//  come from each one's own `retn` (0, 4, 4, 4).  CAgentWar3 overrides
//  three of them with real bodies (agentwar3_slots.cpp), so these are the
//  do-nothing defaults an agent with no flags word and no sprite keeps.
//----------------------------------------------------------------------------
void CAgent::RefreshOwningPlayerColor() {}
void CAgent::DumpState(void*) {}
void CAgent::SaveFlags(CDataStoreScratch*) {}
void CAgent::LoadFlags(CDataStore*) {}

//----------------------------------------------------------------------------
//  0x6F001890 - slot 22.  `mov eax, offset aCagent` and nothing else.
//----------------------------------------------------------------------------
const char* CAgent::GetClassName()
{
    return "CAgent";
}
