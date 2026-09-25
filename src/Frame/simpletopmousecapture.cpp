//============================================================================
//  0x6F60B410 - the mouse-capture handler CSimpleTop::RegisterShortcuts binds
//  to event slot 12 (simpletopshortcuts.cpp:51).
//
//  This is what sets CSimpleTop::m_pCapturedFrame, and it is the only thing in
//  the binary that can set it non-null: it hit-tests draw layer 2 with a
//  stack-built CMouseEvent, stores the first frame that answers, then tells the
//  outgoing frame slot 24 (NotifyCaptureLost) and the incoming one slot 23
//  (NotifyCaptureGained) - the exact dispatch pair frame.h:883-886 predicted.
//
//  Confirmed live on 2026-08-29, not just read: with clicks delivered into the
//  window, 0x6F60B410 took 16 hits - 11 returning to 0x6F60C845 (the
//  AddToLayerRegions replay) and 5 to 0x6F63253D, the return of
//  PostQueuedInputMessage's own subscriber call at 0x6F63253B - and the store
//  at 0x6F60B4BC took 3, twice acquiring a frame (edx=0 -> eax=0x0A5E04D8) and
//  once releasing the same one back to 0, all on this=0x098C0088, the live
//  CSimpleTop singleton.
//
//  Two things about the signature, because the old thunk had them wrong:
//  it takes TWO register arguments (the raw event in ecx, the CSimpleTop in
//  edx, since RegisterEventHandler is handed `(int)this` as the context), and
//  it returns int - 1 when nothing ends up captured, 0 otherwise.
//
//  `esi` is reused: it is the copy source for the cached event and then the
//  layer loop's index, which is why the shipped stream has an `xor esi, esi`
//  in the middle of the prologue work.
//============================================================================
#include <string.h>

#include "frame.h"
#include "framethunks.h"
#include "mouseevent.h"

int __fastcall FrameShortcutHandler_6F60B410(const void* rawEvent,
                                             CSimpleTop* top)
{
    CMouseEvent ev(rawEvent);

    CSimpleFrame* old = top->m_pCapturedFrame;

    //  Nine dwords into m_8D4..m_8F4 - the cached event AddToLayerRegions
    //  replays through this same handler when a layer changes and m_168 is set.
    memcpy(&top->m_8D4, rawEvent, 9 * sizeof(int));
    top->m_168 = 1;

    //  SIGNED on purpose, even though m_used is unsigned: the shipped stream
    //  branches with `jle`/`jl`, not `jbe`/`jb`.  Taking the field's own type
    //  here generated the unsigned pair and cost two instructions plus the
    //  loop's whole comparison shape (verify diff, 2026-08-30).
    CFramePriorityArray* layer = &top->m_layers[2];
    int count = (int)layer->m_used;

    ev.m_08 = 0x400500CA;

    CSimpleFrame* found = 0;
    for (int i = 0; i < count; ++i)
    {
        CSimpleFrame* frame = layer->m_data[i]->m_pFrame;
        if (frame->HitTestMessage(&ev))
        {
            found = frame;
            break;
        }
    }

    if (found != old)
    {
        top->m_pCapturedFrame = found;
        if (old)
            old->NotifyCaptureLost();
        if (found)
            found->NotifyCaptureGained();
    }

    return found == 0;
}
