//============================================================================
//  0x6F4C3E80 - CStatus::ClearEntries.  Two instructions, `add ecx,4` and a
//  jump: the member tail call docs/msvc-vc8-idioms.md records under
//  "`add ecx, <offset>` / `jmp` is a member tail call, not an MI thunk".
//  Its own translation unit for the same reason - in FreeAll's the whole
//  body would inline and there would be no jump left.
//============================================================================
#include "cstatus.h"

void CStatus::ClearEntries()
{
    m_list.FreeAll();
}
