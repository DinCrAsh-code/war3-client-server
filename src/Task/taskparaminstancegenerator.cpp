//============================================================================
//  InstanceGenerator<CTaskParam>'s own construction path and its three real
//  vtable-slot bodies (`??_7?$InstanceGenerator@VCTaskParam@@@@6B@` @
//  0x6F92F994, agent_worktrees/classes/0x6F92F994.json) - mirrors
//  taskinstancegenerator.cpp (InstanceGenerator<CTask>) exactly, the
//  reference implementation this whole InstanceGenerator<CTaskXxx> family
//  follows.
//
//  None of the three is spelled `virtual` here, for the same reason as
//  InstanceGenerator<CTask>'s own (taskinstancegenerator.cpp's own header
//  comment has the fullest version): nothing in the currently-reachable
//  data ever calls through this vtable indirectly.
//
//  Four addresses:
//    0x6F291BA0 - InstanceGenerator<CTaskParam>::CreateInstance   (slot 0)
//    0x6F27CB50 - InstanceGenerator<CTaskParam>::DestroyInstance  (slot 1)
//    0x6F27CB80 - InstanceGenerator<CTaskParam>::ReleaseAll       (slot 2)
//    0x6F28ECC0 - CDataAllocator::ConstructCTaskParam (THUNK - see
//                 dataallocator.h's own comment for why: a real
//                 __except_handler4-shaped SEH frame this toolchain cannot
//                 emit)
//============================================================================
#include "taskparam.h"
#include "ctaskparaminstancegenerator.h"

//----------------------------------------------------------------------------
//  0x6F291BA0 - InstanceGenerator<CTaskParam>::CreateInstance, slot 0
//  (+0x00).  `retn 0`, no arguments: hand the generator's own embedded
//  allocator a hard-coded `zeroMemory=0` and let it Alloc-and-construct a
//  fresh CTaskParam.
//----------------------------------------------------------------------------
CTaskParam* InstanceGenerator_CTaskParam_CreateInstance()
{
    return GetCTaskParamGeneratorSingleton()->m_allocator.ConstructCTaskParam(0);
}

//----------------------------------------------------------------------------
//  0x6F27CB50 - InstanceGenerator<CTaskParam>::DestroyInstance, slot 1
//  (+0x04).  `retn 4`, one stack argument (the object to tear down), no
//  register carries anything in - `__stdcall` matches both: call its own
//  (real override, taskparam.h) virtual DeleteSelf(0), then free the block
//  back to the same pool with the tag/line the shipped call site pushes
//  (0, 0), matching CDataAllocator::Free's own established defaulting.
//----------------------------------------------------------------------------
void __stdcall InstanceGenerator_CTaskParam_DestroyInstance(CTaskParam* obj)
{
    obj->DeleteSelf(0);
    GetCTaskParamGeneratorSingleton()->m_allocator.Free(obj, 0, 0);
}

//----------------------------------------------------------------------------
//  0x6F27CB80 - InstanceGenerator<CTaskParam>::ReleaseAll, slot 2 (+0x08).
//  `retn 0`, no arguments: release every block the pool has ever grown by
//  back to Storm, tagged with CTaskParam's own RTTI name.
//----------------------------------------------------------------------------
void InstanceGenerator_CTaskParam_ReleaseAll()
{
    GetCTaskParamGeneratorSingleton()->m_allocator.ReleaseAllBlocks(".?AVCTaskParam@@", -2, 1);
}

//----------------------------------------------------------------------------
//  0x6F28ECC0 - CDataAllocator::ConstructCTaskParam.  THUNK: a real
//  `__except_handler4`-shaped SEH frame this repo's fixed `/GS- /EHs-c-`
//  toolchain cannot emit (see dataallocator.h's own comment on the
//  declaration, and taskparam.h's header comment for exactly what the real
//  body does).  Declared here, in this address neighbourhood, rather than
//  in dataallocator.cpp, for the same "one translation unit per original
//  module" reasoning taskinstancegenerator.cpp's own comment gives for
//  ConstructCTask.
//----------------------------------------------------------------------------
#define CTASKPARAM_CTOR_THUNK(addr)   \
    {                                 \
        __asm { mov eax, addr }      \
        __asm { jmp eax }            \
    }

__declspec(naked) CTaskParam* CDataAllocator::ConstructCTaskParam(int /*zeroMemory*/) CTASKPARAM_CTOR_THUNK(0x6F28ECC0)
