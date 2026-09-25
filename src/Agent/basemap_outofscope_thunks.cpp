//============================================================================
//  Naked real-signature thunks for basemap.h's own declared-but-not-defined
//  out-of-scope callees (SBaseMapCellStorage/SBaseMapRealDtorTarget) - the
//  spatial-map cluster's own InitGrid/Teardown/ScalarDeletingDestructor
//  bodies call into these five leaves, but reconstructing their own bodies
//  was outside that session's scope cap (see basemap.h's own comments on
//  each address). link_check.py finds a declared-but-undefined member the
//  moment anything actually calls it - same seam as
//  Agent/rlagentdef_save.cpp's own SaveRecordArray - so each gets the
//  established "real signature, naked redirect" thunk shape
//  (Pathfinding/pathref_load.cpp's SPathRefArrayGrow::SetAlloc) rather than
//  staying merely declared: real callers keep calling these by their real
//  C++ name/signature, and the .mix links, while the real body underneath
//  each address stays exactly what it always was until a future session
//  reconstructs it for real.
//============================================================================
#include "basemap.h"

namespace NIpse {

__declspec(naked) void SBaseMapCellStorage::AllocateCells_6F484810(const void*, unsigned int)
{
    __asm { mov eax, 06F484810h
            jmp eax }
}

__declspec(naked) void SBaseMapCellStorage::AllocateCells_6F49EC90(const void*, unsigned int)
{
    __asm { mov eax, 06F49EC90h
            jmp eax }
}

__declspec(naked) int SBaseMapCellStorage::ShiftCells_6F484890(unsigned int, unsigned int)
{
    __asm { mov eax, 06F484890h
            jmp eax }
}

__declspec(naked) int SBaseMapCellStorage::ShiftCells_6F49ED10(unsigned int, unsigned int)
{
    __asm { mov eax, 06F49ED10h
            jmp eax }
}

__declspec(naked) void SBaseMapCellStorage::ResetCells_6F4847E0()
{
    __asm { mov eax, 06F4847E0h
            jmp eax }
}

__declspec(naked) void SBaseMapCellStorage::ResetCells_6F49EAB0()
{
    __asm { mov eax, 06F49EAB0h
            jmp eax }
}

__declspec(naked) void* SBaseMapRealDtorTarget::RealDtor_6F486A40()
{
    __asm { mov eax, 06F486A40h
            jmp eax }
}

__declspec(naked) void* SBaseMapRealDtorTarget::RealDtor_6F49F190()
{
    __asm { mov eax, 06F49F190h
            jmp eax }
}

}  // namespace NIpse
