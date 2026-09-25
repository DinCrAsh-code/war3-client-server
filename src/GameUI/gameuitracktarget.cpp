//============================================================================
//  0x6F2FB7E0 - CGameUI::SetTrackedTarget.
//
//  Everything the UI has to be told when what it is following changes.  The
//  order is the shipped one and it matters: the outgoing target is dealt
//  with completely - portrait, notification, history - before the incoming
//  one is looked at at all.
//
//  Releasing the keyboard first is unconditional except while the chat bar
//  is open: typing into chat survives a change of tracked target, anything
//  else loses focus.
//
//  Who gets the portrait is the one piece of real logic on the way out.  The
//  portrait goes back to the camera unless the outgoing target was trackable
//  *and* the incoming one is not - i.e. the only case that blanks the
//  portrait is going from something followable to nothing followable.
//
//  On the way in, an untrackable target (or none) is the same as no target:
//  the portrait goes back to the camera and nothing else happens.  A
//  trackable one may take the camera with it, closes the chat bar unless it
//  is one of the two exempt targets at +0x230/+0x234, and cancels the
//  portrait's own control track.
//
//  The last word belongs to the target: slot 0x24 answering zero undoes the
//  whole switch, and the tail call to PopTrackedTarget puts the previous one
//  back rather than leaving the UI following nothing.
//
//  `this` is cast to CLayer for the keyboard release because that is what
//  the shipped code does - it calls CLayer::SetKeyboardFocus with the
//  CGameUI itself in ecx (0x6F2FB7FA), which says CGameUI derives from
//  CLayer and not straight from CObserver as gameui.h currently has it.
//  The cast costs the one `mov ecx, esi` the shipped stream does not need,
//  because it never moved `this` out of ecx in the first place.
//============================================================================
#include "gameui.h"
#include "clayer.h"

void CGameUI::SetTrackedTarget(STrackTarget* target, int recordPrevious)
{
    if (m_pSelectionHost->m_entryActive == 0)
        ((CLayer*)this)->SetKeyboardFocus(0, 0, 0);

    STrackTarget* previous = m_pTrackedTarget;
    if (previous == target)
        return;

    if (previous != 0)
    {
        //  Two calls and not one call on a computed subject: the shipped
        //  stream pushes the literal zero in its own arm and only the other
        //  arm pushes the camera, which is MSVC cross-jumping two calls back
        //  together.  Folding it into one call costs the literal its `push`.
        if (previous->IsTrackable() != 0
            && (target == 0 || target->IsTrackable() == 0))
            m_pPortrait->SetPortraitFrame(0);
        else
            m_pPortrait->SetPortraitFrame(m_pCamera);

        //  Re-read through the field rather than through `previous`: the
        //  shipped code reloads +0x1B4 here and at the history push below.
        m_pTrackedTarget->OnTrackLost();

        if (recordPrevious != 0
            && m_pTrackedTarget->IsWorthRemembering() != 0)
            m_trackHistory.Append(1, (void* const*)&m_pTrackedTarget);
    }

    if (target != 0 && target->IsTrackable() != 0)
    {
        if (target->TakesOverCamera() != 0)
        {
            m_pCamera->ClearCursorMode();
            m_pCamera->StopAll(1);
        }

        if (m_pSelectionHost->m_entryActive != 0
            && target != m_pChatExemptTargetA
            && target != m_pChatExemptTargetB)
            m_pSelectionHost->SetEntryActive(0);

        m_pPortrait->CancelControlTrack();
    }
    else
    {
        m_pPortrait->SetPortraitFrame(m_pCamera);
    }

    //  The shipped block order is the other way round from this build's: the
    //  store is the fall-through there and the tail call to PopTrackedTarget
    //  is the branch, where MSVC here sinks the store past the call whatever
    //  the source says.  All three spellings were measured - `if/else`, this
    //  early return, and a `goto` to a label after the store - and all three
    //  emit the same seven-instruction tail in the same order, so the
    //  clearest of them is what is here.
    if (target == 0 || target->AcceptsTracking() != 0)
    {
        m_pTrackedTarget = target;
        return;
    }

    PopTrackedTarget();
}
