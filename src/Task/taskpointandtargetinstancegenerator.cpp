//============================================================================
//  InstanceGenerator<CTaskPointAndTarget>'s own construction path and its
//  three real vtable-slot bodies
//  (`??_7?$InstanceGenerator@VCTaskPointAndTarget@@@@6B@` @ 0x6F92F9F4,
//  agent_worktrees/classes/0x6F92F9F4.json) - mirrors
//  taskpointinstancegenerator.cpp (InstanceGenerator<CTaskPoint>) exactly,
//  same reasons for every shape choice below.
//
//  None of the three is spelled `virtual` here, for this repo's usual
//  reason: nothing in the currently-reachable data ever calls through this
//  vtable indirectly.
//
//  Four addresses:
//    0x6F291C60 - InstanceGenerator<CTaskPointAndTarget>::CreateInstance   (slot 0)
//    0x6F27CF70 - InstanceGenerator<CTaskPointAndTarget>::DestroyInstance  (slot 1)
//    0x6F27CFA0 - InstanceGenerator<CTaskPointAndTarget>::ReleaseAll       (slot 2)
//    0x6F28EFD0 - CDataAllocator::ConstructCTaskPointAndTarget (THUNK - see
//                 dataallocator.h's own comment for why: a real
//                 __except_handler4-shaped SEH frame this toolchain cannot
//                 emit)
//============================================================================
#include "ctaskpointandtarget.h"
#include "ctaskpointandtargetinstancegenerator.h"

//----------------------------------------------------------------------------
//  0x6F291C60 - InstanceGenerator<CTaskPointAndTarget>::CreateInstance,
//  slot 0 (+0x00).  `retn 0`, no arguments.
//----------------------------------------------------------------------------
CTaskPointAndTarget* InstanceGenerator_CTaskPointAndTarget_CreateInstance()
{
    return GetCTaskPointAndTargetGeneratorSingleton()->m_allocator.ConstructCTaskPointAndTarget(0);
}

//----------------------------------------------------------------------------
//  0x6F27CF70 - InstanceGenerator<CTaskPointAndTarget>::DestroyInstance,
//  slot 1 (+0x04).  `retn 4`, one stack argument (the object to tear down).
//----------------------------------------------------------------------------
void __stdcall InstanceGenerator_CTaskPointAndTarget_DestroyInstance(CTaskPointAndTarget* obj)
{
    obj->DeleteSelf(0);
    GetCTaskPointAndTargetGeneratorSingleton()->m_allocator.Free(obj, 0, 0);
}

//----------------------------------------------------------------------------
//  0x6F27CFA0 - InstanceGenerator<CTaskPointAndTarget>::ReleaseAll, slot 2
//  (+0x08).  `retn 0`, no arguments.
//----------------------------------------------------------------------------
void InstanceGenerator_CTaskPointAndTarget_ReleaseAll()
{
    GetCTaskPointAndTargetGeneratorSingleton()->m_allocator.ReleaseAllBlocks(
        ".?AVCTaskPointAndTarget@@", -2, 1);
}

//----------------------------------------------------------------------------
//  0x6F28EFD0 - CDataAllocator::ConstructCTaskPointAndTarget.  THUNK: a real
//  `__except_handler4`-shaped SEH frame this repo's fixed `/GS- /EHs-c-`
//  toolchain cannot emit (see dataallocator.h's own comment on the
//  declaration, and ctaskpointandtarget.h's header comment for what the
//  real body does).  Declared here, in this address neighbourhood, rather
//  than in dataallocator.cpp - same "one translation unit per original
//  module" reasoning taskpointinstancegenerator.cpp already gives for
//  ConstructCTaskPoint.
//----------------------------------------------------------------------------
#define TASKPOINTANDTARGET_CTOR_THUNK(addr)  \
    {                                        \
        __asm { mov eax, addr }              \
        __asm { jmp eax }                    \
    }

__declspec(naked) CTaskPointAndTarget* CDataAllocator::ConstructCTaskPointAndTarget(int /*zeroMemory*/) TASKPOINTANDTARGET_CTOR_THUNK(0x6F28EFD0)
