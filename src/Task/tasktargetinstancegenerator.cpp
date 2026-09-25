//============================================================================
//  InstanceGenerator<CTaskTarget>'s own construction path and its three real
//  vtable-slot bodies (`??_7?$InstanceGenerator@VCTaskTarget@@@@6B@` @
//  0x6F92FA04, agent_worktrees/classes/0x6F92FA04.json) - mirrored directly
//  from taskinstancegenerator.cpp (InstanceGenerator<CTask>, the reference
//  implementation for this whole family).
//
//  None of the three is spelled `virtual` here, for this repo's usual
//  reason: nothing in the currently-reachable data ever calls through this
//  vtable indirectly, so there is no evidence-backed reason to model
//  polymorphic dispatch that would just be guessed at.
//
//  Four addresses:
//    0x6F291C80 - InstanceGenerator<CTaskTarget>::CreateInstance   (slot 0)
//    0x6F27D020 - InstanceGenerator<CTaskTarget>::DestroyInstance  (slot 1)
//    0x6F27D050 - InstanceGenerator<CTaskTarget>::ReleaseAll       (slot 2)
//    0x6F28F050 - CDataAllocator::ConstructCTaskTarget (THUNK - see
//                 dataallocator.h's own comment for why: a real
//                 __except_handler4-shaped SEH frame this toolchain cannot
//                 emit)
//============================================================================
#include "tasktarget.h"
#include "ctasktargetinstancegenerator.h"

//----------------------------------------------------------------------------
//  0x6F291C80 - InstanceGenerator<CTaskTarget>::CreateInstance, slot 0
//  (+0x00).  `retn 0`, no arguments: hand the generator's own embedded
//  allocator a hard-coded `zeroMemory=0` and let it Alloc-and-construct a
//  fresh CTaskTarget.
//----------------------------------------------------------------------------
CTaskTarget* InstanceGenerator_CTaskTarget_CreateInstance()
{
    return GetCTaskTargetGeneratorSingleton()->m_allocator.ConstructCTaskTarget(0);
}

//----------------------------------------------------------------------------
//  0x6F27D020 - InstanceGenerator<CTaskTarget>::DestroyInstance, slot 1
//  (+0x04).  `retn 4`, one stack argument (the object to tear down), no
//  register carries anything in - `__stdcall` matches both: call its own
//  (real override, tasktarget.h) virtual DeleteSelf(0), then free the block
//  back to the same pool with the tag/line the shipped call site pushes
//  (0, 0), matching CDataAllocator::Free's own established defaulting.
//----------------------------------------------------------------------------
void __stdcall InstanceGenerator_CTaskTarget_DestroyInstance(CTaskTarget* obj)
{
    obj->DeleteSelf(0);
    GetCTaskTargetGeneratorSingleton()->m_allocator.Free(obj, 0, 0);
}

//----------------------------------------------------------------------------
//  0x6F27D050 - InstanceGenerator<CTaskTarget>::ReleaseAll, slot 2 (+0x08).
//  `retn 0`, no arguments: release every block the pool has ever grown by
//  back to Storm, tagged with CTaskTarget's own RTTI name.
//----------------------------------------------------------------------------
void InstanceGenerator_CTaskTarget_ReleaseAll()
{
    GetCTaskTargetGeneratorSingleton()->m_allocator.ReleaseAllBlocks(".?AVCTaskTarget@@", -2, 1);
}

//----------------------------------------------------------------------------
//  0x6F28F050 - CDataAllocator::ConstructCTaskTarget.  THUNK: a real
//  `__except_handler4`-shaped SEH frame this repo's fixed `/GS- /EHs-c-`
//  toolchain cannot emit (see dataallocator.h's own comment on the
//  declaration, and tasktarget.h's header comment for exactly what the real
//  body does).  Declared here, in this address neighbourhood, rather than
//  in dataallocator.cpp, because 0x6F28F050 shipped with the rest of this
//  0x6F28xxxx CTaskTarget module, not with CDataAllocator's own 0x6F4C1xxx
//  one - the same "one translation unit per original module" reasoning that
//  keeps ConstructCTask's own definition in taskinstancegenerator.cpp
//  instead of dataallocator.cpp.
//----------------------------------------------------------------------------
#define CTASKTARGET_CTOR_THUNK(addr) \
    {                                \
        __asm { mov eax, addr }     \
        __asm { jmp eax }           \
    }

__declspec(naked) CTaskTarget* CDataAllocator::ConstructCTaskTarget(int /*zeroMemory*/) CTASKTARGET_CTOR_THUNK(0x6F28F050)
