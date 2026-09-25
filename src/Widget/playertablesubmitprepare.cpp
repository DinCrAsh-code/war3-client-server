//============================================================================
//  0x6F26D0C0 - SPlayerTableSubmitParts::Prepare, the first half of every
//  player-table query: turn a world point into the pair of 16-bit codes
//  Submit() (playertablesubmitparts.cpp) indexes its answer table with.
//
//  It was a `mov eax,<address> / jmp eax` redirect in playertablesubmit.cpp,
//  parked there because "its own body calls one more function (0x6F192620)
//  that nothing in this repo has yet".  That is still true - 0x6F192620 has
//  no dump anywhere in `asm/` - but it is one call, and a redirect for that
//  one callee buys the whole thirty-three-instruction body back.
//
//  What the body does: ask 0x6F192620 for the cell the point falls in as a
//  (column, row) pair, fold the pair into one index with the object's own
//  row shift at +0x68, and read the same index out of two parallel word
//  arrays - the one at +0x30 with 0xF000 forced on, the one at +0x2C with
//  everything above twelve bits masked off.  Both stores are word-sized,
//  which is what fixes both out-pointers as `unsigned short*` (the finding
//  playertablesubmit.cpp already records, and which must not be changed).
//
//  Both cell coordinates are zeroed before the call and only the callee
//  writes them, so they are ordinary out-parameters and not in/out ones.
//
//  Own translation unit: all three of the fog queries (playertablefog.h)
//  reach this with a real call, so it must not be in a position to inline
//  into any of them.
//============================================================================
#include "playertablefog.h"
#include "game.h"

//----------------------------------------------------------------------------
//  0x6F192620 - SPlayerTableCellQuery::PointToCell, the world-point-to-cell
//  conversion.  Also reached from SetFogStateRadius (0x6F3BA4C8),
//  SetFogStateRect (0x6F3B76FC) and 0x6F4075F9, none of which has a dump.
//
//  A thiscall member with four stack arguments, `retn 10h` (confirmed by
//  its own dump, sub_6F192620_0x6F192620_calltree_asm.md).
//
//  Body: for each axis, subtract the grid origin from the coordinate, scale
//  by the per-axis cell size reciprocal, and truncate to int (the same
//  operator-/operator*/CFloatToInt sequence, once for X into *outColumn and
//  once for Y into *outRow).  Both results are then clamped into
//  [0, axisCount-1] with the classic unsigned-compare-then-sign-select shape:
//  `(unsigned)v > (unsigned)(count-1)` selects the clamp path, and inside it
//  `v < 0 ? 0 : count-1` becomes `sar/not/and` because count-1 is itself a
//  variable (not a compile-time constant), which is why it's not the
//  `cmov`-free two-cmp clamp documented elsewhere in msvc-vc8-idioms.md.
//----------------------------------------------------------------------------
struct SPlayerTableCellGrid
{
    char   m_reserved00[0x44];
    CFloat m_originY;       // +0x44
    CFloat m_originX;       // +0x48
    char   m_reserved4C[0x60 - 0x4C];
    int    m_columnCount;   // +0x60
    char   m_reserved64[0x6C - 0x64];
    int    m_rowCount;      // +0x6C
    CFloat m_scaleX;        // +0x70
    CFloat m_scaleY;        // +0x74
};

struct SPlayerTableCellQuery
{
    void PointToCell(const CFloat* x, const CFloat* y, int* outColumn,
                     int* outRow);
};

void __thiscall SPlayerTableCellQuery::PointToCell(const CFloat* x,
                                                    const CFloat* y,
                                                    int* outColumn,
                                                    int* outRow)
{
    SPlayerTableCellGrid* self = (SPlayerTableCellGrid*)this;

    *outColumn = CFloatToInt((*x - self->m_originX) * self->m_scaleX);
    *outRow    = CFloatToInt((*y - self->m_originY) * self->m_scaleY);

    //  Written as the branchless mask select rather than a ternary: a
    //  ternary here compiles to a second, nested branch
    //  (`test eax,eax`/`jge`), where the dump has one branch only (the
    //  in-range skip) and unconditionally computes the clamped value with
    //  `sar/not/and` - `mask` is all-1-bits for a negative value and 0
    //  otherwise, so `~mask & max` is 0 when negative and `max` when not.
    if ((unsigned int)*outColumn > (unsigned int)(self->m_columnCount - 1))
    {
        int mask = *outColumn >> 31;
        *outColumn = ~mask & (self->m_columnCount - 1);
    }

    if ((unsigned int)*outRow > (unsigned int)(self->m_rowCount - 1))
    {
        int mask = *outRow >> 31;
        *outRow = ~mask & (self->m_rowCount - 1);
    }
}

//  Only the three members this function reads.
struct SPlayerTableCellTables
{
    char            m_reserved00[0x2C];
    unsigned short* m_lowCodes;    // +0x2C
    unsigned short* m_highCodes;   // +0x30
    char            m_reserved34[0x34];
    //  +0x68 - how far a row index shifts left before the column is added,
    //  i.e. log2 of the table's row stride.
    int             m_rowShift;    // +0x68
};

void __thiscall SPlayerTableSubmitParts::Prepare(CWorldVec3* pos,
                                                 unsigned short* outCodeA,
                                                 unsigned short* outCodeB)
{
    SPlayerTableCellTables* self = (SPlayerTableCellTables*)this;

    //  Written in this order: the shipped code stores the column slot
    //  first and the row slot second, both from the same zeroed register.
    int column = 0;
    int row = 0;

    ((SPlayerTableCellQuery*)this)->PointToCell(&pos->m_x, &pos->m_y,
                                                &column, &row);

    int index = (row << self->m_rowShift) + column;

    *outCodeA = (unsigned short)(self->m_highCodes[index] | 0xF000);
    *outCodeB = (unsigned short)(self->m_lowCodes[index] & 0x0FFF);
}
