//============================================================================
//  0x6F47CFB0 / 0x6F47CBA0 - SJassRegion::SetCell / SetRect, the two members
//  RegionAddCell/RegionClearCell and RegionAddRect/RegionClearRect
//  (jassnatives_region.cpp) call through the set/clear flag.  Both convert a
//  world-space coordinate (or, for SetRect, a whole four-float block) into
//  the cell grid and register it against the region's own grid-registration
//  sub-object, at +0x20 of the region and the same `CGridRegistration` type
//  Pathfinding/spatialgrid.h already declares - the region shares the path
//  grid's own registration machinery rather than keeping a private one.
//
//  Own translation unit: both bodies issue a real `call` to
//  CCellGrid::RegisterCell / ForEachCellInBox and to the shared flush helper
//  below, so nothing here may be inlined into jassnatives_region.cpp's own
//  callers or into each other.
//============================================================================
#include "jassnatives.h"
#include "spatialgrid.h"
#include "gamebounds.h"
#include "boxmath.h"

//  The world-delta-to-cell-space flush, transliterated exactly as
//  Sync/timesyncbounds.cpp's own FlushedOriginDelta has it (and
//  Pathfinding/pathsearchoriginprobe.cpp's own copy) - see that file's
//  header for why it stays a literal bit-trick rather than a named helper
//  shared across translation units.  `delta`'s exponent is what the
//  `+0xFD800000` term shifts down by five (one grid cell is 32 world
//  units), so this is "world units past the origin" *and* "in cells" in one
//  step - which is why CCellGrid::FloatBoxToCellBox (boxmath.cpp) can floor
//  its result straight into a cell index with no further scale.
static CFloat FlushOriginDelta(const CFloat& value, const CFloat& origin)
{
    CFloat delta = value - origin;
    int bits = delta.m_bits;
    int flush = ((bits - 0x3000000) ^ bits) >> 31;
    return CFloat(~flush & (bits + 0xFD800000));
}

//----------------------------------------------------------------------------
//  0x6F47CFB0 - register or release one cell.  `retn 0Ch`.
//
//  The owner's own grid is read once, up front - the shipped code loads
//  both `[this+0x20]` and `owner->m_grid` before touching either coordinate,
//  which only matters because it is what fixes the receiver `RegisterCell`
//  is finally called on (the grid, not the region and not the owner).
//----------------------------------------------------------------------------
void SJassRegion::SetCell(const int* x, const int* y, int set)
{
    CGridRegistration* owner = *(CGridRegistration**)((char*)this + 0x20);
    CCellGrid* grid = owner->m_grid;

    CFloat flushedX = FlushOriginDelta(*(const CFloat*)x, g_pGameData->m_originX);
    CFloat flushedY = FlushOriginDelta(*(const CFloat*)y, g_pGameData->m_originY);

    //  y before x, the same order every other CFloatFloor/CFloatToInt pair
    //  in this codebase floors world position into a cell (see
    //  Pathfinding/pathfloortocell.cpp's own header note).
    int cellY = CFloatToInt(CFloatFloor(flushedY));
    int cellX = CFloatToInt(CFloatFloor(flushedX));

    grid->RegisterCell(cellX, cellY, owner, set ? 0x1000000 : 0);
}

//----------------------------------------------------------------------------
//  0x6F47CBA0 - register or release every cell a rect's bounds cover.
//  `retn 8`.
//
//  `bounds` is the four-word block RegionAddRect/RegionClearRect build from
//  a rect's own m_minX/m_minY/m_maxX/m_maxY - which, per SJassRect's own
//  comment (jassnatives.h), has one word crossed from what its name says:
//  the labelled m_maxY/m_maxX/m_minY/m_minX fields hold the true X-max,
//  Y-max, X-min and Y-min values respectively.  Reading them by the
//  labelled offset here (not by what the label claims) is what reproduces
//  the shipped field order - each flush pairs a labelled field with the
//  origin axis its *true* value belongs to.
//----------------------------------------------------------------------------
void SJassRegion::SetRect(const int* bounds, int set)
{
    const SJassBounds* b = (const SJassBounds*)bounds;

    CGridRegistration* owner = *(CGridRegistration**)((char*)this + 0x20);
    CCellGrid* grid = owner->m_grid;

    //  Each flush named and stored before the next begins, in the shipped
    //  call order (labelled maxY, maxX, minY, minX - see the header note on
    //  SJassRect's own "one word crossed" labelling).  VC8 schedules the
    //  four independent operator- calls ahead of their own bit-trick tails
    //  here rather than interleaving one pair at a time the way the shipped
    //  code does - a scheduling difference, not a value or call-order one;
    //  pending angr IDENTICAL verification.
    CFloat maxXCell = FlushOriginDelta(*(const CFloat*)&b->m_maxY, g_pGameData->m_originX);
    CFloat maxYCell = FlushOriginDelta(*(const CFloat*)&b->m_maxX, g_pGameData->m_originY);
    CFloat minXCell = FlushOriginDelta(*(const CFloat*)&b->m_minY, g_pGameData->m_originX);
    CFloat minYCell = FlushOriginDelta(*(const CFloat*)&b->m_minX, g_pGameData->m_originY);

    SCellBoxF worldRect(minYCell, minXCell, maxYCell, maxXCell);
    SCellBox cellBox = grid->FloatBoxToCellBox(worldRect);

    grid->ForEachCellInBox(&cellBox, owner, set ? 0x1000000 : 0);
}
