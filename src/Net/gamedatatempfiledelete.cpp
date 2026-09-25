//============================================================================
//  0x6F534990 - delete this stream's own temporary file, if it has one, and
//  forget the name.
//============================================================================
#include "os.h"

void __fastcall GameDataDeleteTempFile(void* self)
{
    char* name = (char*)self + 0x104;

    if (name[0] == 0)
        return;

    OsDeleteFile(name);
    name[0] = 0;
}
