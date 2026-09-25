//============================================================================
//  0x6F4CE050 - parse a config value as an int: a leading `'` packs up to
//  four following characters (stopping at NUL or the closing quote) into a
//  big-endian four-character code; anything else goes through the CRT's
//  own `strtol` with base 0.
//
//  CRASH ROOT CAUSE (found via live debugger, MoveForward_run1): the caller
//  (`sub_6F4D1F80`) does `mov edx, eax` / `call sub_6F4CE050` with no
//  instruction touching `ecx` beforehand at all - the string pointer is the
//  __fastcall function's *second* register argument (edx), not its first
//  (ecx).  The instruction-diff scored the original one-parameter
//  `__fastcall(const char* s)` as IDENTICAL because the differ only checks
//  that registers form a consistent permutation *within* this function's own
//  body, which a single-parameter leaf trivially does regardless of which
//  physical register the parameter lands in - it cannot see that the
//  parameter register is part of the external ABI and must match the real
//  caller.  The live game hooked this straight over the original code
//  address, so it kept being called the original way (arg in edx, ecx
//  untouched/garbage) while this reconstruction read its string pointer out
//  of ecx - garbage in, and the first dereference (`cmp [ecx], 39`) faulted.
//  A leading unused parameter forces the compiler to keep `s` in edx, the
//  same "hidden slot 1 unused" shape `docs/msvc-vc8-idioms.md` documents for
//  `ecx`-return-buffer fastcalls; here the unused slot is a genuine unused
//  argument, not a return buffer, most likely because the original source is
//  a two-argument helper (or a member function with a `this` this call site
//  never needs) that only got compiled once for both use sites.
//============================================================================

//  A real DLL import (`ds:__imp_strtol`), not a Storm ordinal or a compiler
//  intrinsic - declared plain C linkage to match.
extern "C" long __cdecl strtol(const char* s, char** endPtr, int radix);

int __fastcall ParseConfigInt(void* /*unused_ecx*/, const char* s)
{
    int packed = 0;

    if (*s == '\'')
    {
        s++;
        for (int count = 4; count != 0; count--)
        {
            char c = *s;
            if (c == 0 || c == '\'')
                break;
            packed = (packed << 8) | (unsigned char)c;
            s++;
        }
        return packed;
    }

    return (int)strtol(s, 0, 0);
}
