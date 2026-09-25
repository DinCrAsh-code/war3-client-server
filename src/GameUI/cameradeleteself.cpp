//============================================================================
//  0x6F3085C0 - CCameraWar3::DeleteSelf, overriding CAgent's own slot 1.
//  `retn 4`.
//
//  Byte for byte the same shape as CAgent::DeleteSelf itself
//  (Agent/agent_deleteself.cpp): run the destructor, then free `this` when
//  the caller asked for it (`flags & 1`) and the pointer survived the
//  destructor call.  Own translation unit for the same reason
//  agent_deleteself.cpp is: ~CCameraWar3 (0x6F308340) carries a real
//  __CxxFrameHandler3 frame (see cameravtablethunks.cpp - the frame is why
//  it stays thunked this session) and must not be inlined here, which a
//  visible definition in the same TU would invite even under this build's
//  fixed /GS- /EHs-c-.
//
//  Was one of CCameraWar3's nine naked-thunk vtable overrides
//  (cameravtablethunks.cpp); wired in for real now that the dump for this
//  slot has arrived.
//============================================================================
#include "gameui.h"
#include "storm.h"

static const char kDelete[] = "delete";

CAgent* CCameraWar3::DeleteSelf(int flags)
{
    this->CCameraWar3::~CCameraWar3();
    if ((flags & 1) && this != 0)
        SMemFree(this, kDelete, -1, 0);
    return this;
}
