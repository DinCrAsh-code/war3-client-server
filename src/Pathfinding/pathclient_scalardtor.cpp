//============================================================================
//  0x6F49CDB0 - NIpse::CLrPath::`scalar deleting destructor'(unsigned int),
//  vtable slot 2.  Calls the real ~CPathClient() (lrpath_dtor.cpp) first,
//  then frees `this` when bit 0 of `flags` is set, guarded by `this != 0` -
//  the same shape NIpse::CRlAgent::ScalarDeletingDestructor
//  (Pathfinding/crlagent_scalardtor.cpp) uses whenever a real destructor
//  call precedes the free.
//============================================================================
#include "pathmove.h"
#include "storm.h"

void* CPathClient::ScalarDeletingDestructor(unsigned int flags)
{
    this->~CPathClient();

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}
