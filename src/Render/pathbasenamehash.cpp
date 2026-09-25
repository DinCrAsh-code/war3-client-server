//============================================================================
//  0x6F4F5C60 - copy `src` into the caller's `dest` buffer (0x104 =
//  MAX_PATH bytes, Storm_501's own safe-strcpy shape - cstatus.h/
//  allochashtable.h), and, if the copy is at least 4 characters long and
//  its fourth-from-last character is a literal period (a 3-letter
//  extension, e.g. ".mdx"/".blp"), truncate `dest` right there and hash the
//  base name through Storm_590 into `*outHash`, returning 1.  Otherwise
//  `dest` is left an empty string, `*outHash` is set to 0, and the function
//  returns 0.
//
//  A free `__fastcall` and not a member: the shipped body never treats
//  `ecx` as `this` (it is Storm_501's own `src` argument, not read back
//  afterward), which is what a member declaration cannot spell.  No caller
//  in this batch's own dump.  `retn 4` - one stack dword (`dest`).
//
//  Storm_501 is declared here exactly as every other caller in this repo
//  declares it (`void __stdcall`, cstatus.h/allochashtable.h) rather than
//  with a return type that would suit this function's own use of the
//  length it hands back in `eax` - a `__stdcall` C++ symbol's mangled name
//  *does* encode its return type (unlike a plain `extern "C"` `_name@N`
//  decoration), so a locally re-declared `unsigned int` prototype for the
//  same import mangles to a second, different symbol gen_storm.lib does
//  not provide - link_check.py caught exactly this as a live `.mix` link
//  failure.  The real length is read back with a raw `mov`, right after
//  the call, into a local the C++ side can then use normally.
//============================================================================
#include "storm.h"

void         __stdcall Storm_501(char* dest, const char* src, unsigned int maxLen);
unsigned int __stdcall Storm_590(const char* s);

int __fastcall HashPathBaseNameStrippingExtension(const char* src, unsigned int* outHash,
                                                   char* dest)
{
    unsigned int len;

    Storm_501(dest, src, 0x104);
    __asm { mov len, eax }

    if (len >= 4 && dest[len - 4] == '.')
    {
        dest[len - 4] = 0;
        *outHash = Storm_590(dest);
        return 1;
    }

    dest[0] = 0;
    *outHash = 0;
    return 0;
}
