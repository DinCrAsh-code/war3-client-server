//============================================================================
//  zlib 1.1.4, infcodes.c - inflate_codes_free.  Own TU: infblock.c's
//  inflate_blocks_reset calls it for real.
//============================================================================
#include "zlibint.h"

//----------------------------------------------------------------------------
//  0x6F6B72F0 - hand the codes state back to the stream's allocator.
//
//  Two instructions of the shipped body are the ZFREE macro shuffling its
//  own arguments (`c` has to leave ecx for edx so that z->opaque can take
//  ecx), and the tail is a `jmp` because nothing happens after the call.
//----------------------------------------------------------------------------
void __fastcall inflate_codes_free(inflate_codes_statef* c, z_streamp z)
{
    ZFREE(z, c);
}
