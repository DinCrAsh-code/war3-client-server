//============================================================================
//  0x6F5FAF10 - CLayer::SetKeyboardFocus.
//
//  One global says which layer currently owns the keyboard
//  (dword_6FACE67C) and a second one freezes it (dword_6FACE680); while the
//  lock is set this does nothing at all, which is the first thing the body
//  tests.
//
//  Otherwise the incumbent is always told it lost the keyboard - through slot
//  0xB0, before anything else is decided - and then:
//
//    * `take` zero clears the owner outright;
//    * a `candidate` that is this layer itself does the same, which is how a
//      frame gives the keyboard up by nominating itself;
//    * a null `candidate` means "me", and the layer is asked through slot
//      0xAC whether it will accept.  It becomes the owner only if it says
//      yes; a refusal leaves the global exactly as the slot-0xB0 call left
//      it, which is *not* the same as clearing it.
//
//  Both slot dispatches are on `this`, so tools/vtable_dispatch_audit.py can
//  judge them; the argument counts are the shipped call sites' own (one dword
//  for 0xB0's receiver alone, two pushed for 0xAC).
//============================================================================
#include "clayer.h"

//  Slot 0xB0 (index 44) - "you no longer have the keyboard".
typedef void (__thiscall *LayerFocusLostFn)(void* self);
//  Slot 0xAC (index 43) - "will you take the keyboard for `reason`, on
//  behalf of `candidate`?"  Non-zero is yes.
typedef int  (__thiscall *LayerAcceptFocusFn)(void* self, int reason,
                                              CLayer* candidate);

void CLayer::SetKeyboardFocus(int take, int reason, CLayer* candidate)
{
    if (g_keyboardFocusLocked != 0)
        return;

    CLayer* incumbent = g_pKeyboardFocusLayer;
    if (incumbent != 0)
        ((LayerFocusLostFn)(*(void***)incumbent)[0xB0 / 4])(incumbent);

    if (take != 0 && candidate != this)
    {
        if (candidate == 0)
            candidate = this;

        if (((LayerAcceptFocusFn)(*(void***)this)[0xAC / 4])(this, reason,
                                                             candidate) != 0)
            g_pKeyboardFocusLayer = this;

        return;
    }

    g_pKeyboardFocusLayer = 0;
}
