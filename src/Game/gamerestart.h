//============================================================================
//  A sixth view of dword_6FAB65F4, for the fields the three game-state
//  natives - ReloadGame, RestartGame and ReloadGameCachesFromDisk - latch on
//  the world object before anything else happens.  One `extern` and one cast
//  per view, exactly as Game/gameconfig.h, Widget/playercolor.h and
//  Jass/jassnatives_endgame.cpp already do.
//
//  +0x320 and +0x324 are the same two words EndGame writes; the rest are
//  this family's own.
//============================================================================
#ifndef GAMERESTART_H
#define GAMERESTART_H

#include "jassnatives.h"        // g_unk6FAB65F4, STStringField

struct SWorldRestart
{
    void RequestRestart(int doScoreScreen, int reResolveSaveGame,
                        int endNow, int endReason);     // 0x6F3A6AA0

    //  0x6F3A4C70 - Game/gamerestartload.cpp.  Start the load itself.
    void RequestLoad(const char* path, int doScoreScreen, int keepSlots);

    char m_reserved000[0x1C];
    void*  m_eventHost;         // +0x1C  - what the reload notifies
    char m_reserved020[0x2D0];
    //  +0x2F0 - the map (or save-game) path the next load will use.  The
    //  same counted string every other view calls STStringField.
    char   m_mapPath[0x30];     // +0x2F0 .. +0x31F
    int    m_endReason;         // +0x320 - EndGame's "reason" slot, reused
                                //          here as "a load is pending"
    int    m_showScoreScreen;   // +0x324
    int    m_reloadFailed;      // +0x328
    int    m_pathIsSaveGame;    // +0x32C
    int    m_loadKeepsSlots;    // +0x330
    char m_reserved334[0x08];
    char   m_slotRecord[0x8C];  // +0x33C - the session's own slot record,
                                //          which the save game's slot table
                                //          is written into (a
                                //          Net::GAMESETUP_SLOTRECORD)
};

//  +0x3C8 sits past the block above; reached through its own accessor so the
//  reserved run does not have to be sized.
inline int& WorldSuppressReload(void* world)
{ return *(int*)((char*)world + 0x3C8); }

//  0x6F53E670 - Game/gamedatatag.cpp.  "Does the current game data carry a
//  real value in the tag at +0x2288" - i.e. is this a POOL or ENON session,
//  in which case none of these natives may do anything.
int IsGameDataTagSet_6F53E670();

//  0x6F3A6AA0 - Game/gamerestartrequest.cpp.  The one body ReloadGame and
//  RestartGame share.  Declared as a member of the view above, not as a
//  `__fastcall` free function taking the world: the shipped body is
//  `retn 10h` against a receiver in ecx and four pushes, and a free
//  function would take the first argument in edx and clean four bytes too
//  few - abi_audit.py's FATAL bucket, and a live crash on the first call.

//  0x6F3B28C0 - Game/gamecachereloadgate.cpp.
int ReloadGateOpen();

//  0x6F426AE0 - Game/gamerestartnotify.cpp.  Announce the request on the
//  world's own event host.
void __fastcall PostReloadRequestedEvent(void* host);

//  0x6F53F830 - Game/gamedatadiscardtemp.cpp.  Discard whatever temporary
//  game-data file the last session left behind.  Takes nothing, cleans
//  nothing.
void __fastcall GameDataDiscardTempFile();

//  0x6F3A83E0 - Game/gameobjectdatacache.cpp.  `retn 0`, this in ecx.
void* __fastcall WorldEnsureObjectDataCache(void* world);

//  0x6F4061F0 - the game-cache store's own reload.  A member, not a
//  `__fastcall` free function: the shipped body is `retn 4` against a
//  receiver in ecx and one push.  Not in this tree; redirected.
struct SGameCacheStore
{
    int ReloadFromDisk(int fromDisk);
};

#endif
