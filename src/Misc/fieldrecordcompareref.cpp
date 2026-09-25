//============================================================================
//  0x6F6F3820 - SFieldRecordCompareRef::NormalizeCompare: normalises
//  `this->vtbl[6](key, 0x0C)` (0x0C = a fixed compare length) to a plain
//  0/1 via the standard `neg/sbb/neg` boolify idiom
//  (docs/msvc-vc8-idioms.md) - `result != 0` compiles to the identical
//  sequence.  Called from SOwnedRefChecksumCache::FoldRecord's own case 3
//  (ownedrefchecksumcachefoldrecord.cpp) against a receiver that function's
//  own vtable slot 2 returns - a different object from the cache/record it
//  is itself called with, hence the separate receiver type.
//============================================================================
#include "customdatachecksumcache.h"

int __thiscall SFieldRecordCompareRef::NormalizeCompare(unsigned int key)
{
    typedef int (__thiscall *CompareUpToFn)(void*, unsigned int, int);
    int result = ((CompareUpToFn)(*(void***)this)[6])(this, key, 0x0C);
    return result != 0;
}
