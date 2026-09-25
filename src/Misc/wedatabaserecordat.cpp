//============================================================================
//  0x6F6F8D50 - CItem vtable slot 32/104 closure (depth 7).
//  CWEDatabase::RecordAt - see wedatabase.h for the two fields this touches.
//============================================================================
#include "wedatabase.h"

void* CWEDatabase::RecordAt(int index)
{
    if (index > 0)
        return *(void**)((char*)m_records + (index - 1) * 0x108);
    return m_defaultRecord;
}
