//============================================================================
//  0x6F051CA0 - CBulletBase's vtable slot 1, the scalar deleting destructor.
//  Own translation unit, same reasoning as agent_deleteself.cpp: only
//  ~CBulletBase's declaration is visible here, so the `call` into it
//  survives instead of collapsing in (this build has no SEH frame to force
//  the split the way ~CAgent's own does, so the split has to be done by
//  hand, same as CAgentWar3/CWar3Image's own single-store destructors
//  would need if either were ever called through a real DeleteSelf).
//============================================================================
#include "bulletbase.h"

static const char kDelete[] = "delete";
void  __stdcall SMemFree(void* ptr, const char* logfilename,
                         int logline, unsigned int flags);

CBulletBase* CBulletBase::DeleteSelf(int flags)
{
    this->CBulletBase::~CBulletBase();
    if ((flags & 1) && this != 0)
        SMemFree(this, kDelete, -1, 0);
    return this;
}
