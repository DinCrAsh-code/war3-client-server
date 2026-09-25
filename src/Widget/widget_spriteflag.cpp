//============================================================================
//  0x6F2AB6D0 - CWidget's vtable slot 37 (+0x94).  See widget.h.
//
//  Two separate calls to CWar3Image::SetSpriteVisible (widget_spritevisible.cpp) with
//  two different first arguments and two different tails, not one call with
//  a ternary: the shipped code has two `retn 8` epilogues, and the second
//  path does something extra afterwards.
//============================================================================
#include "widget.h"

//  slot 61 (+0xF4) - not identified; called with no arguments after the
//  null-owner path.
typedef void (__thiscall *Slot0xF4Fn)(void* self);

void CWidget::SetSpriteVisible(int force, int visible)
{
    if (force != 0)
    {
        CWar3Image::SetSpriteVisible(force, visible);
        return;
    }

    CWar3Image::SetSpriteVisible(0, visible);
    ((Slot0xF4Fn)(*(void***)this)[0xF4 / 4])(this);
}
