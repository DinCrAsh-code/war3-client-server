//============================================================================
//  0x6F53DF50 - CNetObserver::CallbackObserver's vtable slot 1, the scalar
//  deleting destructor.  Own translation unit for the same single reason
//  Agent/observer_deleteself.cpp is: the base ~CObserver() call below must
//  not be inlined, or this build's `/GS- /EHs-c-` compiler folds the whole
//  resource-release body in and overshoots the shipped instruction count.
//============================================================================
#include "netobserver.h"

//  Storm.dll ordinal 403.
void __stdcall SMemFree(void* ptr, const char* logfilename, int logline,
                        unsigned int flags);

static const char kDelete[] = "delete";

CObserver* CNetObserver::CallbackObserver::DeleteSelf(int flags)
{
    this->CObserver::~CObserver();
    if ((flags & 1) && this != 0)
        SMemFree(this, kDelete, -1, 0);
    return this;
}
