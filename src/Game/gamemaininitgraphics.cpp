//============================================================================
//  0x6F009580 - reached from GameMainInitSequence
//  (Game/gamemaininitsequence.cpp): read the "Gfx Fullscreen"/"Gfx SwTNL"/
//  "Gfx OpenGL" preferences (each defaulting to what the caller pre-filled,
//  each overridable by a command-line switch through ForwardToStorm472),
//  bring up the render device, and either finish the window setup on
//  success or fall back to the fatal-message path.
//============================================================================
#include <windows.h>

//  Storm ordinal 423 - not otherwise declared in this tree.  `*out` is
//  read-or-default: the caller pre-fills it and this leaves it alone when
//  the key is genuinely absent, the same shape
//  CPreferencesWar3::GetIntValue already documents.
int __stdcall Storm_423(const char* section, const char* key, int flag, int* out);

//  0x6F4C19C0 - already reconstructed (Misc/gamemain_small_wrappers.cpp).
int __fastcall ForwardToStorm472(const char* text);

//  0x6F6C7730/0x6F00BC20 - already reconstructed
//  (Misc/gamemain_trivial_leaves.cpp / Config/preferencescreenconfigthunk.cpp).
void __fastcall SetLatchedValueOnce(int value);
void DetectScreenConfigThunk();

//  0x6F0094F0 - already reconstructed as void (Config/preferencesapplyfive.cpp),
//  but this call site's own `cmp eax,1` needs its return value, which that
//  declaration cannot give in C++.  Self-redirected to the same real
//  address instead of calling the existing void reconstruction, purely so
//  this one call site can read eax - functionally the same body either way.
__declspec(naked) int __fastcall ApplyStartupPreferencesInt(void* /*self*/, int /*edxArg*/, int /*stackArg*/)
{
    __asm { mov eax, 0x6F0094F0 }
    __asm { jmp eax }
}

//  0x6F526D10 - already reconstructed (Render/renderdeviceswitch.cpp).
//  `class`, not `struct` - renderdevice.h's own declaration; a mismatched
//  forward-declare keyword mangles the return type differently and
//  silently loses the symbol's own PUBLIC/EXTRN comment
//  (docs/msvc-vc8-idioms.md, "struct vs class into a mangled name").
#include "renderdevice.h"
CRenderDevice* __fastcall SwitchRenderDevice(void* recv0, int recv1, int kind,
                                             int arg4, int arg8, int argC);

//  0x6F005720 - already reconstructed (Config/preferences.cpp).
#include "preferences.h"

//  0x6F525F40 - already reconstructed as void() (Render/renderdeviceslot24thunk.cpp);
//  redeclared with its real eax return captured for this call site alone -
//  same naked body, same address; thunk_abi_audit.py checks retn bytes,
//  not a return register, so a second declared shape costs nothing.
__declspec(naked) HWND RenderDeviceSlot0x24ForwardHwnd()
{
    __asm {
        mov ecx, g_renderDevice
        mov eax, [ecx]
        mov edx, [eax + 24h]
        jmp edx
    }
}

//  0x6F6BADA0/0x6F7ADAB0 - already reconstructed (Misc/gamemain_trivial_leaves.cpp).
void __fastcall CacheGameWindowHandle(HWND value);
void __fastcall SetAgentField420(int value);

//  0x6F0068C0 - already reconstructed (Game/gamemainfatalmessage.cpp).
void __fastcall GameMainShowFatalMessage(unsigned int lineCount);

//  0x6F6BAD70/0x6F6C2600 - already reconstructed
//  (Misc/gamemain_trivial_leaves.cpp is NOT it; GetSelectedWindow lives in
//  Window/wininputbridge.h, SetWindowTextFromWide in
//  Window/messageboxprompt.cpp).
HWND __fastcall GetSelectedWindow(unsigned int which);
void __fastcall SetWindowTextFromWide(HWND hWnd, const wchar_t* text);

//  0x6F526080 - already reconstructed (Render/renderdeviceslot8c.cpp).
void __stdcall RenderDeviceSlot0x8CForward(float a, float b);

extern int   g_rowTableCurrentRow;    // dword_6FA4F6E8 (Game/gamemainreplaycheck.cpp)
extern void* g_renderDeviceResult;    // dword_6FAAE34C
extern float g_defaultAspectOrFov;    // flt_6F875BEC - purpose not established by this call tree

int __fastcall GameMainInitGraphicsDevice()
{
    int fullscreenPref = 1;
    int sharedPref;
    int swTnlFlag = 1;
    int openGlFlag = swTnlFlag;

    Storm_423("Warcraft III", "Gfx Fullscreen", 0, &fullscreenPref);
    if (ForwardToStorm472((const char*)(swTnlFlag + 6)))
        fullscreenPref = swTnlFlag;
    else if (ForwardToStorm472((const char*)8))
        fullscreenPref = 0;

    sharedPref = 0;
    Storm_423("Warcraft III", "Gfx SwTNL", 0, &sharedPref);
    if (!ForwardToStorm472((const char*)2) && sharedPref == 0)
        swTnlFlag = 0;

    sharedPref = 0;
    Storm_423("Warcraft III", "Gfx OpenGL", 0, &sharedPref);
    if (!ForwardToStorm472((const char*)openGlFlag) && sharedPref == 0)
        openGlFlag = 0;

    if (ForwardToStorm472((const char*)0))
        openGlFlag = 1;

    SetLatchedValueOnce(1);
    DetectScreenConfigThunk();

    //  ApplyStartupPreferences's own guard: SwitchRenderDevice and the
    //  window-handle/preference follow-up only run when it reports 1;
    //  otherwise this falls straight to the g_renderDeviceResult check
    //  below with whatever that global already held - the shipped body's
    //  own `jnz` skips the whole block rather than gating an `else`.
    if (ApplyStartupPreferencesInt(&openGlFlag, fullscreenPref, (int)&sharedPref) == 1)
    {
        g_renderDeviceResult = SwitchRenderDevice(&openGlFlag, (int)0x6F6C6AA0, fullscreenPref,
                                                  swTnlFlag, 0, g_rowTableCurrentRow);
        if (g_renderDeviceResult)
        {
            CPreferencesWar3* prefs = GetPreferences();
            HWND hWnd = RenderDeviceSlot0x24ForwardHwnd();
            CacheGameWindowHandle(hWnd);

            int windowedValue;
            prefs->GetIntValue(0xD, &windowedValue);
            SetAgentField420(windowedValue);

            int unused;
            prefs->GetIntValue(6, &unused);
        }
    }

    if (g_renderDeviceResult)
    {
        HWND hWnd = GetSelectedWindow(0);
        SetWindowTextFromWide(hWnd, L"Warcraft III");
        RenderDeviceSlot0x8CForward(g_defaultAspectOrFov, 0.0f);
        return 1;
    }

    GameMainShowFatalMessage((swTnlFlag != 1) + 1);
    return 0;
}
