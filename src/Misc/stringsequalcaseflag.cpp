//============================================================================
//  0x6F71B580 - `retn 4`.  Bounded string-equality test that picks the
//  case-sensitive or case-insensitive Storm compare by a caller-supplied
//  flag.  `!Storm_50x(...)` is MSVC's branchless logical negation of an
//  arbitrary int (`neg`/`sbb`/`add 1` - docs/msvc-vc8-idioms.md), turning
//  "0 means equal" into "nonzero means equal".
//============================================================================
int __stdcall Storm_508(const char* a, const char* b, unsigned int max);
int __stdcall SStrnICmp(const char* a, const char* b, unsigned int max);

//  0x6F71B580 (a in ecx, b in edx, caseInsensitive on the stack; `retn 4`).
int __fastcall StringsEqualCaseFlag(const char* a, const char* b,
                                    int caseInsensitive)
{
    if (caseInsensitive == 0)
        return !Storm_508(a, b, 0x7FFFFFFF);
    return !SStrnICmp(a, b, 0x7FFFFFFF);
}
