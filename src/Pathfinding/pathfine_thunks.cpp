//============================================================================
//  The fine search's two not-yet-reconstructed leaves, as naked jmp thunks
//  so that build_mix.py's full-DLL link resolves them (verify.py never
//  links, so a declared-only extern compiles clean here and only fails
//  there).  Same pattern as src/unreconstructed_thunks.cpp.
//
//  Both are declared exactly as their call sites use them, so the argument
//  bytes the shipped callee cleans match what this build pushes -
//  tools/thunk_abi_audit.py is what checks that, and both are `retn <n>`
//  callee-cleanup like everything else in this binary.
//============================================================================
#include "pathfinesearch.h"
#include "spatialgrid.h"

//  0x6F4A1080 - four stack arguments.  tools/thunk_abi_audit.py lists this
//  one as unknown rather than checked: there is no dump body for it yet, and
//  the ida_query worker stopped answering before one could be fetched.  The
//  argument count is not a guess, though - all four Neighbours walks push
//  four dwords and none of them adjusts esp afterwards, and everything in
//  this binary is callee-cleanup, so the callee cleans 16 bytes.
__declspec(naked) int CPathFineSearch::TestPerimeter(int, int, int, int)
{
    __asm { mov eax, 06F4A1080h }
    __asm { jmp eax }
}


//  0x6F4A1960 - one stack argument.  The fine search's own closed-list
//  unlink, CPathAStar::Unlink's counterpart.  Unknown to the thunk audit for
//  the same reason as TestPerimeter above, and pinned the same way: its one
//  call site (CPathFineSearch::Relax) pushes one dword and does not adjust
//  esp.
__declspec(naked) void CPathFineSearch::Unlink(int)
{
    __asm { mov eax, 06F4A1960h }
    __asm { jmp eax }
}
