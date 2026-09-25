//============================================================================
//  zlib 1.1.4, infblock.c - inflate_blocks_free.  Own TU; see zlibint.h.
//============================================================================
#include "zlibint.h"

//----------------------------------------------------------------------------
//  0x6F6B6B80 - reset the block state and give it, its window and its huft
//  pool back to the stream's allocator.
//----------------------------------------------------------------------------
int __fastcall inflate_blocks_free(inflate_blocks_statef* s, z_streamp z)
{
    inflate_blocks_reset(s, z, (uLong*)0);
    ZFREE(z, s->window);
    ZFREE(z, s->hufts);
    ZFREE(z, s);
    return Z_OK;
}
