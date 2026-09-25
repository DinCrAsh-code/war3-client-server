//============================================================================
//  0x6F0071B0 - release one AUFixedString-tagged block out of `this`+8 if
//  it is non-null.  SMemFree is Storm.h's own SMemAlloc/SMemFree pair
//  (Storm ordinal 403), already reconstructed.
//============================================================================
#include "storm.h"

static const char aAuFixedstringD[] = ".?AVAUFixedString@@";   // type descriptor name, content not score-relevant

void __fastcall ReleaseFixedStringField(void* self)
{
    void* p = *(void**)((char*)self + 8);
    if (p != 0)
        SMemFree(p, aAuFixedstringD, -2, 0);
}
