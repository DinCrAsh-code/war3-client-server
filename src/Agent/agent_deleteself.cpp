//============================================================================
//  0x6F0018A0 - CAgent's vtable slot 1, the scalar deleting destructor.
//
//  Its own translation unit, and that is the whole point of the file:
//  ~CAgent (agent_dtor.cpp, 0x6F0017F0) must *not* be inlined into it.  The
//  shipped body makes a real `call sub_6F0017F0` with no vtable stamp of
//  its own, because ~CAgent carries the __CxxFrameHandler3 frame
//  agent_dtor.cpp describes and MSVC will not inline a function with one.
//  This build has no such frame (/GS- /EHs-c-), so with the definition
//  visible it inlines the destructor happily and the function comes out
//  seventeen instructions against the shipped fifteen.  Split apart, only
//  the declaration is visible and the call survives.
//
//  The `this != 0` test is the source's own.  Every deleting destructor in
//  this chain whose base destructor *does* inline - CAgentWar3's,
//  CWar3Image's, CWidget's, CSelectable's (agentwar3_dtor.cpp) and
//  TRefCnt's (refcnt_deleteself.cpp) - has no such test in the shipped
//  stream, because the inlined destructor's first act is a store through
//  `this` and that proves the pointer non-null before the test is reached.
//  Here there is a call in the way and nothing to prove it, so the test
//  stays.  Do not "simplify" it away.
//============================================================================
#include "agent.h"

//  Storm.dll ordinal 403.
void __stdcall SMemFree(void* ptr, const char* logfilename, int logline,
                        unsigned int flags);

static const char kDelete[] = "delete";

CAgent* CAgent::DeleteSelf(int flags)
{
    this->CAgent::~CAgent();
    if ((flags & 1) && this != 0)
        SMemFree(this, kDelete, -1, 0);
    return this;
}
