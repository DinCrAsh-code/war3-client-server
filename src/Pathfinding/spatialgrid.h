//============================================================================
//  The spatial partition grid an object re-registers itself into when it
//  moves (0x6F49xxxx / 0x6F4Bxxxx).
//
//  The grid is a row-major array of cell words, one per cell, plus a pool of
//  8-byte registration records.  A cell word is
//
//      bits  0..23   index of the first record registered in this cell,
//                    0xFFFFFF meaning "none"
//      bits 24..31   flags, preserved across every update
//
//  and a record is
//
//      +0  bits  0..23   index of the next record in the same cell's chain
//          bit  24       the layer the registration belongs to (the `flag`
//                        argument threaded down from CGridRegistration::
//                        UpdateBox: 0 for the old box, 0x1000000 for the new)
//      +4  the registering object
//
//  Free records are kept on their own singly linked chain through the same
//  low 24 bits, headed by m_freeRecord; when it runs out a new record is
//  appended to the pool instead.  m_occupancy is a parallel bitset, one bit
//  per cell, that only the layer-0 registration touches.
//============================================================================
#ifndef SPATIALGRID_H
#define SPATIALGRID_H

#include "boxmath.h"
#include "cellbuffer.h"

//  pathfind.h's cell coordinate pair; only ever taken by pointer here.
struct SCellPoint;
class CGridVec2;

//----------------------------------------------------------------------------
//  One registration record.  The pool is a CMemBlock-backed growable array of
//  these, so it carries the array bookkeeping (count/capacity/growth chunk)
//  on top of CMemBlock's own.
//----------------------------------------------------------------------------
struct SCellRecord
{
    int   m_link;    //  low 24 bits: next record; bit 24: layer flag
    void* m_owner;
};

const int kCellIndexMask = 0x00FFFFFF;   //  the link/head field
const int kCellFlagMask  = 0xFF000000;   //  everything the link field leaves
const int kCellIndexNone = 0x00FFFFFF;   //  "no record"

class CCellRecordArray : public CMemBlock
{
public:
    //  0x6F49E3E0 - store `*value` into every slot from `start` up to, but
    //  not past, min(m_count, start + count).  The only caller passes the
    //  post-append count for `count`, which the clamp against m_count makes
    //  equivalent to the number of slots it just added.
    void FillRecords(unsigned int start, const SCellRecord* value,
                     unsigned int count);

    //  0x6F49EE50 - make room for `count` more records, growing the block in
    //  whole growth chunks, then fill them with *value.  Returns 0 only when
    //  growth is needed and either impossible (no growth chunk configured) or
    //  refused by CMemBlock::SetSize.
    int AppendRecords(const SCellRecord* value, int count);

    unsigned int m_growChunk;    // +0x14
    unsigned int m_capacity;     // +0x18  records the block has room for
    unsigned int m_count;        // +0x1C  records in use
};

class CGridRegistration;

//----------------------------------------------------------------------------
//  A second instance of the exact same generic growable-pointer-array shape
//  `SBhPoTickRelaySlotArray` (Missile/bhpoprojectile.h) already documents -
//  a `CMemBlock` plus growBy/capacity/count, holding raw 4-byte pointers -
//  compiled separately for this module (confirmed by `Grow`'s own address,
//  0x6F49EF30, sitting in the exact same `spatialgrid.cpp` module range as
//  `CCellRecordArray::AppendRecords`/`FillRecords` right above it, not next
//  to the `SBhPoTickRelaySlotArray` twin at all). Holds `CGridRegistration*`
//  entries collected out of one grid cell by `CCellGrid::CollectRelated`
//  below - NIpse::CPoPosCl::Slot21's own related-agent-list rebuild
//  (Agent/poposcl_relatedagentslot21.cpp) is its only known user, which
//  borrows the actual backing instances from a small fixed pool on
//  `CTimeSync` rather than owning one itself - see that file for the pool.
//----------------------------------------------------------------------------
struct SGridRegistrationPtrArray
{
    //  0x6F49E420 - byte-identical shape to
    //  `SBhPoTickRelaySlotArray::FillNewSlots` (0x6F4AAC80).
    void FillNewSlots(unsigned int from, void* const* fillValue,
                      unsigned int count);

    //  0x6F49EF30 - byte-identical shape to `SBhPoTickRelaySlotArray::Grow`
    //  (0x6F4AB050).
    int Grow(const void* fillValue, unsigned int count);

    //  0x6F486950 - byte-identical shape to
    //  `SBhPoTickRelaySlotArray::RemoveRange` (0x6F4AB0D0), but a genuinely
    //  separate compiled twin in a completely different module (the
    //  0x6F4869xx neighbourhood of `Agent/poposcl_relatedagentslot21.cpp`'s
    //  own scratch-pool helpers, not this file's own 0x6F49Exxx/0x6F49Fxxx
    //  range) - defined there, not here, for the same reason
    //  `bhpoprojectile_slots.cpp`'s own copy of the shape lives next to its
    //  own caller rather than in this header's `.cpp`.
    int RemoveRange(unsigned int start, unsigned int count);

    CMemBlock    m_storage;   // +0x00 (m_data, +0x0C, is this array's base)
    unsigned int m_growBy;    // +0x14
    unsigned int m_capacity;  // +0x18
    unsigned int m_count;     // +0x1C
};

//----------------------------------------------------------------------------
//  The grid itself.  Only the fields this call tree reads are named; the gaps
//  keep the offsets exact.
//----------------------------------------------------------------------------
class CCellGrid
{
public:
    //  0x6F47BA20 - the half-open cell rectangle covering an already
    //  cell-scaled float rectangle.  `this` is dead in this build; see
    //  boxmath.cpp.
    SCellBox FloatBoxToCellBox(const SCellBoxF& box);

    //  0x6F49FDA0 - clamp `box` to the grid's own extent and register every
    //  cell it covers.
    void ForEachCellInBox(const SCellBox* box, CGridRegistration* owner,
                          unsigned int flag);

    //  0x6F49FCD0 - register one cell (row `y`, column `x`) for `owner`.
    void RegisterCell(int x, int y, CGridRegistration* owner,
                      unsigned int flag);

    //  0x6F469A70 - floor both components of a grid-space point into the
    //  cell they land in.  Another member whose `this` is never read - the
    //  shipped callers load ecx with the grid all the same.  Returns `out`.
    SCellPoint* FloorToCell(SCellPoint* out, const CGridVec2* point);

    //  0x6F4A0AB0 - the centre of `cell`, as two grid-space CFloats.  A
    //  member whose `this` is never read; see pathcellcentre.cpp.
    CFloat* CellCentre(CFloat* out, const SCellPoint* cell);

    //  Bounds-checked cell fetch, null when the cell is off the grid.
    //  Inlined at every call site; see SAStarGrid::CellAt (pathastar.h) for
    //  the same helper over that grid's own 8-byte cells.
    int* CellAt(int x, int y) const
    {
        if ((unsigned int)x >= (unsigned int)m_stride)
            return 0;
        if ((unsigned int)y >= (unsigned int)m_rows)
            return 0;
        return m_cells + (m_stride * y + x);
    }

    //  0x6F49F6C0 - claim this cell for a search: make (or reuse) a
    //  0x02000000-kind head record for it and stamp the pair
    //  {tag, node} into the second dword.  See pathfinelookup.cpp.
    void RegisterAStarCell(int x, int y, unsigned int tag, unsigned int node);

    //  0x6F49FE50 - walk `point`'s own cell record chain once, bump
    //  `m_visitStamp` by two (`stampCollected`/`stampSeen` below), and for
    //  every record that is not an AStar head (flag byte 0x02, see
    //  `RegisterAStarCell` above) and whose owner has not already been
    //  stamped by an earlier call this same walk (own `m_visitStamp` !=
    //  either fresh value, and != -1 "never test it") - collect it into
    //  `out` (`SGridRegistrationPtrArray::Grow(&owner, 1)`) and stamp it
    //  `stampCollected` when the record's own flag byte is 0x01 (the "new
    //  box" layer `CGridRegistration::UpdateBox` registers with,
    //  spatialgrid.cpp's own `0x1000000` `flag` argument), or just stamp it
    //  `stampSeen` (no collection) otherwise (the "old box", `flag == 0`,
    //  layer). Guarding on the owner's own +0x40 `m_moveFlags` bit
    //  0x10000000 first - collected/stamped only when that bit IS set,
    //  which is the OPPOSITE polarity `CPathSearch::TestCell`'s own header
    //  comment documents for the same bit ("reject the cell... when
    //  0x10000000 is set") - transcribed exactly as the shipped stream
    //  tests it (`test [owner+40h],10000000h` / `jz skip`) rather than
    //  guessed from that other reader's own polarity, which this call tree
    //  does not confirm either way. `this` in ecx, `retn 8`.
    void CollectRelated(const SCellPoint* point, SGridRegistrationPtrArray* out);

    char             m_reserved00[0x28];
    int*             m_cells;              // +0x28
    char             m_reserved2C[0x3C - 0x2C];
    int              m_stride;             // +0x3C  cells per row
    //  +0x40  rows in the grid.  Named by CPathSearch::TestCell
    //  (0x6F4A0D60), which bounds-checks a cell index against +0x3C and
    //  +0x40 before forming `m_cells[stride * y + x]`.
    int              m_rows;               // +0x40
    char             m_reserved44[0x54 - 0x44];
    SCellBox         m_extent;             // +0x54
    //  +0x64  cells -> world units, the inverse of m_cellScale below.
    //  CPathClient::PopWaypoint (0x6F49C2E0) reads it through
    //  CPathSearch::m_grid to scale a grid-space delta back to world space
    //  before handing it to PlaceWaypoint.
    CFloat           m_cellSize;           // +0x64
    CFloat           m_cellScale;          // +0x68  world units -> cells
    CCellRecordArray m_records;            // +0x6C
    char             m_reserved8C[0x98 - 0x8C];
    unsigned int*    m_occupancy;          // +0x98
    char             m_reserved9C[0xAC - 0x9C];
    int              m_freeRecord;         // +0xAC
    int              m_liveRecords;        // +0xB0
    //  +0xB4  a monotonically increasing "visit" stamp.  Bumped once per
    //  CPathSearch::TestCell (0x6F4A0D60) walk and written into every
    //  registration the walk reaches, so that an object registered in more
    //  than one of the cells a footprint covers is only tested once.
    int              m_visitStamp;         // +0xB4
};

//----------------------------------------------------------------------------
//  One object's registration in one grid: the cell rectangle it currently
//  occupies, the grid it occupies it in, and how many cells that is.
//----------------------------------------------------------------------------
class CGridRegistration
{
public:
    //  0x6F49FF90 - move the registration to `newBox`.  When the old and new
    //  rectangles overlap, only the symmetric difference is touched (the
    //  overlap is left registered as it stands); when they do not, the old
    //  rectangle is unregistered and the new one registered wholesale.
    void UpdateBox(const SCellBox& newBox);

    //  0x6F47D750 - the classification word an agent enumeration folds
    //  against its own exclusion mask before it will visit this
    //  registration's agent.  Built out of the packed byte at +0x3F (a
    //  three-bit kind, a four-bit sub-kind that only counts for kind 1,
    //  and one more flag in its top bit) and two bits of m_typeMask.  See
    //  gridregistrationclassbits.cpp.
    unsigned int ClassBits() const;

    //  0x6F49E0A0 - replace the rectangle and nothing else; see
    //  pathregistrationbox.cpp for how it differs from UpdateBox.
    void SetBox(const SCellBox* box);

    //  0x6F4A00B0 - tear this registration down: unregister from every grid
    //  cell it still covers (skipped when m_moveFlags bit 0x10000000 is
    //  set - see gridregistrationteardown.cpp for what that bit means),
    //  release its own slot in the *tempest presence* table
    //  (`g_unk6FAB778C`, Net/tempestproviderlisthash.cpp - a different
    //  object from g_pHandleTable, see Pathfinding/handletable.h's own
    //  note on the two), reset both slot fields, then cancel any pending
    //  path request if this mover has gone idle.  `retn 0`.
    void TeardownRegistration();

    char          m_reserved00[0x14];
    //  +0x14/+0x18 - this registration's own slot in the tempest presence
    //  table (g_unk6FAB778C), the same "handle plus a second field" shape
    //  Pathfinding/handletable.h's own SHandleTableEntry uses for the
    //  *different* global g_pHandleTable - two engine subsystems reusing
    //  one idiom, not the same table.  -1 in `m_presenceSlot` means "not
    //  registered" (TeardownRegistration's own gate).
    unsigned int  m_presenceSlot;      // +0x14
    unsigned int  m_presenceSlot2;     // +0x18
    SCellBox      m_box;          // +0x1C
    CCellGrid*    m_grid;         // +0x2C
    //  +0x34 is one dword in the shipped code and is read two ways:
    //  CPathSearch::TestCell (0x6F4A0D60) ANDs the whole word with the
    //  search's own mask and looks at the low 24 bits, and separately
    //  tests bit 0 of its top byte (+0x37) for "registration is live".
    //  Spelled as a union so both spellings stay on the same four bytes.
    //  +0x30  the coarse search's own node record for this registration,
    //  cached here so a neighbour test that lands on an occupied cell does
    //  not have to look one up - see CPathSearch::VisitCell.
    void*         m_nodeRecord;   // +0x30
    union
    {
        int m_typeMask;                 // +0x34  low 24 bits: collision type
        struct
        {
            unsigned char m_maskBytes[3];
            unsigned char m_flags;      // +0x37  bit 0: registration is live
        } m_bits;
    };
    //  +0x38  the last CCellGrid::m_visitStamp this registration was tested
    //  under; -1 means "never, and never test it".
    int           m_visitStamp;   // +0x38
    //  +0x3C is a cell count in three bytes and a packed classification
    //  byte in the fourth.  The union keeps both spellings on the same
    //  four bytes, exactly as +0x34 above already does for its own two
    //  readings; ClassBits() below is the only thing that reads the byte.
    union
    {
        int m_cellCount;                    // +0x3C
        struct
        {
            unsigned char m_cellCountBytes[3];
            unsigned char m_classBits;      // +0x3F
        } m_cellFields;
    };
    //  +0x40  movement/collision flags.  TestCell rejects the cell outright
    //  when any of 0x8FFFFFFF is set, and additionally when 0x60000000 is
    //  set and the search is not the re-entrant (already-moving) one.
    //  Bit 0x10000000 - set means "this registration was never actually
    //  added to the grid" (or already removed) - TeardownRegistration's
    //  own gate on whether ForEachCellInBox needs to run at all.
    unsigned int  m_moveFlags;    // +0x40
};

//----------------------------------------------------------------------------
//  0x6F4BC060 - subtract `hole` from `box`, writing the (up to four)
//  rectangles that cover what is left into `out` and returning how many.  A
//  `box` that is empty produces none; a `hole` that is empty or misses `box`
//  entirely produces one copy of `box`.
//----------------------------------------------------------------------------
int __fastcall SubtractCellBox(const SCellBox* box, const SCellBox* hole,
                               SCellBox* out);

//----------------------------------------------------------------------------
//  0x6F4A03F0 - SRegistrationContext::AllocRegistration
//  (Pathfinding/gridregistrationalloc.cpp). `this` is never really a
//  member's `this` - it is whatever CCellGrid*/int context each of this
//  function's two callers happens to be carrying (CTimeSync::GetCellGrid()'s
//  return, or CTimeSync's own +0x238 "tick" field, reinterpreted) - see
//  gridregistrationalloc.cpp's own header comment for the full story.
//  `SRegistrationContext` exists purely to spell this calling convention -
//  same "reinterpret-cast to call a member at a fixed address" idiom this
//  whole pipeline already uses for flat/offset-addressed classes.
//----------------------------------------------------------------------------
class SRegistrationContext
{
public:
    CGridRegistration* AllocRegistration(void* nodeRecord, int initArg);
};

#endif
