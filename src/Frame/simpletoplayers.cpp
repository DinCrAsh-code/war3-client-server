//============================================================================
//  0x6F60BCE0 / 0x6F60BD50 - CSimpleTop::AddFrameToLayer and
//  ::RemoveFrameFromLayer: one frame's membership of one draw layer.
//
//  ".\\CSimpleTop.cpp" is the allocation tag on the FRAMEPRIORITY record and
//  is what names this class.  A priority of -1 means "use the frame's own
//  depth", which is how nesting decides draw order.
//============================================================================
#include "frame.h"
#include "framethunks.h"

static const char kSimpleTopFile[] = ".\\CSimpleTop.cpp";
static const char kDeleteTag[] = "delete";

void CSimpleTop::AddFrameToLayer(CSimpleFrame* frame, int layer, int priority)
{
    //  The layer's address is formed *above* the guard, which is where the
    //  shipped code has it (docs/msvc-vc8-idioms.md, "Computation the shipped
    //  code does above a guard").
    CFramePriorityArray* list = &m_layers[layer];

    if (priority == -1)
        priority = frame->m_depth;

    unsigned int before = list->m_used;

    FRAMEPRIORITY* entry = (FRAMEPRIORITY*)SMemAlloc(8, kSimpleTopFile,
                                                     0x8E, 0);
    entry->m_pFrame = frame;
    entry->m_priority = priority;
    list->Insert(entry);

    if (before == 0)
        RegisterShortcuts(layer);
}

void CSimpleTop::RemoveFrameFromLayer(CSimpleFrame* frame, int layer)
{
    CFramePriorityArray* list = &m_layers[layer];

    unsigned int count = list->m_used;
    if (count <= 0)
        return;

    unsigned int index = 0;
    if (count <= 0)
        return;

    FRAMEPRIORITY** cursor = list->m_data;

    do
    {
        FRAMEPRIORITY* entry = *cursor;

        if (entry->m_pFrame == frame)
        {
            list->EraseAt(index);
            SMemFree(entry, kDeleteTag, -1, 0);

            if (frame == m_pCapturedFrame)
            {
                frame->NotifyCaptureLost();
                m_pCapturedFrame = 0;
            }

            if (index < count && count == 1)
                UnregisterShortcuts(layer);

            return;
        }

        index += 1;
        cursor += 1;
    }
    while (index < count);
}
