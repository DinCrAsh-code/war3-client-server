//============================================================================
//  0x6F4BC1D0 - PushIndexedFreeListEntry.  See indexedfreelist.h.
//
//  Own translation unit so that STwoIndexedFreeLists::Push's own real call
//  into this (indexedfreelistpush.cpp) is not inlined away by this build.
//============================================================================
#include "indexedfreelist.h"

void __stdcall PushIndexedFreeListEntry(int index, int /*unused*/,
                                        SIndexedFreeListOwner* obj,
                                        int* headIndex, int* remaining)
{
    SIndexedFreeListRecord* record = &obj->m_records[index];
    record->m_field0 = *headIndex;
    record->m_field4 = 0;
    *headIndex = index;
    *remaining -= 1;
}
