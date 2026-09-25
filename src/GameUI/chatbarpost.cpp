//============================================================================
//  0x6F3415A0 / 0x6F3415B0 - the chat bar's two event forwarders.
//
//  Neither posts through the bar itself: both load the editable line at
//  +0x1E0 and go through *its* CObserver base.  DispatchRange takes the
//  virtual route (slot 2, CObserver::Dispatch) and Post takes the direct one
//  (CObserver::PostEvent, 0x6F62A570, `retn 8`), which is exactly the pair
//  CGameUI::PostCameraEvents picks between - the direct arm for a camera
//  going away, the virtual one for a camera arriving.
//
//  Both are tail calls in the shipped code and are written as tail calls
//  here, which is what turns the `call` into the `jmp` the dump shows.
//
//  These two replace the ADDR_THUNK redirects gameui_thunks.cpp carried for
//  the same two addresses; the redirects are deleted in the same change, so
//  that one shipped address still has exactly one C++ name.
//============================================================================
#include "gameui.h"

void SSelectionHost::DispatchRange(unsigned int low, unsigned int high,
                                   void* target)
{
    m_pEntryBox->Dispatch(low, high, target);
}

void SSelectionHost::Post(unsigned int id, void* target)
{
    m_pEntryBox->PostEvent(id, target);
}
