//============================================================================
//  0x6F3505D0 - CEscMenuMainPanel::RefreshCaption (renamed from
//  CGameUIPauseFrame::RefreshCaption, 2026-08-28 - see CEscMenuMainPanel's
//  own comment in gameui.h for the vtable evidence behind the rename).
//
//  Rewrite the pause button's caption and re-enable or grey it out.  The
//  caption is one of two game strings ("KEY_RESUME_GAME" / "KEY_PAUSE_GAME")
//  and the enabled state comes from three different questions depending on
//  which of the two it is:
//
//    * while paused, the button is live only for a real, non-referee player
//      whose slot is still occupied - anything else, including having no
//      local slot at all, leaves it live so the game can be un-paused;
//    * while running, the local player's *owner* slot decides.
//
//  Both branches then AND that with "the local player is a full player", and
//  the answer goes through vtable slot 0x10C of the frame at +0x190.
//
//  The 0x80-byte caption buffer is what puts a /GS cookie in the shipped
//  prologue; this repo builds at a fixed /GS-, so the cookie load, the
//  `xor esp` and the check call are the only instructions that cannot come
//  back.
//
//  Own translation unit: six real calls out of it.
//============================================================================
#include "gameui.h"
#include "gameuithunks.h"
#include "netdata.h"
#include "playercolor.h"        // g_unk6FAB65F4

struct IndexedArrayHolder_6F3A1650
{
    char           m_reserved00[0x28];
    unsigned short m_actingSlot;    // +0x28
    int At(unsigned int index);
};

//  0x6F40B1C0 - Misc/trivialaccessors_04.cpp: the slot's referee flag.
int __fastcall GetField0x2A8_6F40B1C0(const void* self);
//  0x6F61E820 - Misc/trivialaccessors_06.cpp: the frame's +0x1E4 text object.
int __fastcall GetField0x1E4_6F61E820(const void* self);

//  slot 0x10C of the caption frame - enable or grey out.
typedef void (__thiscall *SetFrameEnabledFn)(void* self, int enabled);

void CEscMenuMainPanel::RefreshCaption(int paused)
{
    char caption[0x80];
    int  live;

    if (paused != 0)
    {
        GetGameString("KEY_RESUME_GAME", caption, sizeof(caption));

        unsigned char slot = CNetData::GetPausingSlot(0);

        if (slot == 0xFF)
        {
            live = 1;
        }
        else
        {
            IndexedArrayHolder_6F3A1650* world =
                (IndexedArrayHolder_6F3A1650*)g_unk6FAB65F4;
            int record = world->At(slot);

            //  No null check on the record: the shipped code hands At()'s
            //  answer straight to the accessor.
            if (GetField0x2A8_6F40B1C0((const void*)record) != 0
                && IsLocalPlayerReferee() == 0)
                live = 0;
            else
                live = 1;
        }
    }
    else
    {
        GetGameString("KEY_PAUSE_GAME", caption, sizeof(caption));

        live = CNetData::IsSlotParticipating(CNetData::GetLocalSlot(0), 0);
    }

    ((CTextFrame*)GetField0x1E4_6F61E820(m_pCaptionFrame))->SetString(caption);

    ((SetFrameEnabledFn)(*(void***)m_pCaptionFrame)[0x10C / 4])(
        m_pCaptionFrame,
        (live != 0 && IsLocalPlayerFullPlayer() == 0) ? 1 : 0);

    m_paused = paused;
}
