//============================================================================
//  0x6F3BBBB0 - JASS's `EndGame (B)V`.
//
//  Nine instructions: latch the caller's "show the score screen" flag and a
//  zero beside it on the world object, then ask the session to end the game.
//  The zero at +0x320 is the *reason* slot - JASS has exactly one way to end
//  a game and it is this one, so nothing in this call tree ever writes
//  anything else there and the field keeps its offset for a name.
//
//  A guarded success path, not a refusal: the shipped `jz` jumps forward
//  onto the bare `retn`, so the body is the fall-through.
//
//  See docs/targets/JASS_EndGame.md.
//============================================================================
#include "jassnatives.h"

//  0x6F39F240 - Net/netsessionendrequest.cpp.  `__stdcall`: the one
//  argument is pushed and the callee cleans it (this native ends in a bare
//  `retn`, with no `add esp, 4` behind the call).
void __stdcall RequestSessionGameOver(int index);

//----------------------------------------------------------------------------
//  A fifth view of dword_6FAB65F4, for the two words the end-game request
//  latches.  One `extern` and one cast per view, exactly as
//  Game/gameconfig.h and Widget/playercolor.h already do.
//----------------------------------------------------------------------------
struct SWorldEndGameRequest
{
    char m_reserved000[0x320];
    int  m_endReason;        // +0x320
    int  m_showScoreScreen;  // +0x324
};

void __cdecl JASS_EndGame(int doScoreScreen)
{
    SWorldEndGameRequest* world = (SWorldEndGameRequest*)g_unk6FAB65F4;
    if (world != 0)
    {
        world->m_showScoreScreen = doScoreScreen;
        world->m_endReason = 0;
        RequestSessionGameOver(0);
    }
}
