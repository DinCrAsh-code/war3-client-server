//============================================================================
//  0x6F742820 - the type nibble of the cell at (x, y) in the large Storm
//  singleton's table, or -1 when either coordinate falls outside the
//  table's current bounds.
//
//  Two unsigned bound checks (catching negative coordinates the same test
//  as an out-of-range one), a capacity/clamp pass through a callee deep in
//  Storm's own address range (0x6F73AFA0, out of scope - see
//  stormsingletona.h), then a 28-byte-record lookup: index = (stride+1)*x+y,
//  byte at record+0xA, low nibble only.
//============================================================================
#include "stormsingletona.h"

int SStormSingletonA::QueryCell(int x, int y)
{
    if ((unsigned int)x >= m_boundX)
        return -1;
    if ((unsigned int)y >= m_boundY)
        return -1;

    EnsureStormSingletonACapacity(x, y, &y, &x);

    int index = (m_recordStride + 1) * x + y;
    return m_data[index * 0x1C + 0xA] & 0xF;
}

//============================================================================
//  0x6F747450 - a *second* lookup into the same table, reached from
//  JASS_SetItemPosition_callee1's own ground-lookup helper
//  (0x6F012850, itemgroundlookup.cpp) rather than from anything already in
//  this file's own call tree.
//
//  Converts a world-space point to this table's own grid space (subtract
//  m_originX/m_originY, scale, round to nearest), bound-checks the result
//  against m_recordStride/m_maxTableY (not m_boundX/m_boundY - a different
//  pair, read nowhere else in this call tree), and on success returns the
//  masked dword at +4 of the record at that cell rather than QueryCell's own
//  nibble at +0xA.  Unlike QueryCell, there is no EnsureStormSingletonACapacity
//  clamp/grow call here - out-of-range simply returns -1.
//
//  The shipped code performs the world-to-grid rounding with a magic-double-
//  add-then-shift bias trick against three doubles this dump gives no
//  identifying use for (dbl_6F8763A8/dbl_6F875120/dbl_6F981A38) - not
//  reproduced instruction-for-instruction here; this function is a
//  semantic-level reconstruction of the bound checks, the index arithmetic
//  and the record read, not a claim that the rounding is bit-exact.  See
//  docs/targets/JASS_SetItemPosition_callee1.md.
//============================================================================
int SStormSingletonA::QueryCellRecordIndex(const FVec3* point) const
{
    //  dbl_6F8763A8/dbl_6F875120 - one shared scale and one shared additive
    //  bias, both loaded once and reused for the X and Y conversions alike
    //  (the shipped code keeps them on the FPU stack across both).
    int gridX = (int)((point->m_x - m_originX) * g_unkDblQueryCellScale + g_unkDblQueryCellBias);
    int gridY = (int)((point->m_y - m_originY) * g_unkDblQueryCellScale + g_unkDblQueryCellBias);

    //  Unsigned compares (`cmp edx,eax` / `ja` at 0x6F7474F6-F8, `cmp
    //  esi,[ecx+0B8h]` / `jbe` at 0x6F7474FD-503 in the dump) - the exact
    //  same "catch a negative coordinate as a huge unsigned one" trick
    //  QueryCell's own m_boundX/m_boundY checks use (stormsingletonaquery.cpp
    //  above).  A plain signed `>` here (this reconstruction's previous
    //  shape) lets a negative gridX/gridY - reachable whenever worldX/worldY
    //  falls below m_originX/m_originY - through to the index arithmetic
    //  below, which then reads m_data at a negative byte offset: a real,
    //  live-crash-reproducing out-of-bounds read the signed compare cannot
    //  catch.
    if ((unsigned int)gridX > (unsigned int)m_recordStride)
        return -1;
    if ((unsigned int)gridY > (unsigned int)m_maxTableY)
        return -1;

    int index = (m_recordStride + 1) * gridY + gridX;
    return *(const int*)(m_data + index * 0x1C + 4) & 0x3FFFF;
}

//  Declared here rather than pulled through a shared header: trivialpredicates.cpp
//  is a grab-bag of free __fastcall accessors with no receiver class of its own,
//  so nothing else in this repo forward-declares one across a TU boundary yet.
extern int __fastcall GetFlag0x970Bit9_6F740930(const void* self);

//============================================================================
//  0x6F74CAB0 - reached only through QueryTintAt (0x6F74CBB0, still a
//  redirect); see the declaration in stormsingletona.h for the full
//  breakdown.  g_unkFltStormCellRecordOffset (flt_6FAA1744) is applied to
//  X and Y only - Z is forwarded unchanged - and its own identity is not
//  established from this call tree.
//============================================================================
char SStormSingletonA::QueryCellRecordFlagsAt(const FVec3* point, int* outSubFlag) const
{
    FVec3 shifted;
    shifted.m_x = point->m_x - g_unkFltStormCellRecordOffset;
    shifted.m_y = point->m_y - g_unkFltStormCellRecordOffset;
    shifted.m_z = point->m_z;

    int index = QueryCellRecordIndex(&shifted);
    if (index == -1)
        return -1;

    int gridY = (unsigned int)index / (m_recordStride + 1);
    int gridX = (unsigned int)index % (m_recordStride + 1);
    if ((unsigned int)gridX >= (unsigned int)m_recordStride)
        return -1;
    if ((unsigned int)gridY >= (unsigned int)m_maxTableY)
        return -1;

    //  Written with explicit gotos, matching the shipped shape (a real
    //  `test`/`jz` branch, not the arithmetic 0/1 idiom MSVC's if-converter
    //  reaches for when the same value is spelled as a plain if/else).
    int subFlag;
    if (m_cellRecordFlagBit0xC0Mode == 0)
        goto noSubFlag;
    if ((m_data[index * 0x1C + 0xC] & 0xC0) == 0)
        goto noSubFlag;
    subFlag = 1;
    goto haveSubFlag;
noSubFlag:
    subFlag = 0;
haveSubFlag:

    if (outSubFlag != 0)
        *outSubFlag = subFlag;

    if (GetFlag0x970Bit9_6F740930(this) == 0 && subFlag == 0)
        return 0;

    return m_cellRecordFlagsTable[m_recordStride * gridY + gridX];
}
