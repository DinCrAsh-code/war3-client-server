//============================================================================
//  0x6F49ABB0 - DirectionBetween: which of kPathDir{North,South,East,West}
//  point from `from` towards `to`, OR'd together (both, one, or neither
//  axis can contribute a bit).
//
//  The shipped code takes both points in esi/edi rather than any argument
//  register or the stack - not a convention any MSVC parameter list can
//  ask for - so this is __asm-bodied and __declspec(naked): the C++
//  parameters exist only so a caller has something to name, and every
//  caller has to load esi/edi itself before calling in (see this file's
//  own callers for the shape). The bit literals below are
//  kPathDirWest/East/South/North spelled numerically - referencing the
//  named `const int` from inside __asm emits a symbol operand instead of
//  the immediate the shipped code has.
//============================================================================
#include "pathfind.h"

__declspec(naked) int __fastcall DirectionBetween(const int* from, const int* to)
{
    __asm
    {
        mov     ecx, [esi]
        mov     edx, [edi]
        xor     eax, eax
        cmp     ecx, edx
        jge     xNotLess
        mov     eax, 8
        jmp     xDone
xNotLess:
        jle     xDone
        mov     eax, 2
xDone:
        mov     ecx, [esi+4]
        mov     edx, [edi+4]
        cmp     ecx, edx
        jge     yNotLess
        or      eax, 1
        ret
yNotLess:
        jle     yDone
        or      eax, 4
yDone:
        ret
    }
}
