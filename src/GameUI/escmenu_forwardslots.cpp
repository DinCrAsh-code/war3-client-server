//============================================================================
//  0x6F2F1650 / 0x6F2F1660 - CEscMenu's vtable slots 7 (+0x1C) and 8
//  (+0x20), both new.  See escmenu.h.
//
//  A different address neighbourhood from the rest of this class's own
//  slots (0x6F34Fxxx) - these two sit beside CGameUI::GetCamera and
//  CGameUI::PostCameraEvents (0x6F2F5xxx-adjacent territory), so they did
//  not ship in the same module as the constructor/destructor/DeleteSelf/
//  Method_0x0C/Method_0x18 above and get their own file rather than being
//  folded in with those.  Own TU together because 0x6F2F1660 reaches
//  0x6F2F1650 only through the vtable (a virtual call, never inlinable
//  regardless of translation unit), so nothing forces them apart either.
//============================================================================
#include "escmenu.h"

//  0x6F2F1650 - `retn 0`.  Byte-identical to Method_0x18's own body
//  (escmenu_method18.cpp) at a second address - the shipped link has no
//  /OPT:ICF, so two functions with the same three instructions stay two
//  functions (docs/msvc-vc8-idioms.md, "Four identical functions at four
//  addresses is normal here").
int CEscMenu::Method_0x1C()
{
    return 1;
}

//  0x6F2F1660 - `retn 0`.  Three instructions: `mov eax,[ecx]` /
//  `mov edx,[eax+18h]` / `jmp edx` - a tail call through `this`'s own
//  vtable at offset 0x18 (slot 6), i.e. a plain virtual call to
//  Method_0x18 rather than a duplicate of its body.
int CEscMenu::Method_0x20()
{
    return Method_0x18();
}
