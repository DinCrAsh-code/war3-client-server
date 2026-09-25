//============================================================================
//  0x6F4EA040 - the last step of setting a handle object up: hand it to its
//  own vtable slot 4, with no arguments and as a tail jump.
//
//  Three instructions, and nothing in this call tree says what slot 4 does -
//  so the name is what its one caller uses it for.  CWorldFrameWar3's
//  terrain-fog setup (0x6F39A910) creates a light object, writes two of its
//  slots, resets a third and then calls this on it, which is the object
//  going live; nothing reads a result.
//
//  Own translation unit: the dispatch is a real indirect call.
//============================================================================
#include "controllerslot.h"

void __fastcall ActivateHandleObject(void* object)
{
    typedef void (__thiscall *ActivateFn)(void*);
    ((ActivateFn)(*(void***)object)[4])(object);
}
