//============================================================================
//  See lrpath_staticpools.h.
//============================================================================
#include "lrpath_staticpools.h"
#include "game.h"

//  0x6F49B040
void* CLrPath_GetStaticPool()
{
    return (char*)g_pTimeSync + 0x958;
}

//  0x6F497D00
void* CLrPathingAcc_GetStaticPool()
{
    return (char*)g_pTimeSync + 0x938;
}

//  0x6F4A0970
void* CLrPathingSys_GetStaticPool()
{
    return (char*)g_pTimeSync + 0x918;
}
