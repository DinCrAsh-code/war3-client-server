//============================================================================
//  0x6F01FD60 - `retn 4`, `__fastcall(a in ecx)` plus one stack argument
//  (b).  `fcompp`/`fnstsw ax`/`test ah,1` on `*b` vs `*a`: returns 1 when
//  `*b >= *a` (C0 clear), 0 when `*b < *a` (C0 set) - the classic
//  "is a already <= b" ascending-order predicate.
//============================================================================

int __fastcall IsFloatLessOrEqual(const float* a, const float* b)
{
    return (*b >= *a) ? 1 : 0;
}
