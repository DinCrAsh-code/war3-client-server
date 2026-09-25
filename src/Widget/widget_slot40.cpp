//============================================================================
//  0x6F2AB980 - CWidget's vtable slot 16 (+0x40).  See widget.h.
//
//  Own translation unit: the colour-slot refresh it opens with
//  (widget_selectioncircle.cpp) is a real call in the shipped code, and the
//  slot-65 dispatch it ends with is a tail jump.
//============================================================================
#include "widget.h"

//  slot 65 (+0x104) - not identified; no arguments.  The same slot
//  CWidget::Method_0xB4 (widget_vtable_leaves.cpp) tail-jumps into.
typedef int (__thiscall *Slot0x104Fn)(void* self);

void CWidget::RefreshOwningPlayerColor()
{
    CWar3Image::RefreshOwningPlayerColor();
    ((Slot0x104Fn)(*(void***)this)[0x104 / 4])(this);
}
