//============================================================================
//  0x6F5D5D10 - a bare tail forward into RCString::~RCString (0x6F4C48F0,
//  already reconstructed in src/Misc/rcstringdtor.cpp).  Every one of its
//  many callers is itself a naked one-instruction `jmp` (a per-class
//  scalar-destructor / exception-unwind funclet whose only local work is
//  tearing down an embedded RCString member), so this is one shared hop in
//  a chain of tail jumps rather than a reconstruction of anything with a
//  body of its own - not a THUNK in the "redirect to unreconstructed
//  shipped code" sense (the real target already has a C++ body here), but
//  the identical naked-jmp shape, so it is written the same way.
//============================================================================
#include "rcstring.h"

__declspec(naked) void RCStringDtorForward()
{
    __asm
    {
        jmp RCString::~RCString
    }
}
