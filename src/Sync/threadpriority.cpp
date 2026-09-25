//============================================================================
//  Three one-call OS wrappers in the 0x6F6CE2xx neighbourhood, none of them
//  reaching each other - grouped here rather than split into three
//  single-function files the way this repo's other leaf-import wrappers
//  usually are, since none is more than a couple of instructions.
//============================================================================
#include <windows.h>
#include <float.h>

//----------------------------------------------------------------------------
//  0x6F6CE230 - reset the FPU's exception state and control word: clear
//  every pending exception, then mask all six (`0xFFFFF`, `_MCW_EM |
//  _EM_DENORMAL`-shaped) and select 53-bit (double) precision, round-to-
//  nearest (`0x9001F` - the same "some caller upstream left the FPU in a
//  non-default mode, put it back" idiom the CRT's own `_fpreset` uses).
//----------------------------------------------------------------------------
void ResetFpuControlWord()
{
    _clearfp();
    _control87(0x9001F, 0xFFFFF);
}

//----------------------------------------------------------------------------
//  0x6F6CE250 / 0x6F6CE260 - the calling thread's own priority, read and
//  written through the real Win32 pair rather than a cached value.
//----------------------------------------------------------------------------
int GetCurrentThreadPriority()
{
    return GetThreadPriority(GetCurrentThread());
}

BOOL __fastcall SetCurrentThreadPriority(int nPriority)
{
    return SetThreadPriority(GetCurrentThread(), nPriority);
}
