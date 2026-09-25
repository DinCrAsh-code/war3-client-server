//============================================================================
//  0x6F4C3E90 - CStatus::~CStatus.
//
//  Three things happen and only one of them is written: the vtable stamp is
//  the compiler's (a polymorphic class's destructor writes its own vftable
//  first), the entry drain is the one statement, and the list's own
//  destructor - UnlinkAll plus the terminator's Unlink - runs after the body
//  because m_list is a member.
//
//  The shipped body carries the cookie-XORed, frame-pointer-omitted SEH
//  frame docs/msvc-vc8-idioms.md records as unreproducible at this repo's
//  fixed `/GS- /EHs-c-`: the `fs:0` chain, the `SEH_6F4C3E90` trampoline and
//  the four bytes of displacement they put under every `[esp+N]`.  Nothing
//  in the body itself is missing.
//============================================================================
#include "cstatus.h"

CStatus::~CStatus()
{
    ClearEntries();
}
