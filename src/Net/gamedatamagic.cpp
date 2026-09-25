//============================================================================
//  0x6F537F10 - does the stream's header carry the magic this build expects?
//
//  Two answers, and the first is the interesting one: a header that carries
//  *no* magic at all (zero) is accepted when this build's own tag is the
//  base game's 'WAR3', which is what lets a stream written before the field
//  existed still open.  Under the expansion's 'W3XP' it is not.
//============================================================================

//  0x6F537EF0 - Misc/sweep_predicates.cpp, already reconstructed as
//  GetProductTag_6F537EF0: this build's own four-character tag, 'W3XP' or
//  'WAR3'.  It takes nothing and touches nothing but eax.
//
//  It was *also* redirected here under a second name, GameDataReadMagic, on
//  the reading that it answered a (found, expected) pair - two C++ names for
//  one shipped address, which under the injection build's /FORCE:MULTIPLE
//  means whichever object comes first on the link line wins.  The redirect
//  is gone.
unsigned int GetProductTag_6F537EF0();

//  Naked, and not by preference: the magic to test arrives in ecx and has to
//  survive the call, which is a whole-program assumption about
//  GetProductTag_6F537EF0's own register use that no MSVC spelling can make
//  (docs/msvc-vc8-idioms.md, "A callee that never sets ecx is relying on the
//  caller to have left it there").  Written as ordinary C++ the value is
//  spilled to the frame and reloaded.
__declspec(naked) int __fastcall GameDataMagicMatches(unsigned int magic)
{
    __asm
    {
        call    GetProductTag_6F537EF0
        test    ecx, ecx
        jnz     compare
        cmp     eax, 057415233h
        jnz     compare
        mov     eax, 1
        retn
    compare:
        xor     edx, edx
        cmp     ecx, eax
        setz    dl
        mov     eax, edx
        retn
    }
}
