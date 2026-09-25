//============================================================================
//  0x6F4BC060 - rectangle subtraction.  Its own translation unit: the address
//  sits well away from the grid's own module and its only caller must reach
//  it through a real call, not an inline copy.
//============================================================================
#include "spatialgrid.h"

//----------------------------------------------------------------------------
//  Subtract `hole` from `box`, writing the pieces that survive into `out` and
//  returning how many there are (0 to 4).
//
//  The pieces come out in the order a row decomposition produces them: the
//  full-width strip of rows above the hole, the full-width strip below it,
//  then the column strip to the left of the hole and the one to its right -
//  the last two clipped to the rows the first two left behind, so the four
//  never overlap.
//
//  `rowLo`/`rowHi` carry that clipped row span: rowLo starts at box.y0 and
//  becomes hole.y0 when the hole starts inside the box, rowHi starts at
//  box.y1 and becomes hole.y1 when the hole ends inside it - i.e. exactly the
//  vertical overlap.  Each is set before the strip that uses it, which is why
//  the shipped code initialises them where it does.
//----------------------------------------------------------------------------
int __fastcall SubtractCellBox(const SCellBox* box, const SCellBox* hole,
                               SCellBox* out)
{
    int by1 = box->m_y1;
    int by0 = box->m_y0;
    if (by0 >= by1)
        return 0;                       //  nothing to subtract from
    int bx0 = box->m_x0;
    int bx1 = box->m_x1;
    if (bx0 >= bx1)
        return 0;

    int hy0 = hole->m_y0;
    int hy1 = hole->m_y1;
    int hx0 = hole->m_x0;
    int hx1 = hole->m_x1;

    if (hy0 >= hy1 || hx0 >= hx1 ||
        hy0 >= by1 || hy1 <= by0 ||
        hx0 >= bx1 || hx1 <= bx0)
    {
        out[0] = *box;                  //  empty or disjoint hole
        return 1;
    }

    int n = 0;
    int rowLo = by0;
    if (hy0 > by0)
    {
        rowLo = hy0;
        out[n].m_y0 = by0;
        out[n].m_y1 = hy0;
        out[n].m_x0 = bx0;
        out[n].m_x1 = bx1;
        n++;
    }

    int rowHi = by1;
    if (hy1 < by1)
    {
        rowHi = hy1;
        out[n].m_x0 = bx0;
        out[n].m_y1 = by1;
        out[n].m_y0 = hy1;
        out[n].m_x1 = bx1;
        n++;
    }

    if (hx0 > bx0)
    {
        out[n].m_x0 = bx0;
        out[n].m_y0 = rowLo;
        out[n].m_y1 = rowHi;
        out[n].m_x1 = hx0;
        n++;
    }

    if (hx1 < bx1)
    {
        out[n].m_x0 = hx1;
        out[n].m_x1 = bx1;
        out[n].m_y1 = rowHi;
        out[n].m_y0 = rowLo;
        n++;
    }

    return n;
}
