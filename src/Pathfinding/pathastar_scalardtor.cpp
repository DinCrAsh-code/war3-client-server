//============================================================================
//  0x6F489300 - NIpse::CLrPathingAcc::`scalar deleting destructor'(unsigned
//  int), vtable slot 2.  Calls the real destructor (~CPathAStar,
//  pathastar_dtor.cpp) first, then frees `this` when bit 0 of `flags` is
//  set, guarded by `this != 0` - the same shape
//  NIpse::CRlAgent::ScalarDeletingDestructor (Pathfinding/crlagent_scalardtor.cpp)
//  and CAgentBaseAbs::ScalarDeletingDestructor already use whenever a real
//  destructor call precedes the free.
//============================================================================
#include "pathastar.h"
#include "storm.h"

void* CPathAStar::ScalarDeletingDestructor(unsigned int flags)
{
    this->~CPathAStar();

    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);

    return this;
}
