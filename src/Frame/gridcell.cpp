//============================================================================
//  0x6F612400 - CSimpleGrid::SetCellFrame: put `frame` into cell
//  (column, row), and place it.
//
//  Whatever was there is either destroyed through its own slot 2 (`release`
//  non-zero) or merely orphaned (`release` zero) - the shipped code branches
//  on the argument, not on whether the frame is shared.
//
//  The placement is the grid's span divided by its own dimensions times the
//  cell's index, and both divisors go through the *unsigned* integer-to-float
//  conversion (the `fild` with the 2^32 fixup above it), which is what says
//  the two counts are unsigned.
//============================================================================
#include "frame.h"
#include "framethunks.h"


void CSimpleGrid::SetCellFrame(unsigned int column, unsigned int row,
                               CSimpleFrame* frame, int release)
{
    CSimpleFrame* old = m_rowArray.m_rows[row].m_cells[column];
    if (old)
    {
        //  Written negated, because that is the order the shipped code lays
        //  the two arms out in: the SetParent(0) block falls through from the
        //  test and the slot-2 destroy is the jumped-to arm.  Spelling it
        //  `if (release) destroy; else orphan;` swaps the two blocks and
        //  moves every branch distance in the function with them.
        if (!release)
        {
            old->SetParent(0);
        }
        else
        {
            typedef void (__thiscall *DestroyFn)(void*, int);
            ((DestroyFn)(*(void***)old)[8 / 4])(old, 1);
        }
    }

    m_rowArray.m_rows[row].m_cells[column] = frame;
    if (!frame)
        return;

    frame->SetParent(this);

    if (m_54 == 0)
        return;

    //  Each step of the placement arithmetic is a named `float` local on
    //  purpose.  Assigning an x87 intermediate to a `float` lvalue makes MSVC
    //  round it to 32 bits with an `fstp <slot>` / `fld <slot>` pair, and the
    //  shipped code has exactly four such pairs in here - one per named
    //  intermediate.  Folding the two placements back into one expression
    //  each keeps the values in st(0) and loses those eight instructions.
    float ySpan  = m_134 - m_12C;
    float yStep  = ySpan / (float)m_rowCount;
    float y      = yStep * (float)row;

    float xSpan  = m_138 - m_130;
    float xStep  = xSpan / (float)m_columnCount;
    float x      = xStep * (float)column;

    frame->SetPointTo(0, this, 0, x, y, 1);
}
