//============================================================================
//  zlib 1.1.4, infblock.c - inflate_blocks_reset.  Own TU: inflate_blocks_free
//  calls it for real, and at /Ob2 one TU would inline it away.
//============================================================================
#include "zlibint.h"

//----------------------------------------------------------------------------
//  0x6F6B6050 - put a block state back to TYPE, freeing whatever the mode it
//  is leaving had allocated.
//
//  The two mode tests are written as zlib writes them - two separate `if`s,
//  not one switch - because the shipped code tests `mode` twice, reloading it
//  for the second test.
//----------------------------------------------------------------------------
void __fastcall inflate_blocks_reset(inflate_blocks_statef* s, z_streamp z,
                                     uLong* c)
{
    if (c != 0)
        *c = s->check;

    if (s->mode == IBS_BTREE || s->mode == IBS_DTREE)
        ZFREE(z, s->sub.trees.blens);

    if (s->mode == IBS_CODES)
        inflate_codes_free(s->sub.decode.codes, z);

    s->mode = IBS_TYPE;
    s->bitk = 0;
    s->bitb = 0;
    s->read = s->write = s->window;

    if (s->checkfn != 0)
        z->adler = s->check = (*s->checkfn)(0, (const Bytef*)0, 0);
}
