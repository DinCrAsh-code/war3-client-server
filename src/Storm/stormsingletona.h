//============================================================================
//  The large (0x22F0 byte) lazily-constructed Storm singleton GetStormSingletonA
//  hands back (0x6F01F5A0, lazysingleton.cpp).  Only the members
//  sub_6F742820 (QueryStormSingletonACell, stormsingletonaquery.cpp) and
//  sub_6F747450 (QueryCellRecordIndex, JASS_SetItemPosition_callee1's own
//  dump) read are named; the object is built by a constructor deep in
//  Storm's own address range (0x6F768D10) this call tree never reaches the
//  body of, so the surrounding layout is not recoverable here - see
//  CLAUDE.md, "struct offsets are exact, and only touched members get
//  names".
//============================================================================
#ifndef STORMSINGLETONA_H
#define STORMSINGLETONA_H

#include "fvec3.h"
#include "packedcolor.h"

//----------------------------------------------------------------------------
//  One visual-attachment record in the singleton's own 0xA0-byte array
//  (+0x2CC).  Only the three fields CSelectable's slot 104 reaches through
//  are named; the stride comes straight out of the shipped index maths
//  (`lea eax,[eax+eax*4]` / `shl eax,5`, i.e. index * 160).
//----------------------------------------------------------------------------
struct SAttachmentRecord
{
    char          m_reserved00[4];
    unsigned int  m_flags;      // +0x04 - bit 4 "on walkable ground",
                                //         bit 3 "dirty, re-upload me"
    float         m_z;          // +0x08
    FVec3         m_pos;        // +0x0C
    char          m_reserved18[0x48 - 0x18];
    unsigned int  m_color;      // +0x48 - packed, written whole
    char          m_reserved4C[0xA0 - 0x4C];
};

struct SStormSingletonA
{
    //  0x6F762310 (`retn 14h`) - the nearest obstruction along the segment
    //  `from` -> `to` in this singleton's own registry, with the hit point
    //  and the distance written through the two out pointers.  Batch G of
    //  the JASS_PauseGame closure owns the body; a redirect for now
    //  (Misc/pausegamecalleethunks.cpp).
    int QueryObstruction(const float* from, const float* to,
                         float* outHitPoint, float* outDistance, int flags);

    //  0x6F742820 - the type nibble of the cell at (x, y), or -1 when
    //  either coordinate falls outside the table's current bounds
    //  (m_boundX/m_boundY).  A plain thiscall member (`this` in ecx, both
    //  arguments on the stack - not __fastcall's edx) - see
    //  stormsingletonaquery.cpp.
    int QueryCell(int x, int y);

    //  0x6F747450 - convert a world-space point to this table's own grid
    //  space (subtract m_originX/m_originY, scale, round), bound-check
    //  against m_recordStride/m_maxTableY (a *different* bound pair from
    //  QueryCell's m_boundX/m_boundY - see stormsingletonaquery.cpp), and if
    //  in range return the masked dword at +4 of the record at that cell;
    //  -1 out of range.  Called from JASS_SetItemPosition_callee1's own
    //  ground-lookup helper (0x6F012850, itemgroundlookup.cpp), which then
    //  uses the returned value as a *second*, direct index into this same
    //  m_data table to fetch a 3-float record - what that indirection means
    //  is not established by this call tree; see itemgroundlookup.cpp.
    //  One argument, a pointer to the point - not two floats.  The shipped
    //  code reads `[edx]` and `[edx+4]` off a single stack dword and cleans
    //  exactly four bytes (`retn 4`); two float parameters build a `retn 8`
    //  that eats four bytes of the real caller's frame per call.
    int QueryCellRecordIndex(const FVec3* point) const;

    //  0x6F74CAB0 - reached only through QueryTintAt below (0x6F74CBB0,
    //  still a redirect) as its own `sub_6F74CBB0 -> sub_6F74CAB0` call.
    //  Shifts the incoming point's X and Y by g_unkFltStormCellRecordOffset
    //  (Z untouched) and re-derives QueryCellRecordIndex's own packed
    //  gridY*(m_recordStride+1)+gridX index by dividing the returned value
    //  back apart - the exact same bound pair QueryCellRecordIndex itself
    //  checks (gridX against m_recordStride, gridY against m_maxTableY),
    //  just decoded rather than computed from a point.  When
    //  m_cellRecordFlagBit0xC0Mode is set, ORs in one more bit read out of
    //  m_data's own record at +0xC (bits 0x40/0x80) - a fourth touched byte
    //  of that record, alongside QueryCell's +0xA nibble and
    //  QueryCellRecordIndex's own +4 masked dword.  Returns the flag bit
    //  through the second argument when it is non-null either way.  Unless
    //  GetFlag0x970Bit9_6F740930(this) or that record bit is set, returns 0
    //  without ever touching the +0x9C4 table; otherwise returns the byte at
    //  m_cellRecordFlagsTable[m_recordStride*gridY + gridX].  `char` return
    //  (not `int`) is what makes the two early failure paths emit
    //  `or al, 0FFh` instead of a full `or eax, 0FFFFFFFFh` - matching
    //  QueryTintAt's own very next instruction, `movzx eax, al`, which only
    //  ever looks at the low byte anyway.
    char QueryCellRecordFlagsAt(const FVec3* point, int* outSubFlag) const;

    //  0x6F74CBB0 - the terrain tint at a world point, written through the
    //  first argument.  A plain thiscall member with both arguments on the
    //  stack (`retn 8`).  Not reconstructed - it walks two different tint
    //  tables (+0x9DC as dwords, +0x19DC as words) depending on a mode flag
    //  at +0x21E4 and calls two more Storm-internal helpers; redirected in
    //  stormsingleton_terrainredirect.cpp.
    void QueryTintAt(unsigned int* out, const FVec3* point);

    //  0x6F740940 - deep in Storm's own address range, out of scope per
    //  CLAUDE.md, "work outward... not into Storm's internals" - declared
    //  only so JASS_SetPlayerAlliance's own closure
    //  (Jass/jasssetplayeralliance.cpp) resolves.  Latches `value` at
    //  +0x99C, then - only when the count at +0xFC (sic: read at +0x100,
    //  0xFC/4 dwords in) is nonzero - walks a table at +0x104 (stride
    //  0x94) setting bit 8 in each entry's own +0x4C word.  Plain
    //  thiscall, one stack argument, `retn 4`.
    void SetField99C(int value);

    //  0x6F73AD20 - is (x, y) within m_recordStride/m_maxTableY?  See
    //  stormsingletonagridrecords.cpp.
    int IsRecordInBounds(unsigned int x, unsigned int y) const;

    //  0x6F741E10 - overwrite the 0x1C-byte record at (col, row) with `src`.
    //  See stormsingletonagridrecords.cpp.
    void SetDataRecordAt(const void* src, int col, int row);

    //  0x6F746640 - does any of 3 neighbouring cells carry a different type
    //  nibble than (x, y) itself?  See stormsingletonagridrecords.cpp.
    int HasDifferingNeighborType(int x, int y) const;

    //  0x6F73EF90 - world point to "quadrant" index (a coarser 4x4-cell
    //  grid over the same table).  See stormsingletonawavefront.cpp.
    int ComputeWavefrontQuadIndex(const FVec3* point) const;

    //  0x6F752470 - decode a packed record index and forward to
    //  BlendCellValue (0x6F7523C0, thunked).  See
    //  stormsingletonawavefront.cpp.
    float QueryBlendedRecordValue(const void* record, int flags);

    //  0x6F754FC0 - does (x, y) carry a "special road marking" flag, either
    //  directly (record+0xC bit 3) or through the RS/RLAA tag probes and
    //  the neighbour-type check?  See stormsingletonawavefront.cpp.
    int HasSpecialRoadMarking(int x, int y);

    //  0x6F7424B0 - push a tint back at one terrain record, blending it
    //  against the record's own alpha.  Same thiscall shape (`retn 8`).
    //  `tint` is in/out despite the `const`: the shipped body writes the
    //  blended result back over it.  The `const` is kept because it is what
    //  the whole call chain above (StoreTerrainTint, 0x6F00F7D0) is already
    //  declared with, and changing it there changes three mangled names to
    //  say what this one comment already does.
    //  Reconstructed in stormsingletonstoretint.cpp - it is 85
    //  instructions, not the "~270" this comment used to claim, and four
    //  fifths of them are one divide-by-255 idiom repeated per channel.
    void StoreTint(int recordIndex, const unsigned int* tint);

    //  0x6F73AFA0 - deep in Storm's own address range, out of this target's
    //  scope per CLAUDE.md, "work outward... not into Storm's internals" -
    //  declared only so QueryCell's own call site resolves.  A real
    //  __thiscall member (`this` in ecx, four stack args), not the
    //  __fastcall free function this repo declared it as before a live
    //  250-hook run crashed three callers at eip = this address + 0x2A:
    //  QueryCell's own call site (sub_6F742820, already in this dump)
    //  pushes, in order, &x, &y, y, x, then `mov ecx, esi` (this) - so the
    //  real stack layout is (x, y, &y, &x), not the (self, x, y, &x, &y)
    //  __fastcall shape guessed here previously (which put `x` in `edx`
    //  instead of on the stack, and had the two out-pointers swapped).
    //  Grows the table and clamps (x, y) in place through the two
    //  pointers when either coordinate falls outside the current capacity.
    void EnsureStormSingletonACapacity(int x, int y, int* outY, int* outX);

    char            m_reserved00[4];
    //  +0x04 / +0x08 - two pointers StoreTint refuses to run without; what
    //  they point at is not recoverable from that one function, only that
    //  both must be non-null before any tint is written.
    void*           m_tintTableA;      // +0x04
    void*           m_tintTableB;      // +0x08
    char            m_reserved0C[0x30 - 0x0C];
    int             m_tintRecordCount; // +0x30
    //  +0x34 - 24-byte terrain records; StoreTint reads a one-byte blend
    //  weight at +0x14 of each and does nothing when it is zero.
    unsigned char*  m_tintRecords;     // +0x34
    char            m_reserved38[4];
    //  +0x3C - the four-channel colour StoreTint blends *towards*, read a
    //  byte at a time from channel 3 down to channel 0.
    SPackedColor    m_tintTarget;      // +0x3C
    char            m_reserved40[0xB0 - 0x40];
    //  +0xB0 - a lazily-constructed sub-object (Storm_401(0x24 bytes) once,
    //  cached here forever after), read by CItem-batch1's own +0x970 option
    //  setter family (Storm/stormsingletonapreferencesinks.cpp).  What it
    //  actually is is not established by this call tree.
    void*           m_lazySubObjectAtB0;   // +0xB0
    int             m_recordStride;    // +0xB4 - also QueryCellRecordIndex's
                                        //         own max valid table-X index
    int             m_maxTableY;       // +0xB8 - QueryCellRecordIndex's own
                                        //         max valid table-Y index
    unsigned int    m_boundX;          // +0xBC
    unsigned int    m_boundY;          // +0xC0
    //  QueryCellRecordIndex's own world-to-table origin, a *different* pair
    //  from m_boundX/m_boundY above (both read, both used, in the same
    //  function) - not otherwise identified from this call tree.
    //
    //  Y first: the shipped code subtracts +0xC8 from the point's *X* and
    //  +0xC4 from its *Y* (0x6F747457 and 0x6F74749E), so the pair is stored
    //  in the opposite order from the one the coordinates arrive in.
    float           m_originY;         // +0xC4
    float           m_originX;         // +0xC8
    //  0x6F741920 / 0x6F741970 - write one visual attachment's Z (with its
    //  "on walkable ground" bit) or its whole world position, and mark the
    //  record dirty either way.  Both hand back the address of the record's
    //  own flags word, which no caller in this family uses.  Plain thiscall
    //  members - the shipped code takes every argument on the stack and
    //  cleans them itself (`retn 0Ch` / `retn 8`).  See stormattachments.cpp.
    unsigned int* SetAttachmentWalkableZ(int index, int onWalkable, float z);
    unsigned int* SetAttachmentPosition(int index, const FVec3* pos);

    //  0x6F741A50 - the same family, one slot along: set a visual
    //  attachment's colour.  Reconstructed in stormattachments.cpp; unlike
    //  its two siblings it neither sets the dirty bit nor returns anything.
    void SetAttachmentColor(int index, const unsigned int* color);

    //  0x6F7419B0 / 0x6F741A00 - CUnit's own third attachment slot
    //  (unit.h's m_handle278) adds two more per-record bits nothing else in
    //  this codebase reaches: same "reload +0x2CC, `or [ecx+eax+4],8`"
    //  dirty-bit shape as SetAttachmentWalkableZ/SetAttachmentPosition
    //  above, just bits 0x200 and 0x400 instead of 0x10.  Nothing in either
    //  call tree that reaches these two says what the bits mean beyond
    //  their position - not named further than that, per CLAUDE.md ("do not
    //  invent fields the call tree does not read").  See
    //  stormattachments.cpp.
    unsigned int* SetAttachmentFlagBit9(int index, int on);
    unsigned int* SetAttachmentFlagBit10(int index, int on);

    char            m_reservedCC[0xE4 - 0xCC];
    unsigned char*  m_data;            // +0xE4 - 0x1C-byte records, a type
                                        //         nibble at +0xA of each,
                                        //         a masked dword at +4 and
                                        //         a 3-float payload at +0x10
    char            m_reservedE8[0x110 - 0xE8];
    //  +0x110 / +0x114 - the pickable-object registry QueryObstruction walks:
    //  a count and an array of pointers, one per registered object.
    unsigned int    m_pickCount;       // +0x110
    void**          m_pickObjects;     // +0x114
    char            m_reserved118[0x2CC - 0x118];
    //  0x2CC - the visual-attachment array.  Re-read from the object on
    //  every access rather than cached in a local, which is what the
    //  shipped code's three separate `mov edx,[ecx+2CCh]` loads in one
    //  17-instruction function are: the compiler cannot prove the stores
    //  in between do not alias the pointer itself.
    SAttachmentRecord* m_attachments;  // +0x2CC
    char            m_reserved2D0[0x780 - 0x2D0];
    //  +0x780 - nonzero once the per-cell scalar table at +0x784 has been
    //  populated; QueryCellScalarAt below returns g_CFloatZero without
    //  touching the table while this is clear.  Not otherwise identified.
    int             m_cellScalarTableReady;   // +0x780
    //  +0x784 - a `m_recordStride`-major table of raw dwords, indexed by
    //  QueryCellScalarAt below; reuses this same object's m_recordStride
    //  (+0xB4) and m_maxTableY (+0xB8) as its own two bounds, which is
    //  the same pair QueryCellRecordIndex already reads for a different
    //  table.  Declared `int*` because the value is copied through with
    //  no float instruction ever touching it - see QueryCellScalarAt.
    int*            m_cellScalarTable;        // +0x784

    //  0x6F7442F0 (`retn 0Ch`) - the raw dword at cell (rowCoord, colCoord)
    //  of the +0x784 table (rowCoord clamped into [0, m_recordStride-1],
    //  colCoord into [0, m_maxTableY-1]), or g_CFloatZero when
    //  m_cellScalarTableReady is clear.  What the value actually encodes
    //  is not established by this call tree; see
    //  src/Unit/unit_widgetplacement.cpp, its only caller here.
    void QueryCellScalarAt(class CFloat* out, int rowCoord, int colCoord) const;

    char            m_reserved788[0x970 - 0x788];
    //  +0x970 - a bitfield of small on/off display/quality/sound option
    //  bits, each one set or cleared by CItem-batch1's own preference
    //  setter family (Storm/stormsingletonapreferencesinks.cpp) and then
    //  poked at SStormTerrainReleaseTarget::ReleaseGate4 (0x6F765240,
    //  Jass/jassstormterraingates.cpp) with the same bit value - only the
    //  handful of bits that family actually touches are named there;
    //  nothing else in this call tree reads or writes this dword.
    unsigned int    m_optionFlags970;      // +0x970
    char            m_reserved974[0x9C4 - 0x974];
    //  +0x9C4 - a `m_recordStride`-major table of raw bytes, same two
    //  bounds (m_recordStride/m_maxTableY) as m_cellScalarTable above, read
    //  only by QueryCellRecordFlagsAt.  What the byte encodes is not
    //  established by this call tree.
    unsigned char*  m_cellRecordFlagsTable;   // +0x9C4
    char            m_reserved9C8[0x21E0 - 0x9C8];
    //  +0x21E0 - gates whether QueryCellRecordFlagsAt tests m_data's own
    //  +0xC record byte at all; a different flag from QueryTintAt's own
    //  +0x21E4 mode switch four bytes further on.
    int             m_cellRecordFlagBit0xC0Mode; // +0x21E0
    char            m_reserved21E4[0x22F0 - 0x21E4];
};

extern SStormSingletonA* g_pStormSingletonA;   // dword_6FAAE790

//  QueryCellRecordIndex's own scale/bias/rounding constants - not otherwise
//  identified from this call tree (see stormsingletonaquery.cpp).
extern const double g_unkDblQueryCellScale;    // dbl_6F8763A8
extern const double g_unkDblQueryCellBias;     // dbl_6F875120

//  QueryCellRecordFlagsAt's own X/Y shift constant - not otherwise
//  identified from this call tree.
extern const float g_unkFltStormCellRecordOffset;  // flt_6FAA1744

//  The world-to-grid cell size the wavefront-quadrant conversion
//  (stormsingletonawavefront.cpp) divides by - not otherwise identified.
extern const float g_unkFltGridCellSizeX;  // flt_6FAA1748
extern const float g_unkFltGridCellSizeY;  // flt_6FAA174C

#endif
