//============================================================================
//  0x6F6C4BF0 - the UI language id: prefer Vista+'s
//  GetSystemDefaultUILanguage (loaded dynamically through kernel32.dll,
//  since this is a pre-Vista-targeted build and the export may not exist),
//  falling back to the always-present GetSystemDefaultLangID.
//============================================================================
#include <windows.h>

unsigned short __fastcall GetSystemUILanguageId()
{
    typedef unsigned short(WINAPI *GetSystemDefaultUILanguageFn)();

    HMODULE hModule = LoadLibraryA("kernel32.dll");
    if (hModule != 0)
    {
        GetSystemDefaultUILanguageFn fn =
            (GetSystemDefaultUILanguageFn)GetProcAddress(hModule, "GetSystemDefaultUILanguage");
        if (fn != 0)
        {
            unsigned short lang = fn();
            FreeLibrary(hModule);
            return lang;
        }
        FreeLibrary(hModule);
    }

    return (unsigned short)GetSystemDefaultLangID();
}
