//============================================================================
//  0x6F4C90A0 - Database.cpp's own teardown: release one tagged record, a
//  `CDataAllocator` instance releasing through the shared ReleaseAllBlocks
//  (Net/dataallocator.cpp, 0x6F4C1A60) - see spritetypetagteardown.cpp's
//  own header comment for why this file no longer routes through a
//  locally-declared `TypeTagRecord::Destroy` duplicate.
//============================================================================
#include "dataallocator.h"

extern CDataAllocator g_databaseTag;   // unk_6FAB7D18

void TeardownDatabaseTypeTag()
{
    g_databaseTag.ReleaseAllBlocks(".\\Database.cpp", 0x28D, 1);
}
