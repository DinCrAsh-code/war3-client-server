//============================================================================
//  0x6F6D7390 - CreateFileA behind a null check and a range check on the
//  creation disposition.
//
//  `tag` is the fourth stack argument and the body never reads it; the one
//  call site in this tree (NetGameLoadFile's constructor, 0x6F5341D0) passes
//  0x3F3F3F3F, i.e. '????'.  It is load-bearing all the same: the function
//  is callee-cleanup with `retn 10h`, so a declaration one argument short
//  leaves four bytes of the caller's frame behind on every call.
//============================================================================
#include "os.h"

HANDLE __fastcall OsCreateFile(const char* lpFileName, DWORD dwDesiredAccess,
                               DWORD dwShareMode, DWORD dwCreationDisposition,
                               DWORD dwFlagsAndAttributes, DWORD tag)
{
    (void)tag;

    //  Two separate tests, not one `||`: the shipped stream emits the
    //  `or eax,-1` / `retn 10h` pair once and branches *backwards* into it
    //  from the second test, which is what two ifs compile to and what a
    //  short-circuit `||` does not.
    if (lpFileName == 0)
        return INVALID_HANDLE_VALUE;
    if (dwDesiredAccess == 0)
        return INVALID_HANDLE_VALUE;

    //  ... and the range check the other way round, so the failure arm is
    //  laid out after the call rather than duplicated in front of it.
    if (dwCreationDisposition - 1 <= 4)
        return CreateFileA(lpFileName, dwDesiredAccess, dwShareMode, 0,
                           dwCreationDisposition, dwFlagsAndAttributes, 0);

    return INVALID_HANDLE_VALUE;
}
