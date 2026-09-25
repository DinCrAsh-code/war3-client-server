//============================================================================
//  0x6F007B00 - GameMain's own teardown-and-reset path (reached from
//  Game/gamemaininitsequence.cpp on the "already running" and locale-
//  rejected exits, and again after a clean run).  Destroys the
//  preferences singleton, releases Storm ordinal 496's scratch pair, runs
//  five otherwise-unrelated static-destructor thunks, releases a cached
//  string tagged "AuCachedDisplay" if one is live, tears the render
//  device down, pauses briefly, and posts Storm ordinal 470's own
//  continuation.
//============================================================================

//  0x6F005720/0x6F005760 - already reconstructed (Config/preferences.cpp,
//  Config/preferencesdestroy.cpp).  `class`, not `struct` - preferences.h's
//  own declaration - a mismatched forward-declare keyword mangles
//  GetPreferences's return type differently and silently loses the
//  symbol's own PUBLIC/EXTRN comment (docs/msvc-vc8-idioms.md, "struct vs
//  class into a mangled name").
class CPreferencesWar3;
CPreferencesWar3* GetPreferences();
void __fastcall DestroyPreferences();

//  Misc/gamemainthunks.cpp - one-instruction tail-jump alias.
void NoOpAlias_6F7AD910();
void NoOpAlias_6F467500();

//  Misc/spritetypetagteardown.cpp / configprofiletypetagteardown.cpp /
//  databasetypetagteardown.cpp - already reconstructed above.
void TeardownSpriteTypeTags();
void TeardownConfigProfileTypeTags();
void TeardownDatabaseTypeTag();

//  Already reconstructed elsewhere in the tree.
void TextBlockAllocatorStaticDtor();     // 0x6F4CCEC0, Misc/gamemain_small_wrappers.cpp
void ObserverPoolStaticDtor();            // 0x6F62ABB0, Agent/observerpoolstaticdtor.cpp
void StatusEntryStaticDtor();              // 0x6F4C4000, Misc/gamemain_small_wrappers.cpp
void TeardownMpqModuleSlots();              // 0x6F0013C0, Game/mpqmoduleslotsteardown.cpp
void __fastcall ShutdownRenderDevice(int shouldReset);   // 0x6F526DF0, Render/renderdeviceshutdown.cpp
void __fastcall PauseAndWaitBriefly();                    // 0x6F6C5440, Sync/threadpausewait.cpp

//  Storm ordinal 496/403/470 - 403/470 already declared this shape
//  elsewhere (SMemFree / Render/renderdeviceshutdown.cpp); 496 not
//  otherwise declared in this tree.
void __stdcall Storm_496(void* a, unsigned int size);
void __stdcall SMemFree(void* ptr, const char* logfilename, int logline, unsigned int flags);
void __stdcall Storm_470(void* callback);

//  0x6F0070F0 - out of this session's scope; a shared continuation address
//  Storm_469/Storm_470 are both handed by address only.  Thunked.
__declspec(naked) void SharedResetContinuation_6F0070F0()
{
    __asm { mov eax, 0x6F0070F0 }
    __asm { jmp eax }
}

extern void* g_cachedDisplayString;   // dword_6FAAE368
extern void* g_renderDeviceResult;    // dword_6FAAE34C (Game/gamemaininitgraphics.cpp)
extern void* g_resetField360;         // dword_6FAAE360
extern void* g_resetField364;         // dword_6FAAE364

void __fastcall GameMainResetState()
{
    GetPreferences();
    DestroyPreferences();
    NoOpAlias_6F7AD910();

    Storm_496(0, 8);
    NoOpAlias_6F467500();

    TeardownSpriteTypeTags();
    TeardownConfigProfileTypeTags();
    TeardownDatabaseTypeTag();
    TextBlockAllocatorStaticDtor();
    ObserverPoolStaticDtor();
    StatusEntryStaticDtor();
    TeardownMpqModuleSlots();

    if (g_cachedDisplayString)
        SMemFree(g_cachedDisplayString, ".?AUAuCachedDisplay@@", -2, 0);

    void* device = g_renderDeviceResult;
    g_resetField360 = 0;
    g_resetField364 = 0;
    g_cachedDisplayString = 0;
    ShutdownRenderDevice((int)(long)device);

    g_renderDeviceResult = 0;
    PauseAndWaitBriefly();

    Storm_470(&SharedResetContinuation_6F0070F0);
}
