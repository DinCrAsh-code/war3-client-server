//============================================================================
//  0x6F3BC4D0 - `PauseGame` "(B)V".
//
//  Sixteen instructions: fetch the CGameUI singleton and, if there is one,
//  ask it to pause with the acting player's slot taken from the world
//  object's own +0x28 word.  The other three arguments are literals - no
//  notification suppression, latch the state, record it.
//
//  `GetGameUI` is asked with (1, 0): create it if it is not up yet, and do
//  not clear the cached pointer on the way out.  The `xor edx, edx` /
//  `lea ecx, [edx+1]` pair is MSVC forming the constant 1 off a register it
//  has just zeroed, which is not source-addressable - the source has a
//  literal 1 (docs/msvc-vc8-idioms.md, "Constants formed off a register the
//  compiler knows is zero").
//
//  The dump's root block is hand-rendered from an `ida_query bytes` answer
//  through objdump, because 0x6F3BC4D0 is `loc_`, not `sub_` - the dump's
//  own `## User notes` records that in full.  Nothing here depends on
//  anything but those sixteen instructions.
//
//  Own translation unit: both callees are real calls into other modules.
//============================================================================
#include "gameui.h"

//  dword_6FAB65F4 - the world object, whose +0x28 word is the acting
//  player's slot.  Spelled with the name the rest of the repo already gave
//  it (Widget/playercolor.h).
extern void* g_unk6FAB65F4;

struct SWorldActingSlot
{
    char           m_reserved00[0x28];
    unsigned short m_actingSlot;    // +0x28
};

void __cdecl JASS_PauseGame(int paused);

void __cdecl JASS_PauseGame(int paused)
{
    CGameUI* ui = (CGameUI*)GetGameUI(1, 0);

    if (ui != 0)
        ui->SetGamePaused(paused,
                          ((SWorldActingSlot*)g_unk6FAB65F4)->m_actingSlot,
                          0, 1, 1);
}
