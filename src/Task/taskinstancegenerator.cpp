//============================================================================
//  InstanceGenerator<CTask>'s own construction path and its three real
//  vtable-slot bodies (`??_7?$InstanceGenerator@VCTask@@@@6B@` @
//  0x6F92F984, agent_worktrees/classes/0x6F92F984.json) - the reference
//  implementation for the whole InstanceGenerator<CTaskXxx> family, mirrored
//  directly from src/Jass/jassfunc.cpp (InstanceGenerator<CJassFunc>, the
//  first of this shape reconstructed anywhere in this repo).
//
//  None of the three is spelled `virtual` here, for this repo's usual
//  reason (agentwar3.h's own header comment gives the fullest version):
//  nothing in the currently-reachable data ever calls through this vtable
//  indirectly, so there is no evidence-backed reason to model polymorphic
//  dispatch that would just be guessed at. They are ordinary functions that
//  happen to sit at the three real vtable-slot addresses.
//
//  Four addresses:
//    0x6F291B80 - InstanceGenerator<CTask>::CreateInstance   (slot 0)
//    0x6F27CAA0 - InstanceGenerator<CTask>::DestroyInstance  (slot 1)
//    0x6F27CAD0 - InstanceGenerator<CTask>::ReleaseAll       (slot 2)
//    0x6F28EC40 - CDataAllocator::ConstructCTask (THUNK - see
//                 dataallocator.h's own comment for why: a real
//                 __except_handler4-shaped SEH frame this toolchain cannot
//                 emit)
//============================================================================
#include "task.h"
#include "ctaskinstancegenerator.h"

//----------------------------------------------------------------------------
//  0x6F291B80 - InstanceGenerator<CTask>::CreateInstance, slot 0 (+0x00).
//  `retn 0`, no arguments: hand the generator's own embedded allocator a
//  hard-coded `zeroMemory=0` and let it Alloc-and-construct a fresh CTask.
//----------------------------------------------------------------------------
CTask* InstanceGenerator_CTask_CreateInstance()
{
    return GetCTaskGeneratorSingleton()->m_allocator.ConstructCTask(0);
}

//----------------------------------------------------------------------------
//  0x6F27CAA0 - InstanceGenerator<CTask>::DestroyInstance, slot 1 (+0x04).
//  `retn 4`, one stack argument (the object to tear down), no register
//  carries anything in - `__stdcall` matches both: call its own (real
//  override, task.h) virtual DeleteSelf(0), then free the block back to
//  the same pool with the tag/line the shipped call site pushes (0, 0),
//  matching CDataAllocator::Free's own established defaulting.
//----------------------------------------------------------------------------
void __stdcall InstanceGenerator_CTask_DestroyInstance(CTask* obj)
{
    obj->DeleteSelf(0);
    GetCTaskGeneratorSingleton()->m_allocator.Free(obj, 0, 0);
}

//----------------------------------------------------------------------------
//  0x6F27CAD0 - InstanceGenerator<CTask>::ReleaseAll, slot 2 (+0x08).
//  `retn 0`, no arguments: release every block the pool has ever grown by
//  back to Storm, tagged with CTask's own RTTI name.
//----------------------------------------------------------------------------
void InstanceGenerator_CTask_ReleaseAll()
{
    GetCTaskGeneratorSingleton()->m_allocator.ReleaseAllBlocks(".?AVCTask@@", -2, 1);
}

//----------------------------------------------------------------------------
//  0x6F28EC40 - CDataAllocator::ConstructCTask.  THUNK: a real
//  `__except_handler4`-shaped SEH frame this repo's fixed `/GS- /EHs-c-`
//  toolchain cannot emit (see dataallocator.h's own comment on the
//  declaration, and task.h's header comment for exactly what the real body
//  does).  Declared here, in this address neighbourhood, rather than in
//  dataallocator.cpp, because 0x6F28EC40 shipped with the rest of this
//  0x6F28xxxx CTask module, not with CDataAllocator's own 0x6F4C1xxx one -
//  the same "one translation unit per original module" reasoning that
//  keeps ConstructCJassFunc's own definition in jassfunc.cpp instead of
//  dataallocator.cpp.
//----------------------------------------------------------------------------
#define CTASK_CTOR_THUNK(addr)   \
    {                            \
        __asm { mov eax, addr }  \
        __asm { jmp eax }        \
    }

__declspec(naked) CTask* CDataAllocator::ConstructCTask(int /*zeroMemory*/) CTASK_CTOR_THUNK(0x6F28EC40)
