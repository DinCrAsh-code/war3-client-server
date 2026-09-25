//============================================================================
//  0x6F6E7070 and 0x6F6E4FC0 - the image's one-instruction aliases for
//  OsNetInitialize and OsNetShutdown.  Same shape and same reason as
//  asyncfilealiases.cpp's three.
//============================================================================
#include "ostcp.h"

__declspec(naked) int __fastcall OsNetInitializeAlias(int, unsigned int)
{
    __asm { jmp OsNet::OsNetInitialize }
}

__declspec(naked) void __fastcall OsNetShutdownAlias(unsigned int)
{
    __asm { jmp OsNet::OsNetShutdown }
}
