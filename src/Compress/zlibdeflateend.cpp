//============================================================================
//  zlib 1.1.4, deflate.c - deflateEnd.  Own TU; see zlibint.h.
//============================================================================
#include "zlibint.h"

//----------------------------------------------------------------------------
//  0x6F6B2B70 - tear a deflate stream down.
//
//  The four TRY_FREEs are in zlib's order, which is the reverse of the order
//  deflateInit2_ allocates them in, and the shipped body frees head before
//  prev for exactly that reason - it is not a transcription slip.
//
//  The return is zlib's own one-liner: a stream torn down mid-block loses
//  data, so BUSY_STATE answers Z_DATA_ERROR and everything else Z_OK.  The
//  shipped code computes it branchlessly (`setnz` / `sub 1` / `and -3`),
//  which is what the conditional expression emits.
//----------------------------------------------------------------------------
int __fastcall deflateEnd(z_streamp strm)
{
    int status;

    if (strm == 0 || strm->state == 0)
        return Z_STREAM_ERROR;

    status = ((deflate_state*)strm->state)->status;
    if (status != INIT_STATE && status != BUSY_STATE && status != FINISH_STATE)
        return Z_STREAM_ERROR;

    //  Deallocate in reverse order of allocations.
    TRY_FREE(strm, ((deflate_state*)strm->state)->pending_buf);
    TRY_FREE(strm, ((deflate_state*)strm->state)->head);
    TRY_FREE(strm, ((deflate_state*)strm->state)->prev);
    TRY_FREE(strm, ((deflate_state*)strm->state)->window);

    ZFREE(strm, strm->state);
    strm->state = 0;

    return status == BUSY_STATE ? Z_DATA_ERROR : Z_OK;
}
