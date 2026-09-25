//============================================================================
//  0x6F473D10 - FindFreeWorldPoint: the world-space front end.
//
//  Everything this function does is a change of coordinates around one call
//  to FindFreeGridPoint:
//
//    * the point and the rectangle come in relative to nothing, so the map
//      origin is subtracted off and the result scaled from world units to
//      grid cells (32 world units to the cell - five exponent steps, with
//      an underflow flush, see WorldToGrid in game.h);
//    * the rectangle is first clamped into the playable area, each axis by
//      the same three-way pointer select ClampToGameBounds uses, against
//      the origin below and the map bound less a margin above;
//    * the mask argument is repacked from a plain 24-bit mask into the
//      "mask in the low 24 bits, flags in the top byte" word every cell and
//      registration in the grid carries;
//    * and the answer is taken back out the way it came in.
//
//  A ring count of -1 means "as many as it takes", which the shipped code
//  spells as the constant 32.
//============================================================================
#include "pathfind.h"
#include "gamebounds.h"

//  The extra inset the placement search adds to the bound margin.
extern const CFloat g_boundSlack;       // dword_6FAAE610

int __fastcall FindFreeWorldPoint(const SCellBoxF* bounds, CFloat* px,
                                  CFloat* py, const CFloat* radius, int mask,
                                  int startSide, int cellCentres,
                                  PathFilterFn filter, void* context,
                                  int unitType, int player, int placement,
                                  int rings)
{
    CGameData* gd = g_pGameData;

    //  Each difference is scaled *in place*, in its own statement: the
    //  shipped code loads the subtraction's own result slot back, scales
    //  it and stores it there again, which is what a named local reassigned
    //  from itself produces and what one nested expression does not (that
    //  one keeps the value in a register and lets MSVC sink the scaling).
    CFloat dx = *px - gd->m_originX;
    dx = WorldToGrid(dx);
    CFloat dy = *py - gd->m_originY;
    dy = WorldToGrid(dy);
    //  The pair handed to the grid-space search, and where it writes the
    //  answer back.
    CFloat cell[2] = { dx, dy };

    CFloat margin = g_boundMargin + g_boundSlack;
    SCellBoxF b = *bounds;

    //  Four clamps, each written as the pointer select of
    //  docs/msvc-vc8-idioms.md, "A three-way clamp is a pointer select" -
    //  here in its `if (below) ... else ...` form, so that the low-bound
    //  case is the fall-through and the high-bound test is jumped to, which
    //  is the block order the shipped code has.  The high bound is
    //  recomputed in each one rather than hoisted into a pair of locals:
    //  the shipped code performs all four subtractions.
    {
        const CFloat& hi = gd->m_boundX - margin;
        const CFloat* p;
        if (*(const float*)&b.m_x0 < *(const float*)&gd->m_originX)
            p = &gd->m_originX;
        else
        {
            p = &hi;
            if (!(*(const float*)&b.m_x0 > *(const float*)&hi))
                p = &b.m_x0;
        }
        b.m_x0 = *p;
    }
    {
        const CFloat& hi = gd->m_boundX - margin;
        const CFloat* p;
        if (*(const float*)&b.m_x1 < *(const float*)&gd->m_originX)
            p = &gd->m_originX;
        else
        {
            p = &hi;
            if (!(*(const float*)&b.m_x1 > *(const float*)&hi))
                p = &b.m_x1;
        }
        b.m_x1 = *p;
    }
    {
        const CFloat& hi = gd->m_boundY - margin;
        const CFloat* p;
        if (*(const float*)&b.m_y0 < *(const float*)&gd->m_originY)
            p = &gd->m_originY;
        else
        {
            p = &hi;
            if (!(*(const float*)&b.m_y0 > *(const float*)&hi))
                p = &b.m_y0;
        }
        b.m_y0 = *p;
    }
    {
        const CFloat& hi = gd->m_boundY - margin;
        const CFloat* p;
        if (*(const float*)&b.m_y1 < *(const float*)&gd->m_originY)
            p = &gd->m_originY;
        else
        {
            p = &hi;
            if (!(*(const float*)&b.m_y1 > *(const float*)&hi))
                p = &b.m_y1;
        }
        b.m_y1 = *p;
    }

    //  Four named locals rather than four arguments of one constructor:
    //  the shipped code scales each difference the moment it has it, where
    //  one expression lets MSVC sink all four scalings past all four
    //  subtractions.  The order is still last-first - x1, y1, x0, y0 - and
    //  the four copies into the rectangle still happen together at the end.
    //  g_pGameData is re-read for each one, which is also the shipped code:
    //  operator- is a call, and nothing here can prove it leaves the global
    //  alone.
    CFloat gx1 = b.m_x1 - g_pGameData->m_originX;
    gx1 = WorldToGrid(gx1);
    CFloat gy1 = b.m_y1 - g_pGameData->m_originY;
    gy1 = WorldToGrid(gy1);
    CFloat gx0 = b.m_x0 - g_pGameData->m_originX;
    gx0 = WorldToGrid(gx0);
    CFloat gy0 = b.m_y0 - g_pGameData->m_originY;
    gy0 = WorldToGrid(gy0);
    SCellBoxF grid(gy0, gx0, gy1, gx1);

    //  The radius is a length, not a position: scaled, not offset.
    CFloat cellRadius = WorldToGrid(*radius);

    //  The mask the grid matches against carries the same 24 bits in its
    //  top byte as its flag byte.
    int cellMask = (mask & 0x00FFFFFF) | (mask << 24);

    int ringLimit = rings;
    if (ringLimit == -1)
        ringLimit = 32;

    //  Success is the fall-through, failure the tail - the block order the
    //  shipped code has.
    if (FindFreeGridPoint(cell, &grid, &cellRadius, &cellMask, placement,
                          unitType, player, ringLimit, filter, context,
                          startSide, (cellCentres == 0)))
    {
        *px = GridToWorld(cell[0]) + g_pGameData->m_originX;
        *py = GridToWorld(cell[1]) + g_pGameData->m_originY;
        return 1;
    }
    return 0;
}

//  The jitter box's half-extent around the starting point, and the amount
//  it grows on each side per failed attempt - two more entries of the same
//  float-constant table g_boundMargin/g_CFloatHalf/g_boundSlack already
//  come from.  Confirmed from the real disassembly (not the root's
//  pseudocode, which does not show this call's true argument shape at
//  all - see the note below): `mov edx, dword_6FAAE520` / `mov eax,
//  dword_6FAAE510` load these two globals' *values* directly, not their
//  addresses, so they are plain CFloat constants added and subtracted
//  onto the grid-space position - never derived from the caller's own
//  `radius` argument, which is a completely separate quantity passed
//  through untouched to CPathSearch::FindFreePointNearby for its own
//  footprint-size test.
extern const CFloat g_jitterSearchHalfExtent;   // dword_6FAAE520
extern const CFloat g_jitterSearchStep;         // dword_6FAAE510

//============================================================================
//  0x6F473A80 - FindFreeWorldPointNearby: the world-space front end of the
//  jitter search - CWidget::MoveForward's other branch, run instead of
//  FindFreeWorldPoint above when the caller does not hand it a rebuilt
//  bounds rectangle (root pseudocode in
//  docs/targets/CWidget__MoveForward.md: `sub_6F473A80(&dword_6FAAE470,
//  v27, v38, v39)` - that pseudocode is wrong about the argument shape,
//  see below).
//
//  Same clamp-and-convert-to-grid-space shape as FindFreeWorldPoint, minus
//  the rectangle: one point in, ClampToGameBounds, WorldToGrid both axes
//  and the radius, repack the mask.  Then up to five attempts, each a
//  20-ring CPathSearch::FindFreePointNearby call against a box centred on
//  the *original* grid position at +/-g_jitterSearchHalfExtent on both
//  axes - not the caller's `radius`, which travels down unchanged as its
//  own argument for FindFreePointNearby's own footprint sizing.  Every
//  failed attempt grows the box outward by g_jitterSearchStep on all four
//  sides and resets the candidate back to the *original* position (not
//  wherever FindFreePointNearby's own internal jitter loop left it) before
//  trying again - confirmed off the real disassembly
//  (`6F473A80`-`6F473C87`), which is why this reconstruction reads the
//  raw addresses directly rather than trusting the root's IDA pseudocode:
//  that pseudocode's `sub_6F473A80(&dword_6FAAE470, v27, v38, v39)` shows
//  only 4 arguments and never names this box or its two constants at all -
//  IDA's decompiler evidently could not resolve this call's real signature
//  (a `CFloat*, CFloat*, const CFloat*, int, PathFilterFn, void*` ABI, per
//  the caller's own push sequence at 0x6F2AC18D-0x6F2AC19B) any better
//  than it resolved sub_6F4A3E80's, which the same pseudocode omits
//  discussing entirely.  On success, the answer comes back out of grid
//  space exactly like FindFreeWorldPoint does.
//============================================================================
int __fastcall FindFreeWorldPointNearby(CFloat* px, CFloat* py,
                                        const CFloat* radius, int mask,
                                        PathFilterFn filter, void* context)
{
    CGridVec2 cell(*px, *py);
    ClampToGameBounds(&cell);

    CFloat gx = cell.m_x - g_pGameData->m_originX;
    gx = WorldToGrid(gx);
    CFloat gy = cell.m_y - g_pGameData->m_originY;
    gy = WorldToGrid(gy);
    CGridVec2 pos(gx, gy);

    CFloat cellRadius = WorldToGrid(*radius);
    int cellMask = (mask & 0x00FFFFFF) | (mask << 24);

    CTimeSync* sync = g_pTimeSync;
    CPathSearch* search = sync->GetPathSearch();

    //  The constant is copied into a local first - the shipped code loads
    //  it once and passes the local's address to all four calls, the same
    //  shape ClampToGameBounds' own margin uses.
    CFloat halfExtent = g_jitterSearchHalfExtent;
    SCellBoxF jitterBox(gy - halfExtent, gx - halfExtent,
                        gy + halfExtent, gx + halfExtent);

    int attempt = 5;
    while (attempt != 0)
    {
        if (search->FindFreePointNearby(&pos, &jitterBox, &cellRadius,
                                        &cellMask, 20, filter, context))
        {
            *px = GridToWorld(pos.m_x) + g_pGameData->m_originX;
            *py = GridToWorld(pos.m_y) + g_pGameData->m_originY;
            return 1;
        }
        CFloat step = g_jitterSearchStep;
        jitterBox.m_x0 = jitterBox.m_x0 - step;
        jitterBox.m_x1 = jitterBox.m_x1 + step;
        jitterBox.m_y1 = jitterBox.m_y1 + step;
        jitterBox.m_y0 = jitterBox.m_y0 - step;
        pos.m_x = gx;
        pos.m_y = gy;
        attempt -= 1;
    }
    return 0;
}
