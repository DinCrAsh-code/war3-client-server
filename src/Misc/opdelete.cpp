//============================================================================
//  0x6F001000 - this module's global `operator delete`, routed through
//  Storm's allocator with the tag "delete" and line -1 rather than a real
//  __FILE__/__LINE__ - the shipped code has no debug info to hand it here,
//  unlike CMemBlock's SetSize, which carries its own file/line pair (see
//  src/cellbuffer_debug.cpp).  __cdecl matches the bare `retn` (no operand):
//  the caller cleans its own single argument.
//
//  Named `ModuleOperatorDelete` rather than declared as the language-level
//  `operator delete(void*)` overload: matching_decomp_injection's own
//  glue.cpp already defines that global operator for the whole injected
//  DLL (needed generically, independent of any one target), and a second
//  definition here is a hard `LNK2005` duplicate-symbol link failure in
//  that build even though verify.py's own per-TU compile never links at
//  all and cannot see the clash.  Same instructions either way.
//============================================================================
#include "storm.h"

void __cdecl ModuleOperatorDelete(void* ptr)
{
    if (ptr)
        SMemFree(ptr, "delete", -1, 0);
}
