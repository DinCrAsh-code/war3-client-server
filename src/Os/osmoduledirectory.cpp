//============================================================================
//  0x6F6C6000 - the directory the process image lives in: the module file
//  name with its last component cut off.
//============================================================================
#include "os.h"

void __fastcall OsGetModuleDirectory(char* buffer, DWORD size)
{
    OsGetModuleFileName(buffer, size);
    OsPathStripFileName(buffer);
}
