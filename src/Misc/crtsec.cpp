//============================================================================
//  See crtsec.h.
//============================================================================
#include "crtsec.h"

//  **`extern`, never a definition** - funcmap.py's DATA table binds this to
//  dword_6FAAE140, the game's own /GS cookie (the dumps are full of the
//  `mov eax, dword_6FAAE140` / `xor eax, esp` pair that loads it), and
//  build_mix.py's gen_data.asm only emits that binding for a symbol that
//  comes back EXTRN - a definition here silently cancels it and leaves the
//  .mix with a private zero.  Every user of this name in src/ is a
//  reconstruction of *game* code that has to agree with live game state:
//  SecurityCheckCookie below is the game's own 0x6F7E1059, so once it is
//  hooked every /GS-protected function in the game compares its frame
//  against whatever is here - a zero would fail all of them straight into
//  __report_gsfailure.  Same for the SEH4 frame cookie in crtseh4.cpp,
//  which the game's own unhooked __except_handler4 validates.
//
//  Nothing in *this* build generates a cookie check of its own to disagree
//  with: the flags are fixed at /GS-.
//
//  See observer.cpp's g_observerResourcePool for the same defect found the
//  hard way, and tools/link_check.py's DATA-BINDING section for the check
//  that now catches the whole class.
extern unsigned int g_securityCookie;

#pragma warning(disable : 4035)     // no return value (both __asm paths leave
                                    // through rep-ret or the tail jmp)

//  0x6F7E1059
__declspec(naked) void __fastcall SecurityCheckCookie(unsigned int)
{
    __asm
    {
        cmp     ecx, g_securityCookie
        jnz     fail
        rep     ret
fail:
        jmp     __report_gsfailure
    }
}
