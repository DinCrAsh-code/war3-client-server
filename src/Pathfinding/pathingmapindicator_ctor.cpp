//============================================================================
//  0x6F381430 - CPathingMapIndicator::CPathingMapIndicator(arg0, arg4,
//  arg8), `retn 0Ch`.
//
//  Carries the same unreproducible `__except_handler4`-shaped SEH frame
//  Pathfinding/pathingfootprint_ctor.cpp's own header note documents in
//  full (docs/msvc-vc8-idioms.md) - reconstructed for the real body, the
//  frame not attempted.
//
//  Zeroes four floats (+0x08/+0x0C/+0x10/+0x14, via `fldz`/`fst`/`fst`/
//  `fst`/`fstp` - one load, four stores, the last popping the FPU stack)
//  and every dword from +0x18 through +0x78 except +0x20/+0x24 (which take
//  the constructor's own second/third arguments) and +0x38 (never zeroed -
//  transcribed exactly as the dump has it, not an omission), then default-
//  constructs the +0x7C sub-object - 0x6F526F70 is TEXTUREIMAGE's own
//  constructor, already reconstructed (Render/textureimagector.cpp) and
//  called directly here rather than re-thunked; sizes match exactly
//  (TEXTUREIMAGE is 0x18 bytes, m_kind/m_result[2]/m_extra, the same
//  0x18 bytes this sub-object occupies) - calls this class's own vtable slot 4 directly
//  (`SetArg0(arg0)` - not through the vtable, standard "call my own
//  not-yet-overridable-from-here virtual directly" constructor shape) and
//  finally a registration/attach call (sub_6F37CDA0, also out of scope)
//  with the two stored arguments.
//============================================================================
#include "pathingmapindicator.h"

#define PATHINGMAPINDICATOR_CTOR_THUNK(name, addr) \
    __declspec(naked) void name() { __asm { mov eax, addr } __asm { jmp eax } }

//  0x6F526F70 IS TEXTUREIMAGE::TEXTUREIMAGE, already reconstructed
//  (Render/textureimagector.cpp) - this local redirect calls that same
//  real address directly (not through `new`/a normal C++ constructor
//  call, which this compiler is free to reorder ahead of the field
//  zeroing above it, unlike the shipped stream's own late placement) so
//  the call lands at the same point in the instruction stream the dump
//  has it. Deliberately NOT re-registered as a thunk in agent_worktrees -
//  0x6F526F70's own entry already correctly records the real
//  reconstruction and this is just a second, harmless redirect to the
//  same address, the same way this repo's own /OPT:ICF-free image
//  sometimes has one body reached from two call sites (CLAUDE.md's own
//  "6 name(s) are hookable at more than one address" note).
PATHINGMAPINDICATOR_CTOR_THUNK(ConstructTextureImageAt, 06F526F70h)

//  0x6F37CDA0 - registration/attach call with the two stored arguments.
//  Out of this session's own scope (no dump/name) - thunked.
PATHINGMAPINDICATOR_CTOR_THUNK(AttachIndicatorThunk, 06F37CDA0h)

static void AttachIndicator(CPathingMapIndicator* self, void* a, void* b)
{
    __asm
    {
        push a
        push b
        mov ecx, self
        call AttachIndicatorThunk
    }
}

CPathingMapIndicator::CPathingMapIndicator(void* arg0, void* arg4, void* arg8)
{
    m_refcount = 0;

    m_f08 = CFloat(0);
    m_f0C = CFloat(0);
    m_f10 = CFloat(0);
    m_f14 = CFloat(0);

    m_reserved18 = 0;
    m_reserved1C = 0;
    m_arg4 = arg4;
    m_arg8 = arg8;

    *(unsigned int*)m_reserved28 = 0;
    *(unsigned int*)(m_reserved28 + 4) = 0;
    m_vec30 = 0;
    *(unsigned int*)m_reserved34 = 0;          // +0x34
    //  +0x38 is genuinely never zeroed by the dump - left alone here too,
    //  matching it exactly (not an omission).
    *(unsigned int*)(m_reserved34 + 8) = 0;    // +0x3C
    *(unsigned int*)(m_reserved34 + 0xC) = 0;  // +0x40
    m_vec44 = 0;
    *(unsigned int*)m_reserved48 = 0;
    *(unsigned int*)(m_reserved48 + 4) = 0;
    *(unsigned int*)(m_reserved48 + 8) = 0;
    m_vec54 = 0;
    *(unsigned int*)m_reserved58 = 0;
    *(unsigned int*)(m_reserved58 + 4) = 0;
    m_g60 = 0;
    *(unsigned int*)m_reserved64 = 0;
    *(unsigned int*)(m_reserved64 + 4) = 0;
    m_vec6C = 0;
    *(unsigned int*)m_reserved70 = 0;
    *(unsigned int*)(m_reserved70 + 4) = 0;
    m_vec78 = 0;

    {
        void* subObject = &m_subObject7C;
        __asm
        {
            mov ecx, subObject
            mov byte ptr [esp - 4], 6   //  the dump's own `var_4 = 6` tag, alive across the call
            call ConstructTextureImageAt
        }
    }

    SetArg0(arg0);

    AttachIndicator(this, m_arg8, m_arg4);
}
