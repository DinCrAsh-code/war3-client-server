//============================================================================
//  0x6F459310/0x6F459320/0x6F459330/0x6F459340 - four `__RTC_NumErrors_NN`
//  symbols (IDA's own names, not invented here): MSVC's real per-source-file
//  `/RTC1` artifact, one `mov eax,4 / retn` stub per translation unit the
//  shipped build compiled with runtime checks on.  This toolchain's own
//  fixed flags (CLAUDE.md's own `/GS- ...`, no `/RTC1`) cannot make the
//  compiler regenerate this shape from an ordinary function body - so, like
//  a thunk, these four are written as naked asm to guarantee the exact two
//  instructions rather than trusted to fall out of `return 4;`.  Real
//  behaviour (however this binary's own RTC table actually uses the
//  constant) is out of this closure's reach; the bytes are the whole claim.
//============================================================================

//  0x6F459310
__declspec(naked) int __RTC_NumErrors_63()
{
    __asm
    {
        mov eax, 4
        retn
    }
}

//  0x6F459320
__declspec(naked) int __RTC_NumErrors_64()
{
    __asm
    {
        mov eax, 4
        retn
    }
}

//  0x6F459330
__declspec(naked) int __RTC_NumErrors_65()
{
    __asm
    {
        mov eax, 4
        retn
    }
}

//  0x6F459340
__declspec(naked) int __RTC_NumErrors_66()
{
    __asm
    {
        mov eax, 4
        retn
    }
}
