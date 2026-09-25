//============================================================================
//  Two adjacent Win32 dialog-adjacent wrappers off the same wide-to-narrow
//  text buffer (lpchText).  One TU per CLAUDE.md's address-neighbourhood
//  rule: 0x6F6C2600 and 0x6F6C2620 are nine bytes apart and both reach
//  sub_6F6BF900 (below this batch's own 43 - a WideCharToMultiByte-backed
//  wide->narrow converter into the shared `lpchText` global; declared and
//  thunked to its own real address, `retn 4` confirmed by its own dump).
//============================================================================
#include <windows.h>

extern "C" void* __cdecl memset(void* dst, int val, unsigned int size);

//  0x6F6BF900 - convert one wide string into the shared narrow-text buffer.
//  A single plain stack argument, no register argument (both call sites
//  push the text pointer without loading a register first) - `retn 4`.
__declspec(naked) void __stdcall ConvertWideTextToNarrow(const wchar_t*)
{
    __asm { mov eax, 06F6BF900h }
    __asm { jmp eax }
}

extern char* lpchText;   // the buffer ConvertWideTextToNarrow fills

//----------------------------------------------------------------------------
//  0x6F6C2600 - SetWindowTextA(hWnd, narrow(text)).
//----------------------------------------------------------------------------
void __fastcall SetWindowTextFromWide(HWND hWnd, const wchar_t* text)
{
    ConvertWideTextToNarrow(text);
    SetWindowTextA(hWnd, lpchText);
}

//----------------------------------------------------------------------------
//  0x6F6C2620 - a one-shot modal prompt: convert both the (optional)
//  caption and the message out of wide text, run a repeating timer while
//  the box is up (sub_6F6B7FC0/sub_6F6B7FF0 - SetTimer/KillTimer,
//  below this batch's own 43; retn 0 both, no arguments pushed at either
//  call site), show a MessageBoxA whose icon/buttons come from a 4-way
//  switch on `kind`, and fold the result down to 0 ("yes"/"ok"),
//  1 ("no") or 2 (anything else, cancel included).
//
//  `kind` selects MB_OK/MB_YESNO/MB_ABORTRETRYIGNORE/MB_YESNOCANCEL - the
//  four values the switch produces (0, 1, 4, 3) are Win32's own uType
//  constants for exactly those, in that order.
//============================================================================
//  0x6F6B7FC0 / 0x6F6B7FF0 - a shared ref-counted repeat timer's
//  start/stop, below this batch's own 43; declared and thunked to their own
//  real addresses so both call sites match the dump (`call sub_6F6B7Fxx`,
//  no arguments).
__declspec(naked) void __fastcall SetTimerForPrompt()
{
    __asm { mov eax, 06F6B7FC0h }
    __asm { jmp eax }
}
__declspec(naked) void __fastcall KillTimerForPrompt()
{
    __asm { mov eax, 06F6B7FF0h }
    __asm { jmp eax }
}

static const char ValueName[] = "";   // IDA's own (unresolved) name for a fallback caption string; content not score-relevant

int __fastcall ShowTextPromptFromWide(HWND hWnd, int kind, const wchar_t* captionWide,
                                       const wchar_t* textWide)
{
    char caption[0x104];
    caption[0] = 0;
    memset(caption + 1, 0, 0x103);

    if (captionWide != 0)
    {
        ConvertWideTextToNarrow(captionWide);
        // Storm_501 = SStrCopy(dest, src, maxlen)
        extern void __stdcall Storm_501(char* dest, const char* src, unsigned int maxLen);
        Storm_501(caption, lpchText, 0x104);
    }

    ConvertWideTextToNarrow(textWide);

    unsigned int uType;
    switch (kind)
    {
        case 0: uType = 0; break;
        case 1: uType = 1; break;
        case 2: uType = 4; break;
        case 3: uType = 3; break;
        default: uType = 0; break;
    }

    SetTimerForPrompt();

    const char* lpCaption = (captionWide != 0) ? caption : ValueName;
    int mbResult = MessageBoxA(hWnd, lpchText, lpCaption, uType);

    KillTimerForPrompt();

    if (mbResult == IDOK || mbResult == IDYES)
        return 0;
    if (mbResult == IDNO)
        return 1;
    return 2;
}
