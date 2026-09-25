//============================================================================
//  0x6F341430 - SSelectionHost::SetEntryAltEventListener.
//
//  A two-instruction forwarder: the chat bar's own alternate-listener call
//  goes to the editable line at +0x1E0, not to the bar.  Written as a tail
//  call so it compiles to the shipped `jmp`, which is what the shipped body
//  is - CLayer::SetAltEventListener (0x6F614590, `retn 0Ch`) takes the same
//  three stack arguments this one is handed, so nothing has to be re-pushed.
//
//  Own translation unit: the CGameUI constructor calls it for real.
//============================================================================
#include "gameui.h"

void SSelectionHost::SetEntryAltEventListener(int event, void* listener,
                                              int enable)
{
    m_pEntryBox->SetAltEventListener(event, listener, enable);
}
