//============================================================================
//  0x6F49CB60 - NIpse::CLrPath's own vtable slot 0 (CPathClient_GetTypeName).
//  Same one-instruction "class name accessor" shape as
//  NIpse::CRlAgent_GetTypeName (Pathfinding/crlagent.h) - `this` never read.
//============================================================================
#include "pathmove.h"

const char* PathClient_GetTypeName()
{
    return "LrPath";
}
