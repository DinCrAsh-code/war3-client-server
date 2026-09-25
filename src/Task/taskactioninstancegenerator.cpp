//============================================================================
//  InstanceGenerator<CTaskAction>'s own construction path and its three
//  real vtable-slot bodies (`??_7?$InstanceGenerator@VCTaskAction@@@@6B@` @
//  0x6F92F9B4, agent_worktrees/classes/0x6F92F9B4.json) - mirrors
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
//    0x6F291BE0 - InstanceGenerator<CTaskAction>::CreateInstance  (slot 0)
//    0x6F27CCB0 - InstanceGenerator<CTaskAction>::DestroyInstance (slot 1)
//    0x6F27CCE0 - InstanceGenerator<CTaskAction>::ReleaseAll      (slot 2)
//    0x6F28EDD0 - CDataAllocator::ConstructCTaskAction (THUNK - see
//                 dataallocator.h's own comment for why: a real
//                 __except_handler4-shaped SEH frame this toolchain cannot
//                 emit)
//============================================================================
#include "taskaction.h"
#include "ctaskactioninstancegenerator.h"

//----------------------------------------------------------------------------
//  0x6F291BE0 - InstanceGenerator<CTaskAction>::CreateInstance, slot 0
//  (+0x00).  `retn 0`, no arguments: hand the generator's own embedded
//  allocator a hard-coded `zeroMemory=0` and let it Alloc-and-construct a
//  fresh CTaskAction.
//----------------------------------------------------------------------------
CTaskAction* InstanceGenerator_CTaskAction_CreateInstance()
{
    return GetCTaskActionGeneratorSingleton()->m_allocator.ConstructCTaskAction(0);
}

//----------------------------------------------------------------------------
//  0x6F27CCB0 - InstanceGenerator<CTaskAction>::DestroyInstance, slot 1
//  (+0x04).  `retn 4`, one stack argument (the object to tear down), no
//  register carries anything in - `__stdcall` matches both: call its own
//  (real override, taskaction.h) virtual DeleteSelf(0), then free the block
//  back to the same pool with the tag/line the shipped call site pushes
//  (0, 0), matching CDataAllocator::Free's own established defaulting.
//----------------------------------------------------------------------------
void __stdcall InstanceGenerator_CTaskAction_DestroyInstance(CTaskAction* obj)
{
    obj->DeleteSelf(0);
    GetCTaskActionGeneratorSingleton()->m_allocator.Free(obj, 0, 0);
}

//----------------------------------------------------------------------------
//  0x6F27CCE0 - InstanceGenerator<CTaskAction>::ReleaseAll, slot 2 (+0x08).
//  `retn 0`, no arguments: release every block the pool has ever grown by
//  back to Storm, tagged with CTaskAction's own RTTI name.
//----------------------------------------------------------------------------
void InstanceGenerator_CTaskAction_ReleaseAll()
{
    GetCTaskActionGeneratorSingleton()->m_allocator.ReleaseAllBlocks(".?AVCTaskAction@@", -2, 1);
}

//----------------------------------------------------------------------------
//  0x6F28EDD0 - CDataAllocator::ConstructCTaskAction.  THUNK: a real
//  `__except_handler4`-shaped SEH frame this repo's fixed `/GS- /EHs-c-`
//  toolchain cannot emit (see dataallocator.h's own comment on the
//  declaration, and taskaction.h's header comment for exactly what the real
//  body does).  Declared here, in this address neighbourhood, rather than
//  in dataallocator.cpp, for the same "one translation unit per original
//  module" reasoning taskparaminstancegenerator.cpp's own comment gives for
//  ConstructCTaskParam.
//----------------------------------------------------------------------------
#define CTASKACTION_CTOR_THUNK(addr)  \
    {                                 \
        __asm { mov eax, addr }      \
        __asm { jmp eax }            \
    }

__declspec(naked) CTaskAction* CDataAllocator::ConstructCTaskAction(int /*zeroMemory*/) CTASKACTION_CTOR_THUNK(0x6F28EDD0)
