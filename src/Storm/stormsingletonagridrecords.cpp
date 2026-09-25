//============================================================================
//  Three more SStormSingletonA members, reached from CItem's own vtable BFS
//  (batch 8 of 8 - see docs/targets/CItem-callee-batch8.md), all sharing the
//  same +0xB4/+0xB8/+0xE4 field trio (m_recordStride/m_maxTableY/m_data)
//  stormsingletonaquery.cpp's own QueryCell/QueryCellRecordIndex already
//  document for this class.
//============================================================================
#include "stormsingletona.h"

extern "C" void* __cdecl memcpy(void* dst, const void* src, unsigned int size);

//----------------------------------------------------------------------------
//  0x6F73AD20 - `retn 8`: is (x, y) within the table's own m_recordStride/
//  m_maxTableY bounds?  Two unsigned compares, no clamp/grow call (unlike
//  QueryCell's own m_boundX/m_boundY pair) - just a yes/no.
//----------------------------------------------------------------------------
int SStormSingletonA::IsRecordInBounds(unsigned int x, unsigned int y) const
{
    if (x >= (unsigned int)m_recordStride)
        return 0;
    if (y >= (unsigned int)m_maxTableY)
        return 0;
    return 1;
}

//----------------------------------------------------------------------------
//  0x6F741E10 - `retn 0Ch`: overwrite the whole 0x1C-byte record at (col,
//  row) with `src` - the same `index = (m_recordStride+1)*row + col`
//  addressing QueryCell/QueryCellRecordIndex already use, just written with
//  a raw copy instead of one field at a time.  No bounds check here; the
//  shipped body trusts its caller.
//----------------------------------------------------------------------------
void SStormSingletonA::SetDataRecordAt(const void* src, int col, int row)
{
    int index = (m_recordStride + 1) * row + col;
    memcpy(m_data + index * 0x1C, src, 0x1C);
}

//----------------------------------------------------------------------------
//  0x6F746640 - `retn 8`: does any of 3 neighbouring cells (offsets read out
//  of the parallel tables g_neighborProbeDX/g_neighborProbeDY) carry a
//  different type nibble (record+0xA, low 4 bits - the same nibble
//  QueryCell's own return value already documents) than the cell at (x, y)
//  itself?  Returns 1 on the first mismatch, 0 once all 3 probes agree.
//----------------------------------------------------------------------------
extern const int g_neighborProbeDX[3];   // dword_6F9815E4
extern const int g_neighborProbeDY[3];   // dword_6F9815F4

int SStormSingletonA::HasDifferingNeighborType(int x, int y) const
{
    int stride = m_recordStride + 1;
    int baseIndex = stride * y + x;
    int baseType = m_data[baseIndex * 0x1C + 0xA] & 0xF;

    for (int i = 0; i < 3; i++)
    {
        int neighborIndex = stride * (y + g_neighborProbeDY[i])
                           + (x + g_neighborProbeDX[i]);
        int neighborType = m_data[neighborIndex * 0x1C + 0xA] & 0xF;

        if (neighborType != baseType)
            return 1;
    }

    return 0;
}
