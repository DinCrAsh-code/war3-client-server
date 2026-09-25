//============================================================================
//  0x6F7E7300 and 0x6F7E7310 - the async-file layer's own two-line wrappers
//  around OsNet's subsystem gate: start bit 4 with no options, stop bit 4.
//
//  Both are tail jumps through OsNet's one-instruction aliases (0x6F6E7070
//  and 0x6F6E4FC0), which is why the argument registers are loaded and
//  nothing else happens.
//============================================================================
#include "ostcp.h"

int  __fastcall OsNetInitializeAlias(int, unsigned int);
void __fastcall OsNetShutdownAlias(unsigned int);

//  Naked, because the image tail-jumps through OsNet's own one-instruction
//  aliases with the arguments already in registers: a C++ call sets them up
//  the same way but then emits a call and a return instead of the jump.
__declspec(naked) void __fastcall AsyncFileStartOsNet()
{
    __asm
    {
        mov     edx, 4
        xor     ecx, ecx
        jmp     OsNetInitializeAlias
    }
}

__declspec(naked) void __fastcall AsyncFileStopOsNet()
{
    __asm
    {
        mov     ecx, 4
        jmp     OsNetShutdownAlias
    }
}
