//============================================================================
//  0x6F3EC5A0 - CPathingFootprint::CPathingFootprint(shape, haveExtent),
//  `retn 8`.
//
//  **Carries the `__except_handler4`-shaped SEH frame this toolchain
//  cannot reproduce** (docs/msvc-vc8-idioms.md, "An `__except_handler4`-
//  shaped frame this toolchain cannot reproduce") - `push -1` / `push
//  offset SEH_6F3EC5A0` / `mov eax, fs:0`, a cookie XORed into `esp` with
//  no EBP frame, and a trampoline block appended after the function's own
//  `retn` that checks the cookie and tail-jumps `__CxxFrameHandler3`. That
//  doc's own precedent (`sub_6F42CB50`/`sub_6F02F780`, Agent/agent.cpp)
//  is followed here: reconstruct the real body, do not attempt the frame
//  (this repo's fixed `/GS- /EHs-c-` was hand-tested there and cannot
//  reach it with any spelling), and take the resulting score hit rather
//  than leave the function unreconstructed.
//
//  What each call does, to the depth this session could confirm from the
//  dump alone (no live game access this session):
//    - `sub_6F72E9C0(shape, &m_gridDimB, &m_gridDimA, &tmp)` - resolves
//      `shape` into this footprint's own grid dimensions (thiscall on
//      `shape`, so it is almost certainly `CFootprintShape`-family, but
//      the exact method is out of this session's own scope - no dump);
//      `eax != 0` means the grid resolved.
//    - `sub_6F3E95F0(&m_cellCount, cellCount)` - allocates the per-cell
//      byte array (m_cellFlags ends up pointing inside/at whatever this
//      returns - out of scope, no dump).
//    - `CFloatFromInt(dim) * g_halfCellOffset` (already-reconstructed
//      Math/CFloat.h/Pathfinding/footprintdir.cpp machinery) - the
//      world-space half-extent of each grid dimension.
//    - `sub_6F3E0AE0(this)` - some kind of finalize/registration step once
//      the array is filled (out of scope, no dump).
//    - the nested per-cell loop reads three flag bits off a per-cell
//      3-byte record `shape` exposes (through `m_gridDimB`'s own table,
//      indexed `[cellY][cellX]`) and folds them into one flag byte per
//      cell (`0xD2` when bit 2 of byte+2 is set, `|4` + this object's own
//      `m_flags64 |= 1` when byte+1's bit 0 is set, `|8` when byte's own
//      bit 0 is set) - transcribed by shape by reading the dump, not
//      independently confirmed.
//============================================================================
#include "pathingfootprint.h"

extern const CFloat g_halfCellOffset;   // dword_6FAB68A8 - footprintdir.cpp

//  Out of this session's own scope (no dump/name yet) - thunked rather
//  than reconstructed.  Argument shapes read off this constructor's own
//  call sites.
#define PATHINGFOOTPRINT_CTOR_THUNK(name, addr) \
    __declspec(naked) void name() { __asm { mov eax, addr } __asm { jmp eax } }

PATHINGFOOTPRINT_CTOR_THUNK(ResolveFootprintShapeThunk, 06F72E9C0h)
PATHINGFOOTPRINT_CTOR_THUNK(AllocFootprintMaskThunk,     06F3E95F0h)
PATHINGFOOTPRINT_CTOR_THUNK(FinalizeFootprintMaskThunk,  06F3E0AE0h)

static int __fastcall ResolveFootprintShape(void* shape, unsigned int* dimB,
                                             unsigned int* dimA, unsigned char** tmp)
{
    int result;
    __asm
    {
        push tmp
        push dimA
        mov edx, dimB
        mov ecx, shape
        call ResolveFootprintShapeThunk
        mov result, eax
    }
    return result;
}

static void AllocFootprintMask(void* maskArrayField, unsigned int cellCount)
{
    __asm
    {
        push cellCount
        mov ecx, maskArrayField
        call AllocFootprintMaskThunk
    }
}

static void FinalizeFootprintMask(CPathingFootprint* self)
{
    __asm
    {
        mov ecx, self
        call FinalizeFootprintMaskThunk
    }
}

CPathingFootprint::CPathingFootprint(void* shape, void* haveExtent)
{
    m_gridDimA = 0;
    m_extentX = CFloat(0);
    m_extentY = CFloat(0);
    m_cellCount = 0;
    m_cellFlags = 0;
    m_flags64 = 0;
    m_haveExtent = haveExtent;

    unsigned int cellCount = 0;

    if (shape != 0)
    {
        //  `recordsBase` is the dump's own `var_14`: the pointer
        //  ResolveFootprintShape's third (`&tmp`) out-argument resolves to
        //  - this session could not confirm what array it actually points
        //  into (out of scope, no dump for ResolveFootprintShape itself),
        //  so the index arithmetic below is transcribed exactly as the
        //  dump computes it (`x*gridDimB + y`, one 4-byte record each)
        //  rather than re-derived from a confirmed meaning.
        unsigned char* recordsBase = 0;
        if (ResolveFootprintShape(shape, &m_gridDimB, &m_gridDimA, &recordsBase) != 0)
        {
            cellCount = m_gridDimB * m_gridDimA;
            AllocFootprintMask(&m_cellCount, cellCount);

            if (haveExtent != 0)
            {
                m_extentX = CFloatFromInt((int)m_gridDimB) * g_halfCellOffset;
                m_extentY = CFloatFromInt((int)m_gridDimA) * g_halfCellOffset;

                for (unsigned int y = 0; y < m_gridDimA; ++y)
                {
                    for (unsigned int x = 0; x < m_gridDimB; ++x)
                    {
                        const unsigned char* rec = recordsBase + (x * m_gridDimB + y) * 4;
                        unsigned char* out = &m_cellFlags[x * m_gridDimA + y];
                        *out = 0;
                        if (rec[2] & 1)
                            *out = 0xD2;
                        if (rec[1] & 1)
                        {
                            *out |= 4;
                            m_flags64 |= 1;
                        }
                        if (rec[0] & 1)
                            *out |= 8;
                    }
                }

                FinalizeFootprintMask(this);
            }
        }
    }
}
