//============================================================================
//  0x6F4957A0 - FindFreeGridPoint: the grid-space front end of the placement
//  search.
//
//  It pushes a record describing this search onto a global stack of them
//  (there is one entry per nested search, and the depth is popped again on
//  the way out), then runs the search itself.
//
//  The record exists for one reason: when the caller named a unit type to
//  place, the search runs with *its own* filter (0x6F493CA0) and the record
//  as that filter's context, so the filter can see the unit type, the size
//  class and the original position while still chaining to whatever filter
//  the caller supplied.  When no unit type was named the record only carries
//  the caller's own filter, and the caller's own context goes down instead.
//
//  Note that the filter handed to the search is re-read out of the record
//  rather than kept in a local: that is the shipped code, and it is what
//  makes the two branches join on one call.
//============================================================================
#include "pathfind.h"

int __fastcall ClassifySlopeAxis(const CFloat* value);

int __fastcall FindFreeGridPoint(CFloat* pos, const SCellBoxF* box,
                                 const CFloat* radius, const int* mask,
                                 int placement, int unitType, int player,
                                 int rings, PathFilterFn filter,
                                 void* context, int startSide,
                                 int wholeCells)
{
    CTimeSync* sync = g_pTimeSync;
    CPathSearch* search = sync->GetPathSearch();

    SPathSearchRecord* rec = &g_pathSearchStack[g_pathSearchDepth++];

    void* ctx;
    //  The shipped code materialises this comparison into al with `setne`
    //  and then tests the widened byte, the `bool` round trip of
    //  docs/msvc-vc8-idioms.md.  Spelling it that way here changes nothing:
    //  the value is only ever consumed by this one `if`, and this compiler
    //  folds the round trip straight back into the branch whichever way it
    //  is written.  Left as the plain comparison it reads as.
    if (unitType != -1)
    {
        CPathSearch* owner = sync->GetPathOwner();
        rec->m_owner = owner;
        rec->m_cellScale = owner->m_grid->m_cellScale;
        rec->m_unitTypeCount = unitType;
        rec->m_unitType = player;
        rec->m_sizeClass = (unsigned short)ClassifySlopeAxis(radius) >> 1;
        rec->m_placement = placement;
        //  y before x, and both held until after the second call: the
        //  shipped code keeps the y product in a register across the x
        //  multiply and stores the two of them either side of the context
        //  pointer, which is what one pair of named locals produces and
        //  what two `rec->... = ...;` statements does not.
        rec->m_y = pos[1] * rec->m_cellScale;
        rec->m_x = pos[0] * rec->m_cellScale;
        rec->m_filter = PathPlacementFilter;
        rec->m_userFilter = filter;
        rec->m_userContext = context;
        ctx = rec;
    }
    else
    {
        rec->m_filter = filter;
        ctx = context;
    }

    int found = search->FindFreePoint(pos, box, startSide, radius, mask,
                                      rings, rec->m_filter, ctx, wholeCells);
    g_pathSearchDepth -= 1;
    return found;
}
