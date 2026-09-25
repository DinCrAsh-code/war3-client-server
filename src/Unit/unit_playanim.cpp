//============================================================================
//  0x6F27B3E0 / 0x6F27B420 - CUnit's vtable slots 35 and 36 (+0x8C/+0x90).
//  See unit.h.
//
//  Same two-functions-not-one shape as CWar3Image's pair
//  (widget_spriteanim.cpp), and the same reason both survive rather than
//  one calling the other: no /OPT:ICF (agentdefaults.cpp).
//
//  The difference from the base version is where the animation name and
//  the mode come from.  CWar3Image passes the caller's `name` by address
//  off its own argument slot and a literal 1 as the mode; CUnit *stores*
//  the name into m_animName2E0 first and passes the address of that field,
//  and takes the mode out of m_animMode2D8.  The store is not a
//  compiler-invented temporary - the field is a real one the rest of the
//  unit reads back, which is why `lea eax, [esi+2E0h]` is computed once
//  and used both to store through and to pass.
//============================================================================
#include "unit.h"

//  slot 38 (+0x98) - CWar3Image::NotifyWidgetVisibilityChanged.
typedef void (__thiscall *Slot0x98Fn)(void* self);

float CUnit::PlayAnimation(unsigned int name, int flags, int notify)
{
    if (notify != 0)
        ((Slot0x98Fn)(*(void***)this)[0x98 / 4])(this);
    m_animName2E0 = name;
    return PlaySpriteAnimation(&m_animName2E0, m_animMode2D8, flags);
}

float CUnit::PlayAnimationLooping(unsigned int name, int flags, int notify)
{
    if (notify != 0)
        ((Slot0x98Fn)(*(void***)this)[0x98 / 4])(this);
    m_animName2E0 = name;
    //  The `| 1` is on the *third* argument, not the mode: the shipped
    //  slot 36 body ORs the caller's own `flags` before pushing it and
    //  leaves the mode field alone.  Its base-class twin
    //  (CWar3Image::PlayAnimationLooping, widget_spriteanim.cpp) reads the
    //  same way once the mode is a literal 1 there.
    return PlaySpriteAnimation(&m_animName2E0, m_animMode2D8, flags | 1);
}
