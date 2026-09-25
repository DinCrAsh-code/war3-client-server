//============================================================================
//  0x6F607E80 - CFrame::AddLight.
//
//  Sixteen bytes straight from Storm rather than from a pool - the RTTI name
//  ".?AULIGHTNODE@CFrame@@" is what the allocator is handed - zeroed by hand,
//  spliced onto the *tail* of the light list, counted, and then handed the
//  light it speaks for with one reference taken.
//
//  The tail splice is where the list's link offset is needed for real, which
//  is why the shipped stream loads +0x150 (the offset field) and falls back to
//  recovering it from the head when it is still negative - see
//  TSList::LinkToTail in Storm/storm.h.
//============================================================================
#include "frame.h"
#include "framethunks.h"

#include "refcnt.h"

void CFrame::AddLight(void* light, int flags)
{
    //  Placement-new, not an explicit null test: the shipped stream's
    //  `xor ebx,ebx` on the failed path is the new-expression's own answer
    //  for a null buffer, and the four zeroing stores are the constructor's.
    LIGHTNODE* node = new ((LIGHTNODE*)SMemAlloc(
        0x10, ".?AULIGHTNODE@CFrame@@", -2, 8)) LIGHTNODE;

    m_lights.LinkToTail(node);
    m_lightCount++;

    node->m_pLight = ((TRefCnt*)light)->AddRefOrNull();
    node->m_flags = flags;
}
