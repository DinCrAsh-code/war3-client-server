//============================================================================
//  0x6F006750 - reached from GameMainInitSequence
//  (Game/gamemaininitsequence.cpp): if preference 0x48 is still unset, show
//  a blocking window over the "TutorialIn" movie, read five more
//  preferences into a small record, hand them through
//  GameMain_sub_6F526F10 (camera/display-mode plumbing below this
//  session's own dump depth), cache the resulting window handle, tear the
//  blocking window down, and mark preference 0x48 done.
//============================================================================
#include <windows.h>
#include "preferences.h"

//  0x6F6BB1D0/0x6F6BB2E0/0x6F5BC8A0 - already reconstructed
//  (Window/blockingwindow.cpp, Os/buildmoviespath.cpp).
void CreateBlockingWindow();
void DestroyBlockingWindow();
void __fastcall BuildMoviesPathB3(const char* name, char* destBuffer, unsigned int destSize);

//  0x6F526F10 - already reconstructed (Misc/gamemainenginethunks.cpp).
void __fastcall GameMain_sub_6F526F10(void*, unsigned int, unsigned int, unsigned int, unsigned int);

//  0x6F525F40 - already reconstructed as void() (Render/renderdeviceslot24thunk.cpp);
//  redeclared with its real eax return captured for this call site alone,
//  same as Game/gamemaininitgraphics.cpp's own use of it.
#include "renderdevice.h"
__declspec(naked) HWND RenderDeviceSlot0x24ForwardHwnd2()
{
    __asm {
        mov ecx, g_renderDevice
        mov eax, [ecx]
        mov edx, [eax + 24h]
        jmp edx
    }
}

//  0x6F6BADA0 - already reconstructed (Misc/gamemain_trivial_leaves.cpp).
void __fastcall CacheGameWindowHandle(HWND value);

void __fastcall GameMainInitTutorial()
{
    CPreferencesWar3* prefs = GetPreferences();

    int done = 0;
    prefs->GetIntValue(0x48, &done);
    if (done == 0)
    {
        CreateBlockingWindow();

        char moviePath[0x104];
        BuildMoviesPathB3("TutorialIn", moviePath, 0x104);

        int v10, v11, v12, v13, v14;
        prefs->GetIntValue(0x10, &v10);
        prefs->GetIntValue(0x11, &v11);
        prefs->GetIntValue(0x12, &v12);
        prefs->GetIntValue(0x13, &v13);
        prefs->GetIntValue(0x14, &v14);

        GameMain_sub_6F526F10(moviePath, 0, (unsigned int)&v10, 0, 0);

        HWND hWnd = RenderDeviceSlot0x24ForwardHwnd2();
        CacheGameWindowHandle(hWnd);

        DestroyBlockingWindow();

        prefs->StoreValue(0x48, 1);
    }
}
