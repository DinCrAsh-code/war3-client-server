//============================================================================
//  0x6F341460 - SSelectionHost::SetEntryActive: open the chat bar for typing,
//  or close it.
//
//  Closing it releases the keyboard, blanks the line, and hides the label,
//  the line and the bar itself - in that order, innermost first.  Opening it
//  runs the same three shows in the mirror order and takes the keyboard back,
//  but first has to decide which prefix to show, and that decision is three
//  questions deep:
//
//    * is the "allies" chat key held?  then the target is 1;
//    * is the "all" chat key held?  then it is 0;
//    * otherwise whatever the console itself is set to (GetLocalChatTarget).
//
//  Both keys are asked for by the raw key values 1 and 0, which is what
//  IsEventKeyRegistered (Frame/eventkeyquery.cpp) takes in ecx.
//
//  Five hand-written slot dispatches, none of them guesses about argument
//  count: each one pushes exactly what the shipped call site at the address
//  in the comment pushes.  Three of the five are on `this`, so
//  tools/vtable_dispatch_audit.py judges them; the two on the label and the
//  line are not - the class in those slots is not established anywhere in
//  this tree, so there is no committed vtable to check them against and the
//  shipped call site is the whole of the evidence.
//============================================================================
#include "gameui.h"
#include "frame.h"

//  0x6F62B170 - Frame/eventkeyquery.cpp.
int __fastcall IsEventKeyRegistered(void* key);

//  Slot 0x10C - enable or grey out.  One dword pushed (0x6F34147C).
typedef void (__thiscall *FrameSetEnabledFn)(void* self, int enabled);
//  Slots 0xD0 and 0xD4 - hide and show.  Nothing pushed at any of the six
//  call sites (0x6F3414AB, 0x6F3414BB, 0x6F3414C7, 0x6F341508, 0x6F341518,
//  0x6F341539).
typedef void (__thiscall *FrameVisibilityFn)(void* self);

void SSelectionHost::SetEntryActive(int active)
{
    m_entryActive = active;
    ((FrameSetEnabledFn)(*(void***)m_pEntryBox)[0x10C / 4])(m_pEntryBox,
                                                           active);

    if (active == 0)
    {
        m_pEntryBox->SetKeyboardFocus(0, 0, 0);
        SetEntryText("");
        ((FrameVisibilityFn)(*(void***)m_pPrefixLabel)[0xD0 / 4])(
            m_pPrefixLabel);
        ((FrameVisibilityFn)(*(void***)m_pEntryBox)[0xD0 / 4])(m_pEntryBox);
        ((FrameVisibilityFn)(*(void***)this)[0xD0 / 4])(this);
        return;
    }

    //  Three calls and not one call on a computed target: the shipped stream
    //  pushes the literal 1 and the literal 0 in their own arms and only the
    //  fall-through arm pushes a register, which is MSVC cross-jumping three
    //  calls back together.  Folding it into one call here costs the two
    //  literals their `push`.
    if (IsEventKeyRegistered((void*)1) != 0)
        SetColonMessage(1);
    else if (IsEventKeyRegistered((void*)0) != 0)
        SetColonMessage(0);
    else
        SetColonMessage(GetLocalChatTarget());

    ((FrameVisibilityFn)(*(void***)this)[0xD4 / 4])(this);
    ((FrameVisibilityFn)(*(void***)m_pPrefixLabel)[0xD4 / 4])(m_pPrefixLabel);
    m_pEntryBox->SetKeyboardFocus(1, 0, 0);
    ((FrameVisibilityFn)(*(void***)m_pEntryBox)[0xD4 / 4])(m_pEntryBox);
}
