//============================================================================
//  0x6F49B580 / 0x6F49B590 - the two singleton fetches the movement
//  pathfinder reaches its shared state through.
//
//  Both are tail jumps: the time-sync singleton is loaded into ecx and the
//  one-instruction accessor on the far side does the rest.  Written as free
//  functions because that is what they are - neither takes a `this`.
//============================================================================
#include "pathmove.h"

CPathSearch* PathSearchSingleton()
{
    return g_pTimeSync->GetPathSearch();
}

CPathSearch* PathOwnerSingleton()
{
    return g_pTimeSync->GetPathOwner();
}
