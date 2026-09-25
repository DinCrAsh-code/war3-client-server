//============================================================================
//  0x6F60B800 / 0x6F60B930 - CSimpleTop::RegisterShortcuts and
//  ::UnregisterShortcuts: bind or drop the keyboard shortcuts a draw layer's
//  frames answer to.
//
//  Layer 4 is the "all of them" case and is tested with its own compare
//  ahead of the four-case jump table, which is what the shipped code has.
//  Each recurses into itself for layers 0-3.
//============================================================================
#include "frame.h"
#include "framethunks.h"

void CSimpleTop::RegisterShortcuts(int layer)
{
    if (layer == 4)
    {
        if (m_shortcutsOn != 0)
            return;

        unsigned int i = 0;
        CFramePriorityArray* list = &m_layers[0];
        do
        {
            if (list->m_used > 0)
                RegisterShortcuts(i);

            i += 1;
            list += 1;
        }
        while (i < 4);

        m_shortcutsOn = 1;
        return;
    }

    switch (layer)
    {
    case 0:
        RegisterEventHandler(1, (void*)FrameShortcutHandler_6F60B150,
                             (int)this, 1.0f);
        break;
    case 1:
        RegisterEventHandler(8, (void*)FrameShortcutHandler_6F60B200,
                             (int)this, 1.0f);
        RegisterEventHandler(9, (void*)FrameShortcutHandler_6F60B2B0,
                             (int)this, 1.0f);
        RegisterEventHandler(10, (void*)FrameShortcutHandler_6F60B360,
                             (int)this, 1.0f);
        break;
    case 2:
        RegisterEventHandler(12, (void*)FrameShortcutHandler_6F60B410,
                             (int)this, 1.0f);
        RegisterEventHandler(11, (void*)FrameShortcutHandler_6F60B500,
                             (int)this, 1.0f);
        RegisterEventHandler(14, (void*)FrameShortcutHandler_6F60B590,
                             (int)this, 1.0f);
        break;
    case 3:
        RegisterEventHandler(16, (void*)FrameShortcutHandler_6F60B620,
                             (int)this, 1.0f);
        break;
    }
}

void CSimpleTop::UnregisterShortcuts(int layer)
{
    if (layer == 4)
    {
        if (m_shortcutsOn == 0)
            return;

        unsigned int i = 0;
        CFramePriorityArray* list = &m_layers[0];
        do
        {
            if (list->m_used > 0)
                UnregisterShortcuts(i);

            i += 1;
            list += 1;
        }
        while (i < 4);

        m_shortcutsOn = 0;
        return;
    }

    switch (layer)
    {
    case 0:
        UnregisterEventHandler(1, (void*)FrameShortcutHandler_6F60B150,
                               this, -1);
        break;
    case 1:
        UnregisterEventHandler(8, (void*)FrameShortcutHandler_6F60B200,
                               this, -1);
        UnregisterEventHandler(9, (void*)FrameShortcutHandler_6F60B2B0,
                               this, -1);
        UnregisterEventHandler(10, (void*)FrameShortcutHandler_6F60B360,
                               this, -1);
        break;
    case 2:
        UnregisterEventHandler(12, (void*)FrameShortcutHandler_6F60B410,
                               this, -1);
        UnregisterEventHandler(11, (void*)FrameShortcutHandler_6F60B500,
                               this, -1);
        UnregisterEventHandler(14, (void*)FrameShortcutHandler_6F60B590,
                               this, -1);
        break;
    case 3:
        UnregisterEventHandler(16, (void*)FrameShortcutHandler_6F60B620,
                               this, -1);
        break;
    }
}
