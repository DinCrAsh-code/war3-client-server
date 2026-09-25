//============================================================================
//  0x6F7E89A0 - a pure forwarder onto AsyncFileQueue: the same two register
//  arguments passed through untouched and the same five stack arguments
//  re-pushed.  Both are `retn 14h`, so the forwarder pushes exactly what the
//  callee pops and pops exactly what its own caller pushed.
//
//  It exists in the image and has its own dump heading, so it is
//  reconstructed rather than folded away.
//============================================================================
#include "asyncfile.h"

//  Naked: written as a C++ forwarder MSVC turns the whole thing into a
//  single tail `jmp`, because ecx, edx and the five stack slots are already
//  exactly where the callee wants them.  The image does not: it re-pushes
//  all five and makes a real call, so the frame is transcribed.
__declspec(naked) unsigned int __fastcall AsyncFileQueueForward(
    ASYNCFILE*, int, int, int, int, int, int)
{
    __asm
    {
        mov     eax, [esp + 0x14]
        push    eax
        mov     eax, [esp + 0x14]
        push    eax
        mov     eax, [esp + 0x14]
        push    eax
        mov     eax, [esp + 0x14]
        push    eax
        mov     eax, [esp + 0x14]
        push    eax
        call    AsyncFileQueue
        retn    0x14
    }
}
