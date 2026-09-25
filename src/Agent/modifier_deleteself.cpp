//============================================================================
//  0x6F02F590 - CModifier's own slot-1 override, the scalar deleting
//  destructor. ~CModifier() is implicit and trivial, so it collapses into
//  CRelation's own base destructor one level up - same shape as every other
//  level of this chain (Agent/refcnt_deleteself.cpp,
//  Agent/presenceref_deleteself.cpp, Agent/agentfield_deleteself.cpp,
//  Agent/relation_deleteself.cpp).
//
//  Own translation unit - same "keep the destructor call a real out-of-line
//  call" reasoning as the rest of this chain.
//============================================================================
#include "modifier.h"

//  Storm.dll ordinal 403.
void __stdcall SMemFree(void* ptr, const char* logfilename, int logline,
                        unsigned int flags);

static const char kDelete[] = "delete";

CModifier* CModifier::DeleteSelf(int flags)
{
    this->CModifier::~CModifier();
    if (flags & 1)
        SMemFree(this, kDelete, -1, 0);
    return this;
}
