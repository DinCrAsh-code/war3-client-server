//============================================================================
//  0x6F479C60 - NTempest::CPresenceTagged::`scalar deleting destructor'
//  (unsigned int), vtable slot 2.  Byte-identical to
//  Agent/presence_dtor.cpp's own CPresence::ScalarDeletingDestructor apart
//  from address - see presence.h's own header comment.
//============================================================================
#include "presence.h"
#include "storm.h"

namespace NTempest {

void* CPresenceTagged::ScalarDeletingDestructor(unsigned int flags)
{
    *(void**)this = (void*)0x6F951CA0;   // ??_7CPresence@NTempest@@6B@

    if (flags & 1)
        SMemFree(this, "delete", -1, 0);

    return this;
}

}  // namespace NTempest
