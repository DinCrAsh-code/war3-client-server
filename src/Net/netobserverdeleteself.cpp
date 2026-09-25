//============================================================================
//  0x6F548F40 - CNetObserver's own (and only) vtable slot: the scalar
//  deleting destructor.  Own translation unit for the same reason
//  Agent/observer_deleteself.cpp is its own TU: the ~CNetObserver() call
//  below must stay a real out-of-line call, not inline into this one.
//============================================================================
#include "netobserver.h"

//  Storm.dll ordinal 403.
void __stdcall SMemFree(void* ptr, const char* logfilename, int logline,
                        unsigned int flags);

static const char kDelete[] = "delete";

CNetObserver* CNetObserver::DeleteSelf(int flags)
{
    this->CNetObserver::~CNetObserver();
    if ((flags & 1) && this != 0)
        SMemFree(this, kDelete, -1, 0);
    return this;
}
