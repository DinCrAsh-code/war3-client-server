//============================================================================
//  0x6F4196F0 - CPlayerWar3's vtable slot 1, the scalar deleting
//  destructor.
//
//  Own translation unit, same reasoning agent_deleteself.cpp gives:
//  ~CPlayerWar3 (playerwar3_dtor.cpp, 0x6F418A60) carries the
//  __CxxFrameHandler3 frame docs/msvc-vc8-idioms.md describes, and MSVC
//  will not inline a function with one - split apart, only the
//  declaration is visible here and the real `call` survives.
//
//  Same shape as CAgent::DeleteSelf (agent_deleteself.cpp) and
//  CItem::DeleteSelf (abilityinterfaced_deleteself.cpp), including the
//  explicit `this != 0` test the dump itself carries (the call in the way
//  means the compiler cannot prove non-null from an inlined store the way
//  the whole-inlined siblings can).
//============================================================================
#include "playerwar3.h"
#include "storm.h"

static const char kDelete[] = "delete";

CPlayerWar3* CPlayerWar3::DeleteSelf(int flags)
{
    this->CPlayerWar3::~CPlayerWar3();
    if ((flags & 1) && this != 0)
        SMemFree(this, kDelete, -1, 0);
    return this;
}
