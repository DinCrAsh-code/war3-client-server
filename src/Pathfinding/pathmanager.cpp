//============================================================================
//  0x6F481420 / 0x6F481450 / 0x6F481460 - three accessors on the same
//  object the sync channels live on: the cell grid the agent enumerators
//  register against, and the two the placement search reaches its state
//  through.
//
//  One load each.  Their own translation unit because FindFreeGridPoint
//  calls two of them for real and the agent enumerators call the third
//  three times in a row.
//============================================================================
#include "pathfind.h"
#include "spatialgrid.h"

//  0x6F481420 - the cell grid registrations live in.
CCellGrid* CTimeSync::GetCellGrid() const
{
    return m_cellGrid;
}

//  0x6F481450 - the search state the placement run works on.
CPathSearch* CTimeSync::GetPathSearch() const
{
    return m_pathSearch;
}

//  0x6F481460 - the object whose grid and settings that run is against.
CPathSearch* CTimeSync::GetPathOwner() const
{
    return m_pathOwner;
}
