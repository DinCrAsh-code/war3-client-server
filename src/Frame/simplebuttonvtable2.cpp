//============================================================================
//  0x6F603890 - CSimpleButton's own slot 2 (+0x08): the deleting
//  destructor.  Call the real destructor (0x6F603800, thunked - see
//  RealDestructorBody's own comment) and, only when the caller asked for it
//  (bit 0 of `deleteFlags`), free the block.  Returns `this`.
//============================================================================
#include "frame.h"
#include "framethunks.h"

void* CSimpleButton::Destroy2(int deleteFlags)
{
    RealDestructorBody();

    if (deleteFlags & 1)
        SMemFree(this, "delete", -1, 0);

    return this;
}
