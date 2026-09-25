//============================================================================
//  0x6F488330 - CTimeSync::PinRegistration: clip a cell rectangle against
//  the path grid's own extent and, if anything survives, run the pin (or
//  release) over the four slots at +0x23C.
//
//  Was a `mov eax,<address> / jmp eax` thunk in pathmove_thunks.cpp.
//  Fifty-four instructions, and the only already-reconstructed thing in it
//  (IntersectCellBox, 0x6F47D560) is what identifies the first argument: it
//  is a cell *rectangle*, not "the registration's own +0x1C sub-object" the
//  old declaration guessed.
//
//  A null first argument means "the whole grid extent" - the shipped code
//  skips the intersect and points at `grid->m_extent` directly - and both
//  paths meet at one four-dword copy into a local, which is why the pointer
//  is selected first and the copy written once.
//
//  The emptiness test is the same "min >= max" IntersectCellBox's own note
//  describes, y before x, and both halves are signed.
//
//  The tail is one setup call plus a three-iteration loop that steps a
//  pointer through the slot array by one dword at a time, handing each
//  *adjacent pair* to the follow-up - so the four slots are read as three
//  overlapping pairs, not as two disjoint ones.  A `for (i = 3; i != 0;
//  i--)` countdown, which is this codebase's own idiom for a loop whose
//  guard is a cheap compare against zero.
//
//  Own translation unit: CPathClient's own callers reach this with a real
//  call, and it makes five of its own.
//============================================================================
#include "game.h"
#include "boxmath.h"
#include "pathfind.h"
#include "spatialgrid.h"

//----------------------------------------------------------------------------
//  A per-direction pin slot: cell data (8-byte records) plus that
//  direction's own bound rect and cell scale.  `m_pinSlots` (an
//  `unsigned int` array in the existing declaration below - kept, since
//  fixing its element type would ripple into every other reader of that
//  array) actually holds four of these, cast through: BeginPin's own body
//  dereferences its second argument at +0x28/+0x3C/+0x40/+0x68 the moment
//  it arrives, which only a real pointer explains.
//----------------------------------------------------------------------------
struct STimeSyncPinSlot
{
    char   m_reserved00[0x28];
    void*  m_cellData;          // +0x28 - 8-byte records
    char   m_reserved2C[0x10];  // +0x2C
    int    m_boundX;            // +0x3C
    int    m_boundY;            // +0x40
    char   m_reserved44[0x24];  // +0x44
    CFloat m_cellScale;         // +0x68

    //  0x6F483C40 - clip `*in` (a cell rect already scaled by
    //  m_cellScale) against this slot's own [0,m_boundX)x[0,m_boundY)
    //  extent, writing the result into `*out`.  `retn 8` confirmed from
    //  its own machine code (ida_query func_at + trailing bytes); its body
    //  is out of this batch's scope.
    void __thiscall ClipCellBoxF(SCellBox* out, const SCellBoxF* in);
};

__declspec(naked) void __thiscall STimeSyncPinSlot::ClipCellBoxF(SCellBox*,
                                                                  const SCellBoxF*)
{
    __asm
    {
        mov     eax, 06F483C40h
        jmp     eax
    }
}

//  0x6F483160 - register or release (by which of four global handler
//  tables gets passed) one cell record for one of BeginPin's four
//  directions.  `this` is the owning CTimeSync, confirmed `retn 0x14` (five
//  stack dwords: the cell record, the table, the direction tag, and the two
//  doubled coordinates) from its own machine code; its body is out of this
//  batch's scope.
struct STimeSyncPinRecordOps
{
    void __thiscall Apply(void* cellRecord, const void* table, int tag,
                          int x2, int y2);
};

__declspec(naked) void __thiscall STimeSyncPinRecordOps::Apply(void*, const void*,
                                                                int, int, int)
{
    __asm
    {
        mov     eax, 06F483160h
        jmp     eax
    }
}

//  0x6F488010 - PinPair's own per-direction record op: same five-stack-dword
//  shape as STimeSyncPinRecordOps::Apply above (this call tree's own
//  disassembly pushes the identical five values at every one of PinPair's
//  four call sites), so `retn 0x14` is inferred from that identical call
//  shape rather than confirmed independently - its own tail is a backward
//  jump inside its own range, not a `retn`, so the trailing-bytes trick
//  that confirmed the other three thunks in this file does not apply here.
//  Out of this batch's scope either way.
struct STimeSyncPinPairOps
{
    void __thiscall Apply(void* cellRecord, unsigned int b, int tag,
                          int x2, int y2);
};

__declspec(naked) void __thiscall STimeSyncPinPairOps::Apply(void*, unsigned int,
                                                              int, int, int)
{
    __asm
    {
        mov     eax, 06F488010h
        jmp     eax
    }
}

//  The four per-direction handler tables BeginPin's own four release calls
//  each pass - opaque data this call tree only ever forwards, never reads.
extern const void* const g_pinReleaseTable0;    // unk_6FA84E84
extern const void* const g_pinReleaseTable1;    // unk_6FA84E88
extern const void* const g_pinReleaseTable2;    // unk_6FA84E8C
extern const void* const g_pinReleaseTable3;    // unk_6FA84E90

//----------------------------------------------------------------------------
//  0x6F485150 - CTimeSync::BeginPin: convert `box`'s four int fields to
//  world floats, scale by the slot's own cell size, clip against the
//  slot's bound rect, and - only when `pin` is false (a release, not a
//  pin) - walk every surviving cell once, running the four direction ops
//  over each.
//----------------------------------------------------------------------------
struct STimeSyncPin
{
    void __thiscall BeginPin(const SCellBox* box, unsigned int slot, int pin);
    void __thiscall PinPair(const SCellBox* box, unsigned int a, unsigned int b);
};

void __thiscall STimeSyncPin::BeginPin(const SCellBox* box, unsigned int slotArg, int pin)
{
    STimeSyncPinSlot* slot = (STimeSyncPinSlot*)slotArg;

    CFloat x1 = CFloatFromInt(box->m_x1);
    CFloat y1 = CFloatFromInt(box->m_y1);
    CFloat x0 = CFloatFromInt(box->m_x0);
    CFloat y0 = CFloatFromInt(box->m_y0);

    SCellBoxF worldRect(y0, x0, y1, x1);
    SCellBoxF cellSize(slot->m_cellScale, slot->m_cellScale,
                       slot->m_cellScale, slot->m_cellScale);
    SCellBoxF cellRect = MulCellBoxF(worldRect, cellSize);

    SCellBox clipped;
    slot->ClipCellBoxF(&clipped, &cellRect);

    if (clipped.m_y0 >= clipped.m_y1)
        return;
    if (clipped.m_x0 >= clipped.m_x1)
        return;

    int y0i = clipped.m_y0, y1i = clipped.m_y1;
    int x0i = clipped.m_x0, x1i = clipped.m_x1;

    for (int y = y0i; y < y1i; y++)
    {
        for (int x = x0i; x < x1i; x++)
        {
            if ((unsigned)x >= (unsigned)slot->m_boundX)
                continue;
            if ((unsigned)y >= (unsigned)slot->m_boundY)
                continue;

            char* cellRecord = (char*)slot->m_cellData + (y * slot->m_boundX + x) * 8;
            if (!cellRecord)
                continue;

            int x2 = x + x;
            int y2 = y + y;
            cellRecord[7] = 0;
            if (pin)
                continue;

            STimeSyncPinRecordOps* self = (STimeSyncPinRecordOps*)this;
            self->Apply(cellRecord, &g_pinReleaseTable0, 0, x2, y2);
            self->Apply(cellRecord, &g_pinReleaseTable1, 2, x2, y2);
            self->Apply(cellRecord, &g_pinReleaseTable2, 4, x2, y2);
            self->Apply(cellRecord, &g_pinReleaseTable3, 6, x2, y2);
        }
    }
}

//----------------------------------------------------------------------------
//  0x6F4881C0 - CTimeSync::PinPair: the same box-to-cell conversion and clip
//  as BeginPin, against slot `a`, but the per-cell tail is unconditional
//  (no `pin` test) and hands each surviving cell's own record to the
//  *other* slot (`b`) instead of a table.
//----------------------------------------------------------------------------
void __thiscall STimeSyncPin::PinPair(const SCellBox* box, unsigned int a, unsigned int b)
{
    STimeSyncPinSlot* slot = (STimeSyncPinSlot*)a;

    CFloat x1 = CFloatFromInt(box->m_x1);
    CFloat y1 = CFloatFromInt(box->m_y1);
    CFloat x0 = CFloatFromInt(box->m_x0);
    CFloat y0 = CFloatFromInt(box->m_y0);

    SCellBoxF worldRect(y0, x0, y1, x1);
    SCellBoxF cellSize(slot->m_cellScale, slot->m_cellScale,
                       slot->m_cellScale, slot->m_cellScale);
    SCellBoxF cellRect = MulCellBoxF(worldRect, cellSize);

    SCellBox clipped;
    slot->ClipCellBoxF(&clipped, &cellRect);

    if (clipped.m_y0 >= clipped.m_y1)
        return;
    if (clipped.m_x0 >= clipped.m_x1)
        return;

    int y0i = clipped.m_y0, y1i = clipped.m_y1;
    int x0i = clipped.m_x0, x1i = clipped.m_x1;

    for (int y = y0i; y < y1i; y++)
    {
        for (int x = x0i; x < x1i; x++)
        {
            if ((unsigned)x >= (unsigned)slot->m_boundX)
                continue;
            if ((unsigned)y >= (unsigned)slot->m_boundY)
                continue;

            char* cellRecord = (char*)slot->m_cellData + (y * slot->m_boundX + x) * 8;
            if (!cellRecord)
                continue;

            int x2 = x + x;
            int y2 = y + y;
            cellRecord[7] = 0;

            STimeSyncPinPairOps* self = (STimeSyncPinPairOps*)this;
            self->Apply(cellRecord, b, 0, x2, y2);
            self->Apply(cellRecord, b, 2, x2, y2);
            self->Apply(cellRecord, b, 4, x2, y2);
            self->Apply(cellRecord, b, 6, x2, y2);
        }
    }
}

void CTimeSync::PinRegistration(const SCellBox* clip, int pin)
{
    CCellGrid* grid = m_pathSearch->m_grid;

    //  One copy-initialisation from a selected source, not a default-
    //  constructed local assigned in two branches: SCellBox's own default
    //  constructor zeroes all four fields (boxmath.h explains why it has
    //  to exist), and the shipped code zeroes nothing - it selects a
    //  pointer in eax and falls into a single four-dword copy.
    SCellBox box = (clip != 0) ? IntersectCellBox(grid->m_extent, *clip)
                              : grid->m_extent;

    if (box.m_y0 >= box.m_y1)
        return;
    if (box.m_x0 >= box.m_x1)
        return;

    ((STimeSyncPin*)this)->BeginPin(&box, m_pinSlots[0], pin);

    unsigned int* slot = m_pinSlots;
    for (int remaining = 3; remaining != 0; remaining--)
    {
        ((STimeSyncPin*)this)->PinPair(&box, slot[0], slot[1]);
        slot++;
    }
}
