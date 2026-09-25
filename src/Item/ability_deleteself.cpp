//============================================================================
//  0x6F052A10 - CAbility's vtable slot 1, the scalar deleting destructor.
//  See ability.h.
//
//  Own translation unit, apart from CAbility::~CAbility() itself
//  (ability_dtor.cpp) - see that file's header for why the split matters
//  here.
//============================================================================
#include "ability.h"

//  Storm.dll ordinal 403.
void __stdcall SMemFree(void* ptr, const char* logfilename, int logline,
                        unsigned int flags);

static const char kDelete[] = "delete";

//----------------------------------------------------------------------------
//  Covariant return, same shape as every other scalar deleting destructor
//  in this family (agent_deleteself.cpp).
//----------------------------------------------------------------------------
CAbility* CAbility::DeleteSelf(int flags)
{
    this->CAbility::~CAbility();
    if ((flags & 1) && this != 0)
        SMemFree(this, kDelete, -1, 0);
    return this;
}
