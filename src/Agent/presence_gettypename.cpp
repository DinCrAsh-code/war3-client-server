//============================================================================
//  0x6F4798A0 - NTempest::CPresence::GetTypeName (vtable slot 0).
//  0x6F479BC0 - NTempest::CPresenceTagged::GetTypeName (vtable slot 0).
//  Both trivial, no `this` read, `retn` with zero stack bytes - the exact
//  same shape Pathfinding/crlagent_gettypename.cpp's own
//  `CRlAgent_GetTypeName` already establishes.  Kept in one file: neither
//  calls the other or anything else, so there is no inlining risk from
//  combining them.
//============================================================================
#include "presence.h"

namespace NTempest {

const char* CPresence::GetTypeName()
{
    return "Presence";
}

const char* CPresenceTagged::GetTypeName()
{
    return "PresenceTagged";
}

}  // namespace NTempest
