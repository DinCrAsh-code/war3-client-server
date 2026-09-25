//============================================================================
//  CAbilityCustomData::Method3 - vtable slot 3, 0x6F251BB0.  Confirmed this
//  session to be the exact same shape as the six siblings' own slot 3
//  overrides (Item/customdatasiblings_method3.cpp, checksum-batch-B):
//  read a field at this+0x08, lazily fetch a per-class "DB" singleton
//  through sub_6F71C410, thread the result through the shared
//  sub_6F6F4B50 one-line store, lazily fetch a second per-class "MetaDB"
//  singleton through sub_6F71C890, then tail-jump into the shared
//  sub_6F7057A0 (clears two dynamic arrays at this+0x1CC/this+0x1F4) -
//  byte-for-byte the same skeleton as CBuffCustomData::Method3 (0x6F251BD0)
//  and the rest, just with this class's own two DB/MetaDB helper addresses.
//  Both helpers carry the identical __except_handler4-shaped SEH frame
//  docs/msvc-vc8-idioms.md already documents as unreproducible with this
//  toolchain's fixed /GS- /EHs-c-, so - same as every sibling - this is
//  filed as a self-redirect to its own real address rather than an empty
//  stub: CLAUDE.md's own "SELF-REDIRECT THUNK" shape, which hookset.py
//  already knows to treat as unhookable.  This closes CAbilityCustomData's
//  own last open slot (abilitycustomdata.h's own note above the class).
//============================================================================
#include "abilitycustomdata.h"

//  0x6F251BB0.
__declspec(naked) void CAbilityCustomData::Method3()
{
    __asm { mov eax, 06F251BB0h }
    __asm { jmp eax }
}
