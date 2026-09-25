//============================================================================
//  `_chkstk` (`__alloca_probe`), the CRT's own stack probe.
//
//  Not a reconstruction of anything in `asm/` and not in `funcmap.py`: this
//  build compiles with `/Zl`, so no CRT library is on the link line, and any
//  function whose frame exceeds one page makes MSVC emit `call __chkstk`
//  before the frame is opened.  CPreferencesWar3::ApplyStoredPreference
//  (0x6F005BB0, preferenceapplystored.cpp) has a 0x1000-byte buffer and is
//  the first function in this repo to need it; without a definition the
//  whole `.mix` fails to link (LNK2019), which `tools/link_check.py` is
//  there to catch.
//
//  Written from MSVC8's own `vs8/VC/crt/src/intel/chkstk.asm`, trimmed to
//  the x86 branch: touch one dword of every page between the current stack
//  pointer and the requested one so the guard page is hit in order, then
//  move esp down and jump to the caller's return address.  The requested
//  size arrives in eax and the whole thing has to leave every register but
//  eax and the flags alone, which is why it is `__declspec(naked)` and not
//  ordinary C++ - the same technique SecurityCheckCookie (Misc/crtsec.cpp)
//  uses for the same reason.
//
//  GameMain's own closure reaches this address for real (0x6F7E1190,
//  `__alloca_probe` in that dump) and caught a transcription bug the first
//  writing never had a dump to check against: the real source computes
//  `lea ecx, [esp] + 8 - 4`, i.e. `[esp+4]` (the TOS as it was before this
//  function's own `push ecx`, plus the 4 bytes chkstk.asm's own comment
//  reserves "for ret value" and then immediately subtracts back out) - this
//  file had `[esp+8]` instead, four bytes high, which would probe/land one
//  dword short of the real target on every call with a large enough frame.
//============================================================================

extern "C" __declspec(naked) void _chkstk()
{
    __asm
    {
        push    ecx
        lea     ecx, [esp+4]        // ecx = the stack pointer as it was
        sub     ecx, eax
        sbb     eax, eax            // all ones if the subtraction wrapped
        not     eax
        and     ecx, eax            // ... in which case probe down to zero
        mov     eax, esp
        and     eax, 0FFFFF000h

    probe_loop:
        cmp     ecx, eax
        jb      probe_done
        mov     eax, ecx
        pop     ecx
        xchg    eax, esp
        mov     eax, [eax]
        mov     [esp], eax
        ret

    probe_done:
        sub     eax, 1000h
        test    [eax], eax          // touch this page
        jmp     probe_loop
    }
}
