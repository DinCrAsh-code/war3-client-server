//============================================================================
//  0x6F34FC80 - CEscMenu's vtable slot 6 (+0x18), new.  See escmenu.h.
//============================================================================
#include "escmenu.h"

//  0x6F34FC80 - `retn 0`.  Five instructions: `mov eax, 1` / `retn`.
//  Unconditional; no field of `this` is read.
int CEscMenu::Method_0x18()
{
    return 1;
}
