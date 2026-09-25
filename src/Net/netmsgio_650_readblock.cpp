//============================================================================
//  0x6F650770 - read this message's own twenty-byte block through the
//  CDataStore alias at 0x6F4C3420.  Four instructions: the size, the
//  destination the caller left in edx, and a tail call.
//============================================================================
#include "cdatastore.h"


void __fastcall NetMsgReadBlock20(CDataStore* self, void* dest)
{
    self->ReadRawAlias(dest, 0x14);
}
