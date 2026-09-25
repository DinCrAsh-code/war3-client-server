//============================================================================
//  0x6F37CCC0 - CPathingMapIndicator::~CPathingMapIndicator(), `retn 0`.
//
//  Carries the same unreproducible `__except_handler4`-shaped SEH frame
//  the constructor next door does - see pathingmapindicator_ctor.cpp's own
//  header note and docs/msvc-vc8-idioms.md's "An `__except_handler4`-
//  shaped frame this toolchain cannot reproduce".  Reconstructed for the
//  real body; the frame is not attempted.
//
//  Frees six owned pointers (Storm_403, each its own RTTI type-descriptor
//  tag - a generic-container free, not a scalar-deleting destructor call:
//  none of the six run any per-element destructor first, matching the
//  dump's own bare `test`/`jz`/`Storm_403` shape with no call in between),
//  in descending offset order (+0x78 down to +0x30), then re-stamps this
//  object's own vtable pointer back to `TRefCnt` before returning.
//============================================================================
#include "pathingmapindicator.h"

void __stdcall SMemFree(void* ptr, const char* logfilename, int logline,
                         unsigned int flags);

static const char kTypeTag_Vec2[] = ".?AV?$vector@PAVC3Vector@NTempest@@V?$allocator@PAVC3Vector@NTempest@@@std@@@std@@";  // aAvc2vectorNtem (approximate - not independently resolved this session)
static const char kTypeTag_Vec3[] = ".?AV?$vector@PAVC3Vector@NTempest@@V?$allocator@PAVC3Vector@NTempest@@@std@@@std@@";  // aAvc3vectorNtem (approximate - not independently resolved this session)
static const char kTypeTag_G[]    = "G";      // aG_4 (as literally rendered - not independently resolved this session)
static const char kTypeTag_Im[]   = ".?AV?$vector@HV?$allocator@H@std@@@std@@";  // aAvcimvectorNte (approximate - not independently resolved this session)

CPathingMapIndicator::~CPathingMapIndicator()
{
    if (m_vec78 != 0)
        SMemFree(m_vec78, kTypeTag_Vec2, -2, 0);
    if (m_vec6C != 0)
        SMemFree(m_vec6C, kTypeTag_Vec3, -2, 0);
    if (m_g60 != 0)
        SMemFree(m_g60, kTypeTag_G, -2, 0);
    if (m_vec54 != 0)
        SMemFree(m_vec54, kTypeTag_Vec3, -2, 0);
    if (m_vec44 != 0)
        SMemFree(m_vec44, kTypeTag_Vec3, -2, 0);
    if (m_vec30 != 0)
        SMemFree(m_vec30, kTypeTag_Im, -2, 0);

    TRefCnt::~TRefCnt();
}

//----------------------------------------------------------------------------
//  vtable slots 1-4 - all out of this session's own scope (no dump).
//  Naked thunks, kept only so this class's own vtable is the real
//  five-slot shape.
//----------------------------------------------------------------------------
__declspec(naked) TRefCnt* CPathingMapIndicator::DeleteSelf(int /*flags*/)
{
    __asm { mov eax, 06F381510h }
    __asm { jmp eax }
}

__declspec(naked) void CPathingMapIndicator::Slot2()
{
    __asm { mov eax, 06F378AA0h }
    __asm { jmp eax }
}

__declspec(naked) void CPathingMapIndicator::Slot3()
{
    __asm { mov eax, 06F378C70h }
    __asm { jmp eax }
}

__declspec(naked) void CPathingMapIndicator::SetArg0(void* /*arg0*/)
{
    __asm { mov eax, 06F377BD0h }
    __asm { jmp eax }
}
