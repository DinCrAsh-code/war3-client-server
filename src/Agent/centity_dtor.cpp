//============================================================================
//  0x6F467810 - NTempest::CEntity::`scalar deleting destructor'(unsigned int).
//  Root of the chain (see centity.h): no base of its own, so the body is
//  nothing but "restamp this class's own vtable" followed by the flag-gated
//  Storm_403 free, with no `test esi,esi` guard at all - the raw bytes have
//  only the one `test [esp+arg_0],1` branch, unlike the sibling scalar
//  deleting destructors further up the chain (Agent/cmemblock.h's own,
//  Pathfinding/crlagent.h's own) that call a real base destructor first and
//  do carry the extra null check.
//============================================================================
#include "centity.h"
#include "storm.h"

namespace NTempest {

void* CEntity::ScalarDeletingDestructor(unsigned int flags)
{
    *(void**)this = (void*)0x6F951784;   // ??_7CEntity@NTempest@@6B@

    if (flags & 1)
        SMemFree(this, "delete", -1, 0);

    return this;
}

}  // namespace NTempest
