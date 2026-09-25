//============================================================================
//  0x6F6CDF20 - ClassifyWindowsVersion (gamemain-batch-3).  GetVersionExA
//  into a full OSVERSIONINFOA (retrying with the older, shorter struct size
//  if the newer one is refused), then reduce the platform/major/minor triad
//  (plus, for Windows 95, the second byte of the CSD string, which is what
//  tells a plain 95 from the OSR2 refresh) to a small enumerated code:
//
//    0 = unrecognised / GetVersionExA failed entirely
//    1 = Windows 95            2 = Windows 95 OSR2
//    3 = Windows 98            4 = Windows 98 SE
//    5 = Windows ME
//    6 = Windows NT 4
//    7 = Windows 2000          8 = Windows XP        9 = Windows Server 2003
//   10 = Windows Vista        11 = Windows 7
//
//  Every branch below is `lea esi,[eax+K]` off whichever version field is
//  live in `eax` at that point (dwMinorVersion for the 5.x/6.x/9x arms,
//  dwMajorVersion for the NT4 arm) rather than a fresh constant load - kept
//  as `versionField + K` for exactly that reason, even where it reads oddly
//  (`minorVersion + 7` for 2000/XP/2003).
//
//  What is NOT reproduced: the /GS stack-buffer cookie this frame's real
//  0x9C-byte OSVERSIONINFOA carries (`mov eax, dword_6FAAE140` / `xor
//  eax,esp` on entry, the mirrored xor/call sub_6F7E1059 on every exit).
//  This repo's fixed /GS- never emits it - same accepted gap
//  gxdeviceopenglwndproc.cpp and osversionclass's own sibling functions
//  document.  IDENTICAL, not DIFFERS.
//============================================================================
#include <windows.h>

extern "C" void* __cdecl memset(void* dst, int val, unsigned int size);

int ClassifyWindowsVersion()
{
    OSVERSIONINFOA info;
    memset(&info, 0, sizeof(info));

    info.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);
    if (!GetVersionExA(&info))
    {
        info.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA) - sizeof(DWORD);
        if (!GetVersionExA(&info))
            return 0;
    }

    if (info.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
    {
        //  Windows 9x family: only 4.x is recognised at all.
        if (info.dwMajorVersion != 4)
            return 0;

        unsigned int minor = info.dwMinorVersion;
        if (minor == 0)
        {
            int code = minor + 1;   // Windows 95
            char csd1 = info.szCSDVersion[1];
            if (csd1 == 'C' || csd1 == 'B')
                code = 2;            // Windows 95 OSR2
            return code;
        }
        if (minor == 0x0A)
        {
            if (info.szCSDVersion[1] == 'A')
                return minor - 6;   // Windows 98 SE
            return minor - 7;       // Windows 98
        }
        if (minor == 0x5A)
            return 5;                // Windows ME
        return 0;
    }

    if (info.dwPlatformId != VER_PLATFORM_WIN32_NT)
        return 0;

    unsigned int major = info.dwMajorVersion;
    if (major == 4)
        return major + 2;            // Windows NT 4

    if (major == 5)
    {
        unsigned int minor = info.dwMinorVersion;
        if (minor == 0) return minor + 7;   // Windows 2000
        if (minor == 1) return minor + 7;   // Windows XP
        if (minor == 2) return minor + 7;   // Windows Server 2003
        return 0;
    }

    if (major == 6)
    {
        unsigned int minor = info.dwMinorVersion;
        if (minor == 0) return minor + 0x0A;   // Windows Vista
        if (minor == 1) return minor + 0x0A;   // Windows 7
        return 0;
    }

    return 0;
}
