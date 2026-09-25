//============================================================================
//  0x6F498260 - CPathSearch::TestTypeRecordBit.
//============================================================================
#include "pathfind.h"

int CPathSearch::TestTypeRecordBit(int index) const
{
    return m_typeRecords[index * 3] & 1;
}
