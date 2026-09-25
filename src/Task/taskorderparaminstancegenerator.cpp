//============================================================================
//  InstanceGenerator<CTaskOrderParam>'s own construction path and its three
//  real vtable-slot bodies (`??_7?$InstanceGenerator@VCTaskOrderParam@@@@6B@`
//  @ 0x6F92F9C4, agent_worktrees/classes/0x6F92F9C4.json) - mirrors
//  taskparaminstancegenerator.cpp (InstanceGenerator<CTaskParam>) exactly,
//  the reference implementation this whole InstanceGenerator<CTaskXxx>
//  family follows.
//
//  None of the three is spelled `virtual` here, for the same reason as
//  InstanceGenerator<CTask>'s own (taskinstancegenerator.cpp's own header
//  comment has the fullest version): nothing in the currently-reachable
//  data ever calls through this vtable indirectly.
//
//  Four addresses:
//    0x6F291C00 - InstanceGenerator<CTaskOrderParam>::CreateInstance   (slot 0)
//    0x6F27CD60 - InstanceGenerator<CTaskOrderParam>::DestroyInstance  (slot 1)
//    0x6F27CD90 - InstanceGenerator<CTaskOrderParam>::ReleaseAll       (slot 2)
//    0x6F28EE50 - CDataAllocator::ConstructCTaskOrderParam (THUNK - see
//                 dataallocator.h's own comment for why: a real
//                 __except_handler4-shaped SEH frame this toolchain cannot
//                 emit)
//============================================================================
#include "taskorderparam.h"
#include "ctaskorderparaminstancegenerator.h"

//----------------------------------------------------------------------------
//  0x6F291C00 - InstanceGenerator<CTaskOrderParam>::CreateInstance, slot 0
//  (+0x00).  `retn 0`, no arguments: hand the generator's own embedded
//  allocator a hard-coded `zeroMemory=0` and let it Alloc-and-construct a
//  fresh CTaskOrderParam.
//----------------------------------------------------------------------------
CTaskOrderParam* InstanceGenerator_CTaskOrderParam_CreateInstance()
{
    return GetCTaskOrderParamGeneratorSingleton()->m_allocator.ConstructCTaskOrderParam(0);
}

//----------------------------------------------------------------------------
//  0x6F27CD60 - InstanceGenerator<CTaskOrderParam>::DestroyInstance, slot 1
//  (+0x04).  `retn 4`, one stack argument (the object to tear down), no
//  register carries anything in - `__stdcall` matches both: call its own
//  (real override, taskorderparam.h) virtual DeleteSelf(0), then free the
//  block back to the same pool with the tag/line the shipped call site
//  pushes (0, 0), matching CDataAllocator::Free's own established
//  defaulting.
//----------------------------------------------------------------------------
void __stdcall InstanceGenerator_CTaskOrderParam_DestroyInstance(CTaskOrderParam* obj)
{
    obj->DeleteSelf(0);
    GetCTaskOrderParamGeneratorSingleton()->m_allocator.Free(obj, 0, 0);
}

//----------------------------------------------------------------------------
//  0x6F27CD90 - InstanceGenerator<CTaskOrderParam>::ReleaseAll, slot 2
//  (+0x08).  `retn 0`, no arguments: release every block the pool has ever
//  grown by back to Storm, tagged with CTaskOrderParam's own RTTI name.
//----------------------------------------------------------------------------
void InstanceGenerator_CTaskOrderParam_ReleaseAll()
{
    GetCTaskOrderParamGeneratorSingleton()->m_allocator.ReleaseAllBlocks(".?AVCTaskOrderParam@@", -2, 1);
}

//----------------------------------------------------------------------------
//  0x6F28EE50 - CDataAllocator::ConstructCTaskOrderParam.  THUNK: a real
//  `__except_handler4`-shaped SEH frame this repo's fixed `/GS- /EHs-c-`
//  toolchain cannot emit (see dataallocator.h's own comment on the
//  declaration, and taskorderparam.h's header comment for exactly what the
//  real body does).  Declared here, in this address neighbourhood, rather
//  than in dataallocator.cpp, for the same "one translation unit per
//  original module" reasoning taskparaminstancegenerator.cpp's own comment
//  gives for ConstructCTaskParam.
//----------------------------------------------------------------------------
#define CTASKORDERPARAM_CTOR_THUNK(addr)   \
    {                                       \
        __asm { mov eax, addr }            \
        __asm { jmp eax }                  \
    }

__declspec(naked) CTaskOrderParam* CDataAllocator::ConstructCTaskOrderParam(int /*zeroMemory*/) CTASKORDERPARAM_CTOR_THUNK(0x6F28EE50)
