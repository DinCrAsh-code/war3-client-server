//============================================================================
//  InstanceGenerator<CTaskPoint2>'s own construction path and its three
//  real vtable-slot bodies (`??_7?$InstanceGenerator@VCTaskPoint2@@@@6B@` @
//  0x6F92F9E4, agent_worktrees/classes/0x6F92F9E4.json) - mirrors
//  taskpointinstancegenerator.cpp (InstanceGenerator<CTaskPoint>) exactly,
//  same reasons for every shape choice below.
//
//  None of the three is spelled `virtual` here, for this repo's usual
//  reason: nothing in the currently-reachable data ever calls through this
//  vtable indirectly.
//
//  Four addresses:
//    0x6F291C40 - InstanceGenerator<CTaskPoint2>::CreateInstance   (slot 0)
//    0x6F27CEC0 - InstanceGenerator<CTaskPoint2>::DestroyInstance  (slot 1)
//    0x6F27CEF0 - InstanceGenerator<CTaskPoint2>::ReleaseAll       (slot 2)
//    0x6F28EF40 - CDataAllocator::ConstructCTaskPoint2 (THUNK - see
//                 dataallocator.h's own comment for why: a real
//                 __except_handler4-shaped SEH frame this toolchain cannot
//                 emit)
//============================================================================
#include "taskpoint2.h"
#include "ctaskpoint2instancegenerator.h"

//----------------------------------------------------------------------------
//  0x6F291C40 - InstanceGenerator<CTaskPoint2>::CreateInstance, slot 0
//  (+0x00).  `retn 0`, no arguments.
//----------------------------------------------------------------------------
CTaskPoint2* InstanceGenerator_CTaskPoint2_CreateInstance()
{
    return GetCTaskPoint2GeneratorSingleton()->m_allocator.ConstructCTaskPoint2(0);
}

//----------------------------------------------------------------------------
//  0x6F27CEC0 - InstanceGenerator<CTaskPoint2>::DestroyInstance, slot 1
//  (+0x04).  `retn 4`, one stack argument (the object to tear down).
//----------------------------------------------------------------------------
void __stdcall InstanceGenerator_CTaskPoint2_DestroyInstance(CTaskPoint2* obj)
{
    obj->DeleteSelf(0);
    GetCTaskPoint2GeneratorSingleton()->m_allocator.Free(obj, 0, 0);
}

//----------------------------------------------------------------------------
//  0x6F27CEF0 - InstanceGenerator<CTaskPoint2>::ReleaseAll, slot 2 (+0x08).
//  `retn 0`, no arguments.
//----------------------------------------------------------------------------
void InstanceGenerator_CTaskPoint2_ReleaseAll()
{
    GetCTaskPoint2GeneratorSingleton()->m_allocator.ReleaseAllBlocks(
        ".?AVCTaskPoint2@@", -2, 1);
}

//----------------------------------------------------------------------------
//  0x6F28EF40 - CDataAllocator::ConstructCTaskPoint2.  THUNK: a real
//  `__except_handler4`-shaped SEH frame this repo's fixed `/GS- /EHs-c-`
//  toolchain cannot emit (see dataallocator.h's own comment on the
//  declaration, and taskpoint2.h's header comment for what the real body
//  does).  Declared here, in this address neighbourhood, rather than in
//  dataallocator.cpp - same "one translation unit per original module"
//  reasoning taskpointinstancegenerator.cpp already gives for
//  ConstructCTaskPoint.
//----------------------------------------------------------------------------
#define TASKPOINT2_CTOR_THUNK(addr)  \
    {                               \
        __asm { mov eax, addr }     \
        __asm { jmp eax }           \
    }

__declspec(naked) CTaskPoint2* CDataAllocator::ConstructCTaskPoint2(int /*zeroMemory*/) TASKPOINT2_CTOR_THUNK(0x6F28EF40)
