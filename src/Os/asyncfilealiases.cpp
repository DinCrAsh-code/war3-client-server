//============================================================================
//  0x6F7E7FF0, 0x6F7E8000 and 0x6F7E8B10 - three one-instruction aliases the
//  image carries for AsyncFileFlush, AsyncFileClose and AsyncFileOpen.  Each
//  is a bare `jmp` to the real body and each has its own dump heading, so
//  each is reconstructed rather than folded away.
//
//  Naked, because there is no C++ that produces a single relative jump and
//  nothing else: a forwarding call would add a prologue, and a tail call
//  through a plain wrapper would still set up the argument registers the
//  caller has already set up.
//============================================================================
#include "asyncfile.h"

__declspec(naked) void __fastcall AsyncFileFlushAlias(ASYNCFILE*)
{
    __asm { jmp AsyncFileFlush }
}

__declspec(naked) void __fastcall AsyncFileCloseAlias(ASYNCFILE*)
{
    __asm { jmp AsyncFileClose }
}

__declspec(naked) void __fastcall AsyncFileOpenAlias(const char*, int)
{
    __asm { jmp AsyncFileOpen }
}
