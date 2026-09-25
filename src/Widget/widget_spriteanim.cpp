//============================================================================
//  0x6F26BA90 / 0x6F26BAC0 - CWidget's vtable slots 35 and 36 (+0x8C/+0x90).
//  See widget.h.
//
//  The two are the same function apart from one `or eax, 1` - slot 36
//  forces bit 0 of the flags word on - and the shipped build carries both
//  rather than making one call the other, which is what this image being
//  linked without /OPT:ICF leaves standing (agentdefaults.cpp).
//
//  `name` is passed to PlaySpriteAnimation *by address* (`lea ecx,
//  [esp+arg_0]` on the caller's own argument slot), so the callee takes a
//  pointer to it - a four-byte id handed on by reference, the same shape
//  AGILE_TYPE_ID travels in (agiletype.h).
//============================================================================
#include "widget.h"

//  slot 38 (+0x98) - NotifyWidgetVisibilityChanged (widgetvisibility.cpp),
//  reached through the vtable and with no arguments.
typedef void (__thiscall *Slot0x98Fn)(void* self);

float CWar3Image::PlayAnimation(unsigned int name, int flags, int notify)
{
    if (notify != 0)
        ((Slot0x98Fn)(*(void***)this)[0x98 / 4])(this);
    return PlaySpriteAnimation(&name, 1, flags);
}

float CWar3Image::PlayAnimationLooping(unsigned int name, int flags, int notify)
{
    if (notify != 0)
        ((Slot0x98Fn)(*(void***)this)[0x98 / 4])(this);
    return PlaySpriteAnimation(&name, 1, flags | 1);
}
