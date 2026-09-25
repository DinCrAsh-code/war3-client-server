//============================================================================
//  0x6F468700 - the body of CPathSearch::FindFreePoint's __finally.
//
//  A real function in the shipped build: the compiler-generated funclet does
//  nothing but form the frame pointer (`lea ecx, [ebp+var_18]`) and tail-jump
//  here, which is what an `__finally` whose whole body is one call to an
//  out-of-line helper produces.  Its own translation unit for the same
//  reason - inlined, there would be nothing at this address.
//============================================================================
#include "pathfind.h"

void __fastcall RestoreSearchFlag(SPathSearchSave* save)
{
    save->m_search->m_searching = save->m_saved;
}
