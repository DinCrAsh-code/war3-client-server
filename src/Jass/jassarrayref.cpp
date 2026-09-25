//============================================================================
//  0x6F45A3D0 - JassArrayRefSite::AcquireArray.  `retn 8`.
//
//  Kept a redirect rather than the real body this session drafted (drop
//  the cached array unless it is array-shaped and being kept, then
//  lazily allocate-and-stamp-`??_7JassArray@@6B@`-or-reuse it and resize
//  through JassArray::SetCount) for one reason only: stamping JassArray's
//  own vtable needs its real .rdata address, which this session's
//  `ida_query names JassArray` submission never got an answer to (worker
//  offline - CLAUDE.md's own "exit 3 is never an answer"), and the
//  `extern void* const g_vftJassArray` idiom
//  docs/msvc-vc8-idioms.md's "A vtable whose class you cannot declare"
//  documents needs a funcmap.DATA row bound to that real address before
//  link_check.py will pass - so committing the drafted body would leave
//  a live UNEXPECTED unresolved external for its own `g_vftJassArray`.
//  The draft itself scored 0.835 (43/56) against this address, matching
//  the shipped stream case by case except register allocation - see
//  jassarray.h's own JassArrayRefSite class for the field layout it
//  established, which the drafted body already validated and is
//  reusable once the address lands.
//============================================================================
#include "jassarray.h"

__declspec(naked) void JassArrayRefSite::AcquireArray(unsigned int, int)
{
    __asm
    {
        mov eax, 06F45A3D0h
        jmp eax
    }
}
