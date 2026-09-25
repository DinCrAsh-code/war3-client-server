//============================================================================
//  0x6F333760 - turn preference 7 into the model detail flags a sprite is
//  built with.
//
//  Two independent bits come out of it:
//
//    0x200  set when preference 7 is *below* 2 (the `cmp/sbb/and` triple:
//           sbb turns "borrow" into all-ones, and 0x200 masks it) - so a
//           low detail setting adds it, and it is added regardless of the
//           caller's own flag.
//    0x008  set only when the caller's own ecx flag is non-zero *and*
//           preference 7 is non-zero.
//
//  `this` in ecx is that caller flag and nothing else - the function has no
//  stack arguments at all (`retn`), and its own caller
//  (CWar3Image::CreateSpriteFromDescriptor) has already pushed two arguments
//  belonging to a later call by the time it runs.
//
//  Own translation unit: the GetPreferences and GetIntValue calls are real.
//============================================================================
#include "preferences.h"

int __fastcall GetModelDetailFlags(int wantsFullDetail)
{
    //  Deliberately uninitialised: the shipped code takes this slot's
    //  address and passes it straight in, with no store of zero in front.
    int preference;
    GetPreferences()->GetIntValue(7, &preference);

    //  Unsigned, and that is what produces the shipped `cmp / sbb / and`
    //  triple: an unsigned compare leaves the answer in the carry flag and
    //  `sbb reg,reg` turns it into an all-ones mask for the `and`.  The
    //  signed spelling goes through `setge` / `sub` instead and is three
    //  instructions longer.
    int flags = ((unsigned int)preference < 2u) ? 0x200 : 0;
    //  The extra bit goes into its own variable and the `|` happens once, at
    //  a single return.  MSVC's jump threader then duplicates that `or` into
    //  both arms, which is what the shipped pair of exits is - one
    //  `mov eax,8 / or eax,ecx`, one `xor eax,eax / or eax,ecx`.  Returning
    //  `8 | flags` and `0 | flags` directly instead lets it fold the second
    //  one away and collapses the two exits into one.
    int extra;
    if (wantsFullDetail != 0 && preference != 0)
        extra = 8;
    else
        extra = 0;
    return extra | flags;
}
