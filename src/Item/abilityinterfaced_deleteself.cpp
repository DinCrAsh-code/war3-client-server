//============================================================================
//  0x6F0539F0 - CAbilityInterfaced's vtable slot 1, the scalar deleting
//  destructor.  See abilityinterfaced.h.
//
//  Same shape as CAbility::DeleteSelf (ability_deleteself.cpp): this class
//  has no non-trivial destructor of its own (it adds no fields), so the
//  "effective destructor" the dump calls is CAbility::~CAbility() directly,
//  the same one CAbility's own DeleteSelf calls - qualified here exactly the
//  way that file's own body qualifies it.
//============================================================================
#include "abilityinterfaced.h"

//  Storm.dll ordinal 403.
void __stdcall SMemFree(void* ptr, const char* logfilename, int logline,
                        unsigned int flags);

static const char kDelete[] = "delete";

CAbilityInterfaced* CAbilityInterfaced::DeleteSelf(int flags)
{
    this->CAbility::~CAbility();
    if ((flags & 1) && this != 0)
        SMemFree(this, kDelete, -1, 0);
    return this;
}
