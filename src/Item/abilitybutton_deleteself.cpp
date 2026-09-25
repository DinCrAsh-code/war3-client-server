//============================================================================
//  0x6F068900 - CAbilityButton's vtable slot 1, the scalar deleting
//  destructor.  See abilitybutton.h.
//
//  Same shape as CPower::DeleteSelf / CAbilityInterfaced::DeleteSelf /
//  CAbility::DeleteSelf: this class adds fields of its own (m_defaultIconId,
//  m_field70/74/78/7C) but none of them own a resource that needs
//  unwinding, so the "effective destructor" the dump calls is still
//  CAbility::~CAbility() directly (0x6F038580) - confirmed via
//  worktree_store.py show 0x6F068900, the same body every class in this
//  chain from CAbility on up already calls.
//============================================================================
#include "abilitybutton.h"

//  Storm.dll ordinal 403.
void __stdcall SMemFree(void* ptr, const char* logfilename, int logline,
                        unsigned int flags);

static const char kDelete[] = "delete";

CAbilityButton* CAbilityButton::DeleteSelf(int flags)
{
    this->CAbility::~CAbility();
    if ((flags & 1) && this != 0)
        SMemFree(this, kDelete, -1, 0);
    return this;
}
