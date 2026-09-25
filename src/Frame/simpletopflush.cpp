//============================================================================
//  CSimpleTop::Method_0x2C (vtable slot 11, 0x6F60C4B0) and its own drain
//  helper (0x6F60C0A0) - docs/notes/pause-mouse-capture.md's second-to-last
//  named lead in the search for CSimpleTop::m_pCapturedFrame's write site.
//
//  Neither this function nor anything in its full depth-3 closure
//  (asm/sub_6F60C4B0_0x6F60C4B0_calltree_asm.md) ever references +0x16C -
//  see frame.h's own comment on the declaration for the rest of that
//  finding.
//============================================================================
#include "frame.h"
#include "framethunks.h"

//----------------------------------------------------------------------------
//  0x6F60C0A0 - unlink and free every node still on m_pendingHide, one at a
//  time.  The raw tail read (no Tail() clamp - this function's own `jle`
//  tests the field directly) and TSLink<SIMPLEFRAMENODE>::Unlink() are the
//  same shapes AddFrame/RemoveChildNode's own list-splice code already
//  establishes for this exact node type; g_simpleFrameNodeAllocator is the
//  same pool they allocate from.
//----------------------------------------------------------------------------
void __fastcall DrainPendingHideList(TSList<SIMPLEFRAMENODE, 0>* list)
{
    //  A `for(;;)` with a *leading* `if (…) break;`, not a `while` or a
    //  guarded `do`/`while`: the shipped stream has exactly one copy of the
    //  tail test, entered from the top on the first pass and reached again
    //  by the loop's own single unconditional `jmp` back to that same
    //  block - neither a `while` (which this compiler rotates into two
    //  copies of the test here) nor a `do`/`while` behind a separate guard
    //  reproduces that; this shape does.
    for (;;)
    {
        SIMPLEFRAMENODE* node = (SIMPLEFRAMENODE*)list->m_terminator.m_prevlink;
        if ((int)node <= 0)
            break;

        node->m_link.Unlink();
        g_simpleFrameNodeAllocator.Free(node, 0, 0);
    }
}

//----------------------------------------------------------------------------
//  Slot 11 (+0x2C).  See frame.h's own comment on the declaration for the
//  signature correction and the full per-step breakdown; this is that
//  breakdown as code.
//
//  The per-region dispatch below is deliberately a raw vtable-slot call
//  (framechildbroadcast.cpp's own established idiom for this shape) and not
//  CSimpleFrame::Method_0x34(int) by name: this call site's own argument is
//  a genuine float (`fstp`, not `push`), which does not agree with that
//  virtual's declared `int` parameter - the two are different C-level
//  signatures reaching the identical vtable slot from different callers,
//  and only this one is this session's own evidence.
//----------------------------------------------------------------------------
int CSimpleTop::Method_0x2C(float a, float b, const SFrameRect* c)
{
    //  The whole hide-and-drain pass is skipped outright when the raw tail
    //  field is not positive - not just the per-node loop.  Written as an
    //  `int` walk with a `> 0` test (AddFrame's own established idiom for
    //  this exact node type, simpletopframes.cpp) rather than `Tail()`'s
    //  `T*`-typed, `!= 0`-tested return: a `T*` loop variable re-tests with
    //  `jne` where the shipped stream's own back-edge test is `jg`,
    //  because `Prev()` (unlike `Tail()`) never clamps a negative result to
    //  null.  The target's own stream carries one further, textually
    //  redundant test ahead of `Tail()`'s own inlined clamp (both on the
    //  same raw field) that a second source-level guard did not reproduce -
    //  the optimiser folds the two back down to one either way; IDENTICAL,
    //  not a missing store or a different value.
    int link = (int)m_pendingHide.Tail();
    if (link > 0)
    {
        while (link > 0)
        {
            SIMPLEFRAMENODE* node = (SIMPLEFRAMENODE*)link;
            CSimpleFrame* child = (CSimpleFrame*)node->m_pFrame;
            child->m_shown = 0;
            child->NotifyHidden();
            child->Destroy2(1);
            link = (int)m_pendingHide.Prev(node);
        }
        DrainPendingHideList(&m_pendingHide);
    }

    //  This frame's own on-screen rect corner (CLayoutFrame::m_rect[0]/[1],
    //  reached through CFrame's own CLayoutFrame base) - *not* a cursor
    //  position, contrary to the prior session's cold-read guess.  The
    //  shipped stream stores y0 to the lower address and x0 to the higher
    //  one, which is why this local's fields are declared in that order.
    struct { float y, x; } origin;
    origin.y = m_rect[1];
    origin.x = m_rect[0];
    SubmitRegionTransform(c, &origin, b);

    typedef void (__thiscall *RegionSlot0x34Fn)(void*, float);
    for (int layer = 0; layer < 10; ++layer)
    {
        int raw = m_frameLayers[layer].m_terminator.m_prevlink;
        while (raw > 0)
        {
            REGIONNODE* r = (REGIONNODE*)raw;
            RegionSlot0x34Fn f =
                (RegionSlot0x34Fn)(*(void***)r->m_pRegion)[0x34 / 4];
            f(r->m_pRegion, a);
            raw = r->m_link.m_prevlink;
        }
    }

    ProcessLayoutQueue();
    return 1;
}
