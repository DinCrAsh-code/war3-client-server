//============================================================================
//  0x6F379EA0 - RefreshSelectedUnitVisuals.
//
//  Re-place the selection visual of every currently selected widget, and
//  then of the focus widget.  Each entry is validated the same way: its own
//  CAgent handle is resolved, the object behind it has to carry the '+agl'
//  type tag and must not be busy.  That test is one branch and not three -
//  the `setnz`/`sub 1`/`and` checked cast documented in
//  docs/msvc-vc8-idioms.md and already used by
//  Widget/widgetpathref.cpp.
//
//  The last entry is written as a separate statement and not as a
//  thirteenth loop iteration, because the shipped code tail-jumps into
//  0x6F2C74B0 for it and calls it inside the loop.
//
//  Own translation unit: 0x6F2C74B0 is a real call and a different shipped
//  module.
//============================================================================
#include "gameui.h"
#include "gameuiagenthandle.h"

//  0x6F2C74B0 - gameuiselectableplacement.cpp.  Declared locally under the
//  same class name so both sides reach the one symbol.
class CSelectable { public: void RefreshSelectionVisual(); };

//: dword_6FAB62C8 / dword_6FAB62CC - the highlighted-widget array's count
//: and its element pointer.  Named as two separate globals and not as one
//: object, because IDA spells them separately and the canonicaliser
//: compares operand *symbols*: one array object would put every reference
//: to the second field an offset away from its name and cost every one of
//: them its match.
extern unsigned int  g_highlightedWidgetCount;
extern CSelectable** g_pHighlightedWidgets;
//: dword_6FAB62D4 - the widget the cursor is currently over.
extern CSelectable*  g_pFocusWidget;

void RefreshSelectedUnitVisuals()
{
    for (unsigned int i = 0; i < g_highlightedWidgetCount; ++i)
    {
        CSelectable* widget = g_pHighlightedWidgets[i];

        if (widget == 0)
            continue;

        SItemHandleObject* agent = (SItemHandleObject*)
            LookupHandle(((SAgentHandlePair*)widget)->m_handle,
                         ((SAgentHandlePair*)widget)->m_handleType);
        if (agent == 0)
            continue;

        SItemHandleObject* checked =
            (agent->m_kindTag == kAgentTagPathable) ? agent : 0;
        if (checked == 0)
            continue;
        if (checked->m_pendingFlag != 0)
            continue;

        widget->RefreshSelectionVisual();
    }

    CSelectable* focus = g_pFocusWidget;

    if (focus == 0)
        return;

    SItemHandleObject* agent = (SItemHandleObject*)
        LookupHandle(((SAgentHandlePair*)focus)->m_handle,
                     ((SAgentHandlePair*)focus)->m_handleType);
    if (agent == 0)
        return;

    SItemHandleObject* checked =
        (agent->m_kindTag == kAgentTagPathable) ? agent : 0;
    if (checked == 0)
        return;
    if (checked->m_pendingFlag != 0)
        return;

    focus->RefreshSelectionVisual();
}
