//============================================================================
//  0x6F7E8010 - the fourth one-instruction alias in this module, onto
//  0x6F7E7F30.  Same shape and same reasoning as asyncfilealiases.cpp's
//  three; its target is outside these dumps, so the jump goes through the
//  redirect asyncfilethunks.cpp carries for it.
//============================================================================
#include "asyncfile.h"

void __fastcall AsyncFileReset(ASYNCFILE* file);   // 0x6F7E7F30

__declspec(naked) void __fastcall AsyncFileResetAlias(ASYNCFILE*)
{
    __asm { jmp AsyncFileReset }
}
