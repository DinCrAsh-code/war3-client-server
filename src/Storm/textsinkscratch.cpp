//============================================================================
//  The two helpers FormatCFloat (0x6F4B76E0, textsink.cpp) calls behind its
//  own naked redirect: a 32-slot scratch-string ring buffer allocator
//  (0x6F4B7610) and the actual decimal formatter it hands the slot to
//  (0x6F6F0360).
//
//  0x6F4B7610 is genuinely shallow and reconstructed below.  0x6F6F0360 is
//  not: 153 instructions of exponent-driven decimal digit extraction behind
//  five more Storm-side leaves (CFloatToInt, CFloatFromInt, CFloat::
//  operator*, and two more with no name or dump of their own - sub_6F6EF330,
//  sub_6F6EF2D0), the classic shape of a hand-rolled dtoa.  Out of this
//  batch's confidence bar for a guessed body; stays a naked redirect.
//============================================================================
#include "textsink.h"

//----------------------------------------------------------------------------
//  0x6F4B7610 - hand out the next 0x40-byte slot of a 32-entry scratch
//  string ring buffer, lazily initialising the buffer's own storage object
//  the first time any slot is asked for.  `retn` with nothing on the stack:
//  no arguments, and `this` (whatever the caller happened to leave in ecx)
//  is never read.
//
//  `g_scratchRingIndex` is the write cursor, `-1` meaning "never
//  initialised"; the wraparound is a plain `% 32` - MSVC's signed-modulo
//  idiom for a power-of-two divisor (`and eax,0x8000001F` then a
//  sign-correction path), needed here because the cursor is a signed `int`
//  and its very first use is `-1 + 1`.
//----------------------------------------------------------------------------
extern int g_scratchRingIndex;      // dword_6FA85960, -1 = uninitialised
extern char* g_scratchRingBase;     // dword_6FAB779C, first slot's address
extern void* g_scratchRingStorage;  // unk_6FAB7790, the storage object
                                     // sub_6F4B7560 initialises

//  0x6F4B7560 - initialise the ring's storage object.  `retn 8`, two stack
//  arguments (0, 0x20 at the one call site here).  No dump in this batch;
//  naked redirect.
struct SScratchRingStorage
{
    void Init(int, int);
};

__declspec(naked) void SScratchRingStorage::Init(int, int)
{
    __asm
    {
        mov     eax, 06F4B7560h
        jmp     eax
    }
}

void* NextScratchRingSlot()
{
    if (g_scratchRingIndex == -1)
        ((SScratchRingStorage*)&g_scratchRingStorage)->Init(0, 0x20);

    g_scratchRingIndex = (g_scratchRingIndex + 1) % 32;
    return g_scratchRingBase + g_scratchRingIndex * 0x40;
}

//----------------------------------------------------------------------------
//  0x6F6F0360 - format the CFloat `*this` into `Buffer` (edx), `Size`
//  decimal places (one stack argument).  `retn 8`.  See the file comment:
//  a real dtoa, out of this batch's scope.
//----------------------------------------------------------------------------
struct SCFloatFormatter
{
    void FormatDecimal(char* buffer, int size);
};

__declspec(naked) void SCFloatFormatter::FormatDecimal(char*, int)
{
    __asm
    {
        mov     eax, 06F6F0360h
        jmp     eax
    }
}
