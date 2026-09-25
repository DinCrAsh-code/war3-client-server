//============================================================================
//  0x6F509420 - the 0x30-byte record at this index in the module's own
//  table, or null for the -1 no-record marker.  The stride is written as the
//  shipped code forms it: three, shifted left four.
//============================================================================
#include "modelpick.h"

void* __fastcall PickRecordAt_6F509420(unsigned int index)
{
    if (index == 0xFFFFFFFF)
        return 0;

    return g_pickRecordTable + ((index + index * 2) << 4);
}
