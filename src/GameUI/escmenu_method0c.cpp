//============================================================================
//  0x6F34FDC0 - CEscMenu's vtable slot 3 (override of
//  CObserver::Method_0x0C).  See escmenu.h.
//
//  Own translation unit: 0x6F34FDC0 sits in the same 0x6F34Fxxx run as the
//  constructor/destructor/DeleteSelf above it, but calls none of them and
//  is called by none of them (only ever reached through the vtable), so
//  nothing forces it into the same file - kept separate to mirror how the
//  rest of this class's own slots are split one-per-file.
//============================================================================
#include "escmenu.h"

//  0x6F34FDC0 - `retn 4`.  Only message id 0x40060064 is forwarded, and
//  only when this object currently has an active panel (+0x14 -
//  escmenu.h); every other message, and every message with no active
//  panel, answers 0 without touching m_pActivePanel at all.  The forward
//  is a plain virtual call - CObserver::Method_0x0C, the same slot index -
//  which is exactly what the shipped tail jump into `[[ecx]+0x0C]` is.
int CEscMenu::Method_0x0C(const SAgentMessage* msg)
{
    if (msg->m_id == 0x40060064)
        return m_pActivePanel ? m_pActivePanel->Method_0x0C(msg) : 0;

    return 0;
}
