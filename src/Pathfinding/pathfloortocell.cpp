//============================================================================
//  0x6F469A70 - floor a grid-space point into the cell it lands in.
//
//  Both components go through CFloatFloor and CFloatToInt, y first, and are
//  then stored y first as well - the reverse of the store order every point
//  *scaler* in this module uses, and the reason this is written out rather
//  than shared with them.
//
//  `this` is never read.  It is a member because every shipped call site
//  loads ecx with the pathing grid before the call, which nothing but a
//  thiscall explains - the same reasoning CCellGrid::CellCentre's own note
//  carries.
//============================================================================
#include "spatialgrid.h"
#include "pathfind.h"
#include "pathmove.h"

SCellPoint* CCellGrid::FloorToCell(SCellPoint* out, const CGridVec2* point)
{
    //  The two floors through named CFloat locals rather than straight
    //  into CFloatToInt: the shipped frame carries two separate return
    //  buffers for them (one of them the dead `point` argument slot), and
    //  a single nested expression lets MSVC recycle one buffer for both.
    CFloat fy = CFloatFloor(point->m_y);
    int y = CFloatToInt(fy);
    CFloat fx = CFloatFloor(point->m_x);
    int x = CFloatToInt(fx);
    out->m_y = y;
    out->m_x = x;
    return out;
}
