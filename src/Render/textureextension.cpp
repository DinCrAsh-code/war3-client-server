//============================================================================
//  0x6F4DDE80 - which decoder does this file name want?
//
//  0 is `.tga`, 1 is `.blp` and 3 is everything else, including a name with
//  no extension at all.  The shipped code forms the last two out of the
//  compare result rather than branching - `neg` / `sbb eax, eax` /
//  `and eax, 2` / `add eax, 1` - which is what says the two answers are 1
//  and 3 and not 1 and 2, and the no-extension exit is written as
//  `lea eax, [esi+3]` off the register the branch has just proved is zero,
//  which this build reproduces from `return 3` only because esi is already
//  known zero there.
//============================================================================
#include "texture.h"

//  Storm.dll ordinal 572: the last occurrence of a character in a string,
//  the string in ecx and the character in dl.
const char* __fastcall Storm_572(const char* text, char ch);

int __fastcall ClassifyTextureExtension(const char* path)
{
    const char* ext = Storm_572(path, '.');
    if (ext == 0)
        return 3;

    if (SStrnICmp(ext, ".tga", 0x7FFFFFFF) == 0)
        return 0;

    return SStrnICmp(ext, ".blp", 0x7FFFFFFF) == 0 ? 1 : 3;
}
