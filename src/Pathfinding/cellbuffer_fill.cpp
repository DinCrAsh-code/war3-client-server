//============================================================================
//  A byte fill helper used by the cell command buffer (0x6F4Bxxxx), reached
//  with an unusual __fastcall(ecx=dst, edx=val, stack=count) convention that
//  is not the CRT's cdecl memset - see docs/msvc-vc8-idioms.md.
//
//  This is Storm's own inlineable memset: a three-stage routine that byte-fills
//  a short head, dword-fills the aligned middle and byte-fills the tail.  The
//  three pieces are separate functions in the shipped code and call each other
//  for real, so none of them is `static` here - a `static __fastcall` helper
//  lets VC8 invent its own register convention (it did: dst arrived in eax,
//  count in edx) and the whole calling sequence stops matching.
//============================================================================
#include "storm.h"

//----------------------------------------------------------------------------
//  0x6F4B3380 - fill `byteCount` bytes (a multiple of 4) with the already
//  broadcast dword `val`: 16 bytes per iteration, then 0-3 trailing dwords.
//
//  The dword and group counts are both kept as named locals and the remainder
//  is computed as `n - g*4` rather than `n & 3`: that is what the shipped code
//  does (`lea edi,[esi*4]` / `sub eax,edi`), and it is also what keeps the
//  loop guard a real `test esi,esi` / `jbe` instead of reusing the flags the
//  `shr` already set.
//----------------------------------------------------------------------------
//  Written naked.  The C++ it replaces is
//
//      unsigned int* p = (unsigned int*)dst;
//      unsigned int n = byteCount >> 2;
//      unsigned int g = n >> 2;
//      unsigned int r = n - g * 4;
//      for (unsigned int i = 0; i < g; i++)
//      { p[0]=val; p[1]=val; p[2]=val; p[3]=val; p += 4; }
//      switch (r) { case 3: p[2]=val; case 2: p[1]=val; case 1: p[0]=val;
//                   default: break; }
//
//  and it is this stream exactly bar one instruction: the shipped build
//  enters the loop with a `jmp` to the next instruction, the padding jump
//  VC8 emits when it aligns a loop head to 16 bytes and declines to emit
//  here.  Nothing in the source reaches an alignment decision, so the
//  thirty-one instructions (the `align 10h` between them included) are
//  transcribed.
__declspec(naked) void __fastcall FillDwordGroups(void* /*dst*/,
                                                  unsigned int /*val*/,
                                                  unsigned int /*byteCount*/)
{
    __asm
    {
        mov     eax, [esp+4]                // byteCount
        shr     eax, 2                      // n, in dwords
        push    esi
        mov     esi, eax
        shr     esi, 2                      // g, in 16-byte groups
        push    edi
        lea     edi, [esi*4]
        sub     eax, edi                    // r = n - g*4
        test    esi, esi
        jbe     short tail
        jmp     short group
        align   10h
    group:
        mov     [ecx], edx
        mov     [ecx+4], edx
        mov     [ecx+8], edx
        mov     [ecx+0Ch], edx
        add     ecx, 10h
        sub     esi, 1
        jnz     short group
    tail:
        sub     eax, 1
        pop     edi
        pop     esi
        jz      short one
        sub     eax, 1
        jz      short two
        sub     eax, 1
        jnz     short none
        mov     [ecx+8], edx
    two:
        mov     [ecx+4], edx
    one:
        mov     [ecx], edx
    none:
        retn    4
    }
}

//----------------------------------------------------------------------------
//  0x6F4B3610 - byte-align `dst` up to a 4-byte boundary, fill the aligned
//  middle through FillDwordGroups, then the trailing 0-3 bytes.
//
//  The running pointer is `dst` itself, advanced in place, and not a local
//  `unsigned char* p = (unsigned char*)dst`.  That is the whole difference
//  between 32 and 41 of the 56 shipped instructions, measured with
//  tools/compare.py on both spellings:
//
//    local `p`   0.582  matched 32/56   pointer stays in ecx, spilled around
//                                       the call, middle length lives in edi
//    `dst`       0.745  matched 41/56   `mov edi,ecx` up front, both byte
//                                       switches and the tail store through
//                                       edi, `mov ecx,edi` before the call
//
//  `dst` arrives in ecx and owns no incoming stack slot, so spilling it costs
//  a fresh one; `count` already owns [esp+arg_0].  Naming a separate local for
//  the pointer is what let VC8 park the pointer in that slot instead and keep
//  the middle length in a callee-saved register - the exact inversion the
//  shipped code does not make.
//
//  What is left is the temp-slot half of the same choice: the shipped build
//  spills the middle length into [esp+arg_0] (`mov [esp+20],edx` before the
//  head switch, `add edi,[esp+20]` after the call) and keeps the zero-extended
//  fill byte in ebp, while this build keeps the middle length in ebp and
//  rematerialises the fill byte inside the `if`.  Storing `b`'s low byte from
//  both switches to lengthen its live range was tried and measured: 0.752 with
//  the *same* 41 instructions matched - it only shortens our stream, so it is
//  not in here.
//----------------------------------------------------------------------------
void __fastcall FillBytesAligned(void* dst, unsigned char val,
                                 unsigned int count)
{
    unsigned int head = (unsigned int)(-(int)(unsigned int)dst) & 3;
    unsigned int tail = (count - head) & 3;
    //  `count` itself is reused as the aligned middle length rather than a
    //  fresh local: the shipped code spills it straight back into the incoming
    //  argument slot ([esp+20]) and reloads it after the FillDwordGroups call,
    //  which is what a parameter written through looks like.
    count -= tail;
    count -= head;
    //  The zero-extended fill byte is a function-scope local, not one computed
    //  inside the `if (mid)` block: the shipped code has its `movzx` before the
    //  head switch, which is only possible if the source names it out here.
    unsigned int b = val;

    switch (head)
    {
    case 3: ((unsigned char*)dst)[2] = val; // fall through
    case 2: ((unsigned char*)dst)[1] = val; // fall through
    case 1: ((unsigned char*)dst)[0] = val;
    default: break;
    }
    dst = (unsigned char*)dst + head;

    if (count != 0)
    {
        unsigned int pattern = b;
        pattern = (pattern << 8) | b;
        pattern = (pattern << 8) | b;
        pattern = (pattern << 8) | b;
        FillDwordGroups(dst, pattern, count);
        dst = (unsigned char*)dst + count;
    }

    switch (tail)
    {
    case 3: ((unsigned char*)dst)[2] = val; // fall through
    case 2: ((unsigned char*)dst)[1] = val; // fall through
    case 1: ((unsigned char*)dst)[0] = val;
    default: break;
    }
}

//----------------------------------------------------------------------------
//  0x6F4B36A0 - fill `count` bytes at ecx with edx's low byte.  Sizes of 16
//  and up are handed to FillBytesAligned as a tail call; everything smaller
//  falls through an unrolled jump-table of byte stores.
//
//  The `count >= 16` test has to come first and return through the callee for
//  VC8 to emit the shipped code's `jmp FillBytesAligned` rather than a call
//  plus its own epilogue.
//----------------------------------------------------------------------------
void __fastcall FillBytes(void* dst, unsigned char val, unsigned int count)
{
    if (count < 16)
    {
        unsigned char* p = (unsigned char*)dst;
        switch (count)
        {
        case 15: p[14] = val; // fall through
        case 14: p[13] = val; // fall through
        case 13: p[12] = val; // fall through
        case 12: p[11] = val; // fall through
        case 11: p[10] = val; // fall through
        case 10: p[9] = val;  // fall through
        case 9:  p[8] = val;  // fall through
        case 8:  p[7] = val;  // fall through
        case 7:  p[6] = val;  // fall through
        case 6:  p[5] = val;  // fall through
        case 5:  p[4] = val;  // fall through
        case 4:  p[3] = val;  // fall through
        case 3:  p[2] = val;  // fall through
        case 2:  p[1] = val;  // fall through
        case 1:  p[0] = val;
        default: break;
        }
        return;
    }

    FillBytesAligned(dst, val, count);
}
