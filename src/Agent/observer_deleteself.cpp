//============================================================================
//  0x6F62AA80 - CObserver's vtable slot 1, the scalar deleting destructor.
//
//  Its own translation unit for the same single reason agent_deleteself.cpp
//  is: ~CObserver (observer.cpp, 0x6F62A9D0) must not be inlined into it.
//  The shipped body makes a real `call sub_6F62A9D0`; compiled beside that
//  destructor's definition this build inlines the whole resource-release
//  body instead and comes out twenty-six instructions against the shipped
//  fifteen.
//
//  See agent_deleteself.cpp for why the `this != 0` test belongs here and
//  not in the four further down the chain.
//============================================================================
#include "observer.h"

//  Storm.dll ordinal 403.
void __stdcall SMemFree(void* ptr, const char* logfilename, int logline,
                        unsigned int flags);

static const char kDelete[] = "delete";

CObserver* CObserver::DeleteSelf(int flags)
{
    this->CObserver::~CObserver();
    if ((flags & 1) && this != 0)
        SMemFree(this, kDelete, -1, 0);
    return this;
}
