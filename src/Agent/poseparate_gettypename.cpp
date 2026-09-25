//============================================================================
//  0x6F4843B0 - NIpse::CPoSeparate::GetTypeName, vtable slot 0. Trivial, no
//  `this` read, `retn` with zero stack bytes - the same shape
//  Agent/presence_gettypename.cpp's own CPresence/CPresenceTagged pair and
//  Pathfinding/crlagent_gettypename.cpp's own CRlAgent already establish.
//============================================================================
#include "poseparate.h"

namespace NIpse {

const char* CPoSeparate::GetTypeName()
{
    return "PoSeparate";
}

}  // namespace NIpse
