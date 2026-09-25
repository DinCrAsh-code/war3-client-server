//============================================================================
//  0x6F497D30 / 0x6F4A0AB0 - a cell's centre point, in grid-space CFloats.
//
//  Two byte-identical copies in two modules: convert both cell indices from
//  int and add half a cell to each, y first, and hand the caller's own
//  buffer back.  Neither reads `this`, but both callers load ecx with the
//  grid before calling, so both are methods on it rather than free functions
//  - the same dead-`this` shape CCellGrid::FloatBoxToCellBox has.
//
//  y before x: the shipped code converts and adds the y axis, then the x
//  axis, and stores x into the buffer first.
//============================================================================
#include "pathfind.h"
#include "pathastar.h"
#include "spatialgrid.h"

//  The half a cell each axis is offset by.
extern const CFloat g_CFloatHalf;       // dword_6FAAE4D0

//  0x6F497D30 - the per-cell A*'s copy.
CFloat* SAStarGrid::CellCentre(CFloat* out, const SCellPoint* cell)
{
    CFloat y = CFloatFromInt(cell->m_y) + g_CFloatHalf;
    CFloat x = CFloatFromInt(cell->m_x) + g_CFloatHalf;

    out[0] = x;
    out[1] = y;
    return out;
}

//  0x6F4A0AB0 - the fine search's copy, on the spatial grid rather than the
//  A*'s own pyramid level.
CFloat* CCellGrid::CellCentre(CFloat* out, const SCellPoint* cell)
{
    CFloat y = CFloatFromInt(cell->m_y) + g_CFloatHalf;
    CFloat x = CFloatFromInt(cell->m_x) + g_CFloatHalf;

    out[0] = x;
    out[1] = y;
    return out;
}
