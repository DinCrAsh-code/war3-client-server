//============================================================================
//  0x6F71D0C0 - CItem vtable slot 32/104 closure (depth 7).  A plain
//  `lo + rand() % (hi - lo)` helper: __fastcall, ecx = lo, edx = hi.  When
//  hi <= lo the range is empty and lo itself is handed back untouched - the
//  `test edi,edi / jg` guard, where edi is `hi - lo`.  `retn 0`: both
//  arguments are register-passed and nothing is on the stack.
//============================================================================
extern "C" int __cdecl rand();

int __fastcall RandomIntInRange(int lo, int hi)
{
    int range = hi - lo;
    if (range <= 0)
        return lo;
    return lo + rand() % range;
}
