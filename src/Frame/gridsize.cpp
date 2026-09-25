//============================================================================
//  0x6F612E50 - CSimpleGrid::SetSize: give the grid `rows` rows of `columns`
//  cells each, growing every row that is short and clearing every cell.
//
//  The per-row growth is Storm's chunked one written out: when the row has no
//  chunk size yet it takes 0x40 for anything that big and otherwise the
//  largest power of two that fits (the `x & (x-1)` loop), then rounds the
//  requested count up to a multiple of it.  That is the same arithmetic
//  TSGrowableArray::Add carries in Storm/storm.h; it is spelled out here
//  because the shipped code has it inline and against a row record rather
//  than against a TSGrowableArray.
//
//  The rows are walked *backwards* and each row's cells are cleared
//  backwards too, both of which are the shipped order.
//============================================================================
#include "frame.h"
#include "framethunks.h"


void CSimpleGrid::SetSize(unsigned int rows, unsigned int columns)
{
    m_rowCount = rows;
    m_columnCount = columns;
    m_rowArray.SetCount(rows);

    unsigned int index = rows;
    while (index)
    {
        --index;
        SGridRow* row = &m_rowArray.m_rows[index];

        if (columns > row->m_count && columns > row->m_alloc)
        {
            unsigned int chunk = row->m_chunk;
            if (chunk == 0)
            {
                //  `chunk = columns` is hoisted above the test and the
                //  `< 0x40` arm is the fall-through one on purpose: that is
                //  the shipped block order (`cmp esi,40h` / `mov ecx,esi` /
                //  `jae`), and writing it the other way round both swaps the
                //  two arms and costs the compare its immediate, because ecx
                //  then has to hold 0x40 for the store.
                chunk = columns;
                if (columns < 0x40)
                {
                    unsigned int next = (columns - 1) & columns;
                    while (next)
                    {
                        chunk = next;
                        next = (chunk - 1) & chunk;
                    }
                    if (chunk < 1)
                        chunk = 1;
                }
                else
                {
                    row->m_chunk = 0x40;
                    chunk = 0x40;
                }
            }

            unsigned int alloc = columns;
            unsigned int over = columns % chunk;
            if (over)
                alloc = columns + (chunk - over);

            row->SetAlloc(alloc);
        }

        row->m_count = columns;

        unsigned int cell = columns;
        while (cell)
        {
            --cell;
            m_rowArray.m_rows[index].m_cells[cell] = 0;
        }
    }
}
