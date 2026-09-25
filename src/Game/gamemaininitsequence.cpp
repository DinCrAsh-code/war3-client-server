//============================================================================
//  0x6F009720 - GameMain's own second call (Game/gamemain.cpp).  Checks
//  whether another instance is already running ("Warcraft III Game
//  Application"), and if so signals it and shows a fatal message;
//  otherwise creates the manual-reset instance event, sets up Storm's own
//  version/instance strings, brings up CPU-ticks-per-second and the
//  file-cache prefetch limit, applies command-line tuning, initializes
//  MPQ modules and preferences, checks the replay/hardware and graphics
//  device, checks the locale, then either falls straight to
//  GameMainResetState (on any early failure) or finishes with the file
//  cache, MPQ uncachable markers, and the notify-timer registration
//  before returning success.
//============================================================================
#include <windows.h>

//  0x6F6CE0A0/0x6F6CE090/0x6F00B240 - already reconstructed above
//  (Os/osnamedeventprobe.cpp, Game/gamemainsignalrunning.cpp).
BOOL __fastcall OsProbeNamedEventExists(const char* name);
HANDLE __fastcall OsCreateManualEvent(const char* name);
void __fastcall SignalRunningInstance();

//  0x6F0068C0 - already reconstructed (Game/gamemainfatalmessage.cpp).
void __fastcall GameMainShowFatalMessage(unsigned int lineCount);

//  Storm ordinal 567/565/460/469/568 - not otherwise declared in this
//  tree.  567 takes no visible argument at this call site; 460/469/568
//  each take exactly the one stack argument this call site pushes.
void __stdcall Storm_567();
void __stdcall Storm_565(const char* buildString);
void __stdcall Storm_460(unsigned int version);
void __stdcall Storm_469(void* callback);
void __stdcall Storm_568(void* callback);

//  0x6F0070F0 - the same shared continuation Game/gamemainresetstate.cpp
//  already thunks; reused rather than re-thunked under a second name (one
//  shipped address, one C++ symbol).
void SharedResetContinuation_6F0070F0();

//  0x6F007760 - out of this session's scope; address taken only, never
//  called from this tree.  Thunked.
__declspec(naked) void UnknownCallback_6F007760()
{
    __asm { mov eax, 0x6F007760 }
    __asm { jmp eax }
}

//  0x6F6C5730/0x6F6CDEE0/0x6F4C19F0/0x6F006840 - already reconstructed
//  above.
void __fastcall GameMainInitCpuTicksPerSecond();
void __fastcall SetInactivitySleepMs(int milliseconds);
void __fastcall GameMainRegisterHandlerTable();
void __fastcall GameMainApplyCmdLineTuning();

//  0x6F005720 - already reconstructed (Config/preferences.cpp).  0x6F0058A0
//  is a member of it (Game/gamemaininitresolutionlist.cpp).
#include "preferences.h"

//  0x6F0010E0/0x6F007020/0x6F009580/0x6F007670 - already reconstructed
//  above.
int __fastcall GameMainInitMpqModules();
int __fastcall GameMainReplayHardwareCheck();
int __fastcall GameMainInitGraphicsDevice();
int __fastcall GameMainCheckLocaleAllowed();

//  Misc/gamemainthunks.cpp - one-instruction tail-jump alias.
void NoOpAlias_6F7AD920();

//  0x6F007B00 - already reconstructed above (Game/gamemainresetstate.cpp).
void __fastcall GameMainResetState();

//  0x6F4C3F20/nullsub_31/0x6F4BF470/0x6F4BC890 - already reconstructed
//  above.
void InitializeFileCachePrefetchLimit();
void __fastcall GameMainNoOp_6F6EED70();
void __fastcall SetFileCacheEnabled(unsigned int enabled);
void __fastcall CleanupPreFiles();

//  0x6F4C1490 - already reconstructed (Render/filecache.h).
void __fastcall MarkFileUncachable(const char* name);

//  0x6F62AB80/0x6F62ABF0/0x6F62AC30/0x6F4674F0/0x6F00B240/0x6F006750 -
//  already reconstructed above.
void __fastcall ClampAndForwardTimerCount(unsigned int count, void* context);
void* __fastcall RegisterNotifyTimer(int zero, void* tickCallback, unsigned int intervalMs,
                                     void* secondCallback, int flags);
void NoOpAlias_6F62AC30();
void NoOpAlias_6F4674F0();
void __fastcall GameMainInitTutorial();

int __fastcall GameMainInitSequence()
{
    if (OsProbeNamedEventExists("Warcraft III Game Application"))
    {
        SignalRunningInstance();
        GameMainShowFatalMessage(0);
        return 0;
    }

    OsCreateManualEvent("Warcraft III Game Application");
    Storm_567();
    Storm_565("Warcraft III (build 6401)");
    Storm_460(0x1901);
    Storm_469(&SharedResetContinuation_6F0070F0);
    Storm_568(&UnknownCallback_6F007760);

    GameMainInitCpuTicksPerSecond();
    SetInactivitySleepMs(0x32);
    GameMainRegisterHandlerTable();
    GameMainApplyCmdLineTuning();

    GetPreferences()->InitResolutionList(0x50);

    if (GameMainInitMpqModules() &&
        GameMainReplayHardwareCheck() &&
        GameMainInitGraphicsDevice())
    {
        NoOpAlias_6F7AD920();
        int localeResult = GameMainCheckLocaleAllowed();
        if (!localeResult)
            GameMainShowFatalMessage(localeResult + 5);

        GameMainResetState();
        return 0;
    }

    InitializeFileCachePrefetchLimit();
    GameMainNoOp_6F6EED70();
    SetFileCacheEnabled(1);
    CleanupPreFiles();

    MarkFileUncachable("war3mapMap.blp");
    MarkFileUncachable("war3mapMap.tga");

    int zeroBase = 0;
    ClampAndForwardTimerCount(zeroBase + 2, 0);

    RegisterNotifyTimer(1, (void*)0x6F008420, 1, (void*)0x6F007A50, 3);

    NoOpAlias_6F62AC30();
    NoOpAlias_6F4674F0();
    SignalRunningInstance();
    GameMainInitTutorial();

    return 1;
}
