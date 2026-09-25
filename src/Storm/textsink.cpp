//============================================================================
//  0x6F4B76E0 - the one remaining out-of-scope half of the debug text sink
//  (see textsink.h), redirected to its real address rather than
//  reconstructed: forty-four instructions of decimal formatting behind two
//  more unreconstructed helpers and a Storm ordinal.
//
//  Its file-mate LogFormatted (0x6F4B73F0) is now reconstructed in
//  textsinklog.cpp - it turned out to do no formatting at all.
//============================================================================
#include "textsink.h"

__declspec(naked) const char* __fastcall FormatCFloat(const class CFloat*,
                                                       int, int)
{
    __asm
    {
        mov     eax, 06F4B76E0h
        jmp     eax
    }
}
