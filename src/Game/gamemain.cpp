//============================================================================
//  0x6F009850 - GameMain, Game.dll's entry export.  Sets up the world
//  security descriptor, stamps the startup tick, registers the 'game'
//  module tag, resolves the module directory, builds
//  "<module directory>Logs" and ensures it exists, runs the whole init
//  sequence, and - on success - the whole reset/teardown sequence right
//  back (this call tree never observes a live game loop; everything
//  reachable from a depth-5 walk rooted here is startup and shutdown).
//============================================================================
#include <windows.h>

//  0x6F00BAB0/0x6F6C4CA0/0x6F6C58C0/0x6F6C5910 - already reconstructed
//  above.
int   __fastcall SetupWorldSecurityDescriptor();
void  __fastcall SetGameMainStartTick(DWORD tick);
int   __fastcall RegisterModuleTag(DWORD tag, void* value);
void  __fastcall UnregisterModuleTag(DWORD tag);

//  0x6F6C6000 - already reconstructed (Os/os.h).
void __fastcall OsGetModuleDirectory(char* buffer, DWORD size);

//  Storm ordinal 503/585 - 503 already declared this shape elsewhere
//  (Os/os.h); 585 not otherwise declared in this tree - one stack
//  argument, a path, per this call site's own `lea`/`push` pair (no
//  register load).
void         __stdcall Storm_503(char* dest, const char* src, unsigned int maxLen);
void         __stdcall Storm_585(char* path);

//  0x6F009720/0x6F62ABD0/0x6F007B00 - already reconstructed above.
int  __fastcall GameMainInitSequence();
void NoOpAlias_6F62ABD0();
void __fastcall GameMainResetState();

//  0x6F7E1059 - already reconstructed (Misc/crtsec.h - SecurityCheckCookie);
//  called directly by name elsewhere, declared locally here to keep this
//  file self-contained the same way every other TU in this batch does.
void __fastcall SecurityCheckCookie(unsigned int cookie);

int __stdcall GameMain(void* hInstance)
{
    SetupWorldSecurityDescriptor();

    SetGameMainStartTick(GetTickCount());

    RegisterModuleTag(0x67616D65, hInstance);   // 'game'

    char logsPath[0x104];
    OsGetModuleDirectory(logsPath, 0x104);
    Storm_503(logsPath, "Logs", 0x104);
    Storm_585(logsPath);

    if (GameMainInitSequence())
    {
        NoOpAlias_6F62ABD0();
        GameMainResetState();
        UnregisterModuleTag(0x67616D65);
    }

    return 0;
}
