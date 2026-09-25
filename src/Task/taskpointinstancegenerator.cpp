//============================================================================
//  InstanceGenerator<CTaskPoint>'s own construction path and its three real
//  vtable-slot bodies (`??_7?$InstanceGenerator@VCTaskPoint@@@@6B@` @
//  0x6F92F9D4, agent_worktrees/classes/0x6F92F9D4.json) - mirrors
//  taskinstancegenerator.cpp (InstanceGenerator<CTask>) exactly, same
//  reasons for every shape choice below.
//
//  None of the three is spelled `virtual` here, for this repo's usual
//  reason: nothing in the currently-reachable data ever calls through this
//  vtable indirectly.
//
//  Four addresses:
//    0x6F291C20 - InstanceGenerator<CTaskPoint>::CreateInstance   (slot 0)
//    0x6F27CE10 - InstanceGenerator<CTaskPoint>::DestroyInstance  (slot 1)
//    0x6F27CE40 - InstanceGenerator<CTaskPoint>::ReleaseAll       (slot 2)
//    0x6F28EED0 - CDataAllocator::ConstructCTaskPoint (THUNK - see
//                 dataallocator.h's own comment for why: a real
//                 __except_handler4-shaped SEH frame this toolchain cannot
//                 emit)
//============================================================================
#include "taskpoint.h"
#include "ctaskpointinstancegenerator.h"

//----------------------------------------------------------------------------
//  0x6F291C20 - InstanceGenerator<CTaskPoint>::CreateInstance, slot 0
//  (+0x00).  `retn 0`, no arguments.
//----------------------------------------------------------------------------
CTaskPoint* InstanceGenerator_CTaskPoint_CreateInstance()
{
    return GetCTaskPointGeneratorSingleton()->m_allocator.ConstructCTaskPoint(0);
}

//----------------------------------------------------------------------------
//  0x6F27CE10 - InstanceGenerator<CTaskPoint>::DestroyInstance, slot 1
//  (+0x04).  `retn 4`, one stack argument (the object to tear down).
//----------------------------------------------------------------------------
void __stdcall InstanceGenerator_CTaskPoint_DestroyInstance(CTaskPoint* obj)
{
    obj->DeleteSelf(0);
    GetCTaskPointGeneratorSingleton()->m_allocator.Free(obj, 0, 0);
}

//----------------------------------------------------------------------------
//  0x6F27CE40 - InstanceGenerator<CTaskPoint>::ReleaseAll, slot 2 (+0x08).
//  `retn 0`, no arguments.
//----------------------------------------------------------------------------
void InstanceGenerator_CTaskPoint_ReleaseAll()
{
    GetCTaskPointGeneratorSingleton()->m_allocator.ReleaseAllBlocks(
        ".?AVCTaskPoint@@", -2, 1);
}

//----------------------------------------------------------------------------
//  0x6F28EED0 - CDataAllocator::ConstructCTaskPoint.  THUNK: a real
//  `__except_handler4`-shaped SEH frame this repo's fixed `/GS- /EHs-c-`
//  toolchain cannot emit (see dataallocator.h's own comment on the
//  declaration, and taskpoint.h's header comment for what the real body
//  does).  Declared here, in this address neighbourhood, rather than in
//  dataallocator.cpp - same "one translation unit per original module"
//  reasoning taskinstancegenerator.cpp already gives for ConstructCTask.
//----------------------------------------------------------------------------
#define TASKPOINT_CTOR_THUNK(addr)  \
    {                               \
        __asm { mov eax, addr }     \
        __asm { jmp eax }           \
    }

__declspec(naked) CTaskPoint* CDataAllocator::ConstructCTaskPoint(int /*zeroMemory*/) TASKPOINT_CTOR_THUNK(0x6F28EED0)
