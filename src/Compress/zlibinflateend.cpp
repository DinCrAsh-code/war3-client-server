//============================================================================
//  zlib 1.1.4, inflate.c - inflateEnd.  Own TU; see zlibint.h.
//============================================================================
#include "zlibint.h"

//----------------------------------------------------------------------------
//  0x6F6B3C80 - tear an inflate stream down.
//
//  The `z->zfree == 0` arm of the guard is zlib's own: this is the one entry
//  point that frees without ever having allocated, so it cannot assume the
//  caller filled the allocator in.
//----------------------------------------------------------------------------
int __fastcall inflateEnd(z_streamp z)
{
    if (z == 0 || z->state == 0 || z->zfree == 0)
        return Z_STREAM_ERROR;

    if (z->state->blocks != 0)
        inflate_blocks_free(z->state->blocks, z);

    ZFREE(z, z->state);
    z->state = 0;
    return Z_OK;
}
