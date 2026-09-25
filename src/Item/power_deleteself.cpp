//============================================================================
//  0x6F05F8C0 - CPower's vtable slot 1, the scalar deleting destructor.
//  See power.h.
//
//  Same shape as CAbility::DeleteSelf / CAbilityInterfaced::DeleteSelf: this
//  class has no non-trivial destructor of its own (it adds no fields), so
//  the "effective destructor" the dump calls is CAbility::~CAbility()
//  directly (0x6F038580) - the same one both of those classes' own
//  DeleteSelf already call, qualified here exactly the way theirs are.
//============================================================================
#include "power.h"

//  Storm.dll ordinal 403.
void __stdcall SMemFree(void* ptr, const char* logfilename, int logline,
                        unsigned int flags);

static const char kDelete[] = "delete";

CPower* CPower::DeleteSelf(int flags)
{
    this->CAbility::~CAbility();
    if ((flags & 1) && this != 0)
        SMemFree(this, kDelete, -1, 0);
    return this;
}
