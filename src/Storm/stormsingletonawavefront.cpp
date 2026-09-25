//============================================================================
//  Three more SStormSingletonA members, reached from CItem's own vtable BFS
//  (batch 8 of 8 - see docs/targets/CItem-callee-batch8.md).  All three
//  bottom out in helpers Misc/citemclosure_batch8_gridleaves.cpp already
//  thunks (their own record/config layout is never established in this
//  closure); the struct/method declarations below name only the shape
//  needed to call through them - see that file for the real addresses and
//  the reasoning.
//============================================================================
#include "stormsingletona.h"

struct SGridTagProbeThunk { int __thiscall ProbeTagRS(int x, int z); };
struct SGridTagRlaaThunk  { int __thiscall ProbeTagRlaa(int x, int z); };
struct SGridBlendThunk
{
    float __thiscall BlendCellValue(int gridX, int gridY, int recPtr, int flags);
};

//----------------------------------------------------------------------------
//  0x6F73EF90 - `retn 4`: world point to "quadrant" index over a coarser
//  4x4-cell grid laid on top of the same m_recordStride x m_maxTableY
//  table - the same wavefront-list granularity
//  citemclosure_terrain_gates.cpp's own ReleaseTerrainGridMeshWavefrontLists
//  operates on.
//----------------------------------------------------------------------------
int SStormSingletonA::ComputeWavefrontQuadIndex(const FVec3* point) const
{
    int gridX = (int)((point->m_x - m_originX) / g_unkFltGridCellSizeX);
    int gridY = (int)((point->m_y - m_originY) / g_unkFltGridCellSizeY);

    int quadStride = ((m_recordStride - 1) >> 2) + 1;
    int quadMaxY   = ((m_maxTableY   - 1) >> 2) + 1;

    if ((unsigned int)gridX > (unsigned int)(quadStride - 1))
        gridX = quadStride - 1;
    if ((unsigned int)gridY > (unsigned int)(quadMaxY - 1))
        gridY = quadMaxY - 1;

    return gridY * quadStride + gridX;
}

//----------------------------------------------------------------------------
//  0x6F752470 - `retn 8`: unpack a record's own masked +4 dword (the
//  "gridY*(m_recordStride+1)+gridX" scheme QueryCellRecordIndex's own
//  header comment documents) back into its two grid coordinates and forward
//  them, with the record pointer and caller's flags, to BlendCellValue.
//----------------------------------------------------------------------------
float SStormSingletonA::QueryBlendedRecordValue(const void* record, int flags)
{
    int packed = *(const int*)((const char*)record + 4) & 0x3FFFF;
    int stride1 = m_recordStride + 1;
    int gridY = packed / stride1;
    int gridX = packed % stride1;

    return ((SGridBlendThunk*)this)->BlendCellValue(gridX, gridY,
                                                     (int)record, flags);
}

//----------------------------------------------------------------------------
//  0x6F754FC0 - `retn 8`: a "special road marking" test at (x, y) - the
//  record's own +0xC bit 3 short-circuits both probes; otherwise the RS tag
//  probe gates which of the RLAA probe or the neighbour-type check runs.
//----------------------------------------------------------------------------
int SStormSingletonA::HasSpecialRoadMarking(int x, int y)
{
    int stride1 = m_recordStride + 1;
    int baseIndex = stride1 * y + x;

    if (m_data[baseIndex * 0x1C + 0xC] & 8)
        return 1;

    if (((SGridTagProbeThunk*)this)->ProbeTagRS(x, y))
    {
        if (((SGridTagRlaaThunk*)this)->ProbeTagRlaa(x, y))
            return 1;
    }
    else
    {
        if (HasDifferingNeighborType(x, y))
            return 1;
    }

    return 0;
}
