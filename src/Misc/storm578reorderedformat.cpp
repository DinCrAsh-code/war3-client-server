//============================================================================
//  CUnit batch-16 (slot-103 closure).
//
//  0x6F30A330 - a thin `Storm_578` (SStrPrintf-family) wrapper that
//  reorders its own three arguments: `__fastcall(format, size, dest)`,
//  one stack argument (`retn 4`), forwarding as `Storm_578(dest, size,
//  format)` with no varargs of its own.  The argument roles are settled
//  by the push order rather than guessed: the shipped body pushes its own
//  `ecx` first (Storm_578's *last*/third argument, `format`), its own
//  `edx` second (the *middle* argument, `size`), and its one stack
//  argument last (Storm_578's *first* argument, `dest`) - x86 cdecl
//  pushes right-to-left, so the first-pushed operand is the last
//  parameter and vice versa.
//============================================================================
#include "text.h"

int __fastcall Storm578ReorderedFormat(const char* format, unsigned int size,
                                       char* dest)
{
    return Storm_578(dest, size, format);
}
