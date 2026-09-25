//============================================================================
//  0x6F26F430 - CUnit's vtable slot 106 (+0x1A8), ShowAttachments.  See
//  unit.h.
//
//  Run CSelectable's own body first (0x6F2C7720, a direct non-virtual
//  call), then push a "show" flag into each of the unit's four attachment
//  handles.  Three are plain indices tested for negative - the -1 = unset
//  convention itemhandleresolve.h describes - and the fourth is compared
//  against g_unk6F932954 instead, which is the same sentinel spelled as a
//  global.  The last one is a tail jump.
//============================================================================
#include "unit.h"

//  0x6F00D9E0 - already reconstructed (well, redirected) as
//  AttachmentSetVisible in configgates.cpp; re-declared under its real
//  name rather than a fresh one, because the canonicaliser compares symbol
//  names and a locally invented name silently costs every call site here
//  its match.
void __fastcall AttachmentSetVisible(unsigned int slot, int on);

extern const unsigned int g_unk6F932954;

void CUnit::ShowAttachments()
{
    CSelectable::ShowAttachments();

    if ((int)m_notifyHandle25C >= 0)
        AttachmentSetVisible(m_notifyHandle25C, 1);
    if ((int)m_handle260 >= 0)
        AttachmentSetVisible(m_handle260, 1);
    if ((int)m_handle274 >= 0)
        AttachmentSetVisible(m_handle274, 1);
    if (m_handle278 != g_unk6F932954)
        AttachmentSetVisible(m_handle278, 1);
}
