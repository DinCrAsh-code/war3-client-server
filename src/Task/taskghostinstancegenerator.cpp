//============================================================================
//  InstanceGenerator<CTaskGhost>'s own construction path and its three
//  real vtable-slot bodies (`??_7?$InstanceGenerator@VCTaskGhost@@@@6B@`
//  @ 0x6F92FA24, agent_worktrees/classes/0x6F92FA24.json) - mirrors
//  taskorderparaminstancegenerator.cpp (InstanceGenerator<CTaskOrderParam>)
//  exactly, the reference implementation this whole
//  InstanceGenerator<CTaskXxx> family follows.  This is the last one - see
//  taskghost.h's own header comment for why CTaskGhost is the final class
//  in this wave (and this whole family).
//
//  None of the three is spelled `virtual` here, for the same reason as
//  InstanceGenerator<CTask>'s own (taskinstancegenerator.cpp's own header
//  comment has the fullest version): nothing in the currently-reachable
//  data ever calls through this vtable indirectly.
//
//  Four addresses:
//    0x6F291CC0 - InstanceGenerator<CTaskGhost>::CreateInstance   (slot 0)
//    0x6F27D180 - InstanceGenerator<CTaskGhost>::DestroyInstance  (slot 1)
//    0x6F27D1B0 - InstanceGenerator<CTaskGhost>::ReleaseAll       (slot 2)
//    0x6F28F160 - CDataAllocator::ConstructCTaskGhost (THUNK - see
//                 dataallocator.h's own comment for why: a real
//                 __except_handler4-shaped SEH frame this toolchain cannot
//                 emit)
//============================================================================
#include "taskghost.h"
#include "ctaskghostinstancegenerator.h"

//----------------------------------------------------------------------------
//  0x6F291CC0 - InstanceGenerator<CTaskGhost>::CreateInstance, slot 0
//  (+0x00).  `retn 0`, no arguments: hand the generator's own embedded
//  allocator a hard-coded `zeroMemory=0` and let it Alloc-and-construct a
//  fresh CTaskGhost.
//----------------------------------------------------------------------------
CTaskGhost* InstanceGenerator_CTaskGhost_CreateInstance()
{
    return GetCTaskGhostGeneratorSingleton()->m_allocator.ConstructCTaskGhost(0);
}

//----------------------------------------------------------------------------
//  0x6F27D180 - InstanceGenerator<CTaskGhost>::DestroyInstance, slot 1
//  (+0x04).  `retn 4`, one stack argument (the object to tear down), no
//  register carries anything in - `__stdcall` matches both: call its own
//  (real override, taskghost.h) virtual DeleteSelf(0), then free the block
//  back to the same pool with the tag/line the shipped call site pushes
//  (0, 0), matching CDataAllocator::Free's own established defaulting.
//----------------------------------------------------------------------------
void __stdcall InstanceGenerator_CTaskGhost_DestroyInstance(CTaskGhost* obj)
{
    obj->DeleteSelf(0);
    GetCTaskGhostGeneratorSingleton()->m_allocator.Free(obj, 0, 0);
}

//----------------------------------------------------------------------------
//  0x6F27D1B0 - InstanceGenerator<CTaskGhost>::ReleaseAll, slot 2 (+0x08).
//  `retn 0`, no arguments: release every block the pool has ever grown by
//  back to Storm, tagged with CTaskGhost's own RTTI name.
//----------------------------------------------------------------------------
void InstanceGenerator_CTaskGhost_ReleaseAll()
{
    GetCTaskGhostGeneratorSingleton()->m_allocator.ReleaseAllBlocks(".?AVCTaskGhost@@", -2, 1);
}

//----------------------------------------------------------------------------
//  0x6F28F160 - CDataAllocator::ConstructCTaskGhost.  THUNK: a real
//  `__except_handler4`-shaped SEH frame this repo's fixed `/GS- /EHs-c-`
//  toolchain cannot emit (see dataallocator.h's own comment on the
//  declaration, and taskghost.h's header comment for exactly what the
//  real body does).  Declared here, in this address neighbourhood, rather
//  than in dataallocator.cpp, for the same "one translation unit per
//  original module" reasoning taskorderparaminstancegenerator.cpp's own
//  comment gives for ConstructCTaskOrderParam.
//----------------------------------------------------------------------------
#define CTASKGHOST_CTOR_THUNK(addr)   \
    {                                 \
        __asm { mov eax, addr }      \
        __asm { jmp eax }            \
    }

__declspec(naked) CTaskGhost* CDataAllocator::ConstructCTaskGhost(int /*zeroMemory*/) CTASKGHOST_CTOR_THUNK(0x6F28F160)
