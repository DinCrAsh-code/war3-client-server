//============================================================================
//  0x6F4AA340 - NIpse::CPrBehavior::AttachHandle, vtable slot 3. See
//  prbehavior.h's own comment for the full shape and the open question
//  about what `request`'s own +0x0C field really means.
//
//  Own translation unit: SHandleTableEntry::RegisterOrReuse is a real call
//  (Pathfinding/handletable.h) - declared locally rather than `#include`d,
//  the same way Agent/agentbaseabseventbinding.cpp and
//  Agent/agentrelationteardown.cpp both already do, to avoid that header's
//  own `SHandleTable` colliding with game.h's independently-modelled one.
//============================================================================
#include "prbehavior.h"

struct SHandleTableEntry
{
    void RegisterOrReuse(SHandleRegistrationRequest* request);
};

namespace NIpse {

void CPrBehavior::AttachHandle(SHandleRegistrationRequest* request)
{
    ((SHandleTableEntry*)this)->RegisterOrReuse(request);

    //  Two genuinely separate return paths in the shipped body (each with
    //  its own `pop`/`pop`/`retn 4`), not one merged epilogue - written
    //  that way here rather than as a single trailing `m_flags = 0` shared
    //  by both branches, which the compiler folds into one path and
    //  reorders differently.
    if (request == 0)
    {
        m_field34 = 0xFFFFFFFF;
        m_flags = 0;
        return;
    }

    m_field34 = *(const unsigned int*)((const char*)request + 0x0C);
    m_flags = 0;
}

}  // namespace NIpse
