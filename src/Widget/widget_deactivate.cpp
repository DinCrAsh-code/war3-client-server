//============================================================================
//  0x6F2ABEA0 - CWidget's vtable slot 13 (+0x34): tear the widget down
//  without destroying it.
//
//  Own translation unit: both halves of its tail (ReleaseHandle38 and
//  CWar3Image::Deactivate, widget_releaseslots.cpp) are real out-of-line calls in
//  the shipped code, and the last of them is a tail jump.
//============================================================================
#include "widget.h"

//  slot 79 (+0x13C) - "is the value slot 75 hands back above zero"
//  (widget_vtable_leaves.cpp).  Reached through the vtable, not by name:
//  CItem overrides slot 75 underneath it.
typedef int (__thiscall *Slot0x13CFn)(void* self);
//  slot 83 (+0x14C) - CWidget::RemoveFootprint (widgetfootprintremove.cpp).
typedef void (__thiscall *Slot0x14CFn)(void* self, int blocking, int useAlternate);

void CWidget::Deactivate()
{
    //  `neg`/`sbb`/`add 1` is the branchless `x == 0` MSVC emits for a
    //  logical negation feeding an int argument, not a `!= 0`.
    int useAlternate = (((Slot0x13CFn)(*(void***)this)[0x13C / 4])(this) == 0);
    ((Slot0x14CFn)(*(void***)this)[0x14C / 4])(this, 0, useAlternate);

    ReleaseHandle38();
    CWar3Image::Deactivate();
}
