//============================================================================
//  0x6F4C4500 - ShutdownStringManagerB3 (gamemain-batch-3).  Tell
//  g_pStringManager to release itself through its own vtable slot 2 (a
//  plain `struct`, not a real C++ class here - see cstringrep.h's own note
//  on why CStringManager's vtable is hand-stamped rather than emitted; no
//  committed vtable dump exists for it so this dispatch is judged
//  "unknown" rather than clean by vtable_dispatch_audit.py), null the
//  global either way, then release every block of a second, unrelated
//  CDataAllocator pool at unk_6FAAE43C through the generic
//  ReleaseAllBlocks (0x6F4C1A60, Net/dataallocator.h/.cpp) - passing the
//  literal `1`/`-2` the shipped call site pushes verbatim, not a real
//  tag/line pair (see ReleaseAllBlocks's own header comment).
//============================================================================
#include "cstringrep.h"
#include "../Net/dataallocator.h"

extern CDataAllocator g_cstringPoolB3;   // unk_6FAAE43C

void ShutdownStringManagerB3()
{
    if (g_pStringManager)
    {
        typedef void (__thiscall *ReleaseSlotFn)(CStringManager*, int);
        (*(ReleaseSlotFn**)g_pStringManager)[2](g_pStringManager, 1);
    }

    g_pStringManager = 0;

    g_cstringPoolB3.ReleaseAllBlocks((const char*)1, -2, 0);
}
