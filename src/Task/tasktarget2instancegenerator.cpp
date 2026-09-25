//============================================================================
//  InstanceGenerator<CTaskTarget2>'s own construction path and its three
//  real vtable-slot bodies (`??_7?$InstanceGenerator@VCTaskTarget2@@@@6B@` @
//  0x6F92FA14, agent_worktrees/classes/0x6F92FA14.json) - mirrored directly
//  from tasktargetinstancegenerator.cpp (InstanceGenerator<CTaskTarget>,
//  the immediate template sibling this file mirrors).
//
//  None of the three is spelled `virtual` here, for this repo's usual
//  reason: nothing in the currently-reachable data ever calls through this
//  vtable indirectly, so there is no evidence-backed reason to model
//  polymorphic dispatch that would just be guessed at.
//
//  Four addresses:
//    0x6F291CA0 - InstanceGenerator<CTaskTarget2>::CreateInstance   (slot 0)
//    0x6F27D0D0 - InstanceGenerator<CTaskTarget2>::DestroyInstance  (slot 1)
//    0x6F27D100 - InstanceGenerator<CTaskTarget2>::ReleaseAll       (slot 2)
//    0x6F28F0F0 - CDataAllocator::ConstructCTaskTarget2 (THUNK - see
//                 dataallocator.h's own comment for why: a real
//                 __except_handler4-shaped SEH frame this toolchain cannot
//                 emit, confirmed directly by reading sub_6F28F0F0's own
//                 raw_asm - `push -1 / push offset SEH_6F28F0F0 / mov
//                 eax, fs:0 / ...`)
//============================================================================
#include "tasktarget2.h"
#include "ctasktarget2instancegenerator.h"

//----------------------------------------------------------------------------
//  0x6F291CA0 - InstanceGenerator<CTaskTarget2>::CreateInstance, slot 0
//  (+0x00).  `retn 0`, no arguments: hand the generator's own embedded
//  allocator a hard-coded `zeroMemory=0` and let it Alloc-and-construct a
//  fresh CTaskTarget2.
//----------------------------------------------------------------------------
CTaskTarget2* InstanceGenerator_CTaskTarget2_CreateInstance()
{
    return GetCTaskTarget2GeneratorSingleton()->m_allocator.ConstructCTaskTarget2(0);
}

//----------------------------------------------------------------------------
//  0x6F27D0D0 - InstanceGenerator<CTaskTarget2>::DestroyInstance, slot 1
//  (+0x04).  `retn 4`, one stack argument (the object to tear down), no
//  register carries anything in - `__stdcall` matches both: call its own
//  (real override, tasktarget2.h) virtual DeleteSelf(0), then free the
//  block back to the same pool with the tag/line the shipped call site
//  pushes (0, 0), matching CDataAllocator::Free's own established
//  defaulting.
//----------------------------------------------------------------------------
void __stdcall InstanceGenerator_CTaskTarget2_DestroyInstance(CTaskTarget2* obj)
{
    obj->DeleteSelf(0);
    GetCTaskTarget2GeneratorSingleton()->m_allocator.Free(obj, 0, 0);
}

//----------------------------------------------------------------------------
//  0x6F27D100 - InstanceGenerator<CTaskTarget2>::ReleaseAll, slot 2
//  (+0x08).  `retn 0`, no arguments: release every block the pool has ever
//  grown by back to Storm, tagged with CTaskTarget2's own RTTI name.
//----------------------------------------------------------------------------
void InstanceGenerator_CTaskTarget2_ReleaseAll()
{
    GetCTaskTarget2GeneratorSingleton()->m_allocator.ReleaseAllBlocks(".?AVCTaskTarget2@@", -2, 1);
}

//----------------------------------------------------------------------------
//  0x6F28F0F0 - CDataAllocator::ConstructCTaskTarget2.  THUNK: a real
//  `__except_handler4`-shaped SEH frame this repo's fixed `/GS- /EHs-c-`
//  toolchain cannot emit (see dataallocator.h's own comment on the
//  declaration).  Declared here, in this address neighbourhood, rather than
//  in dataallocator.cpp, because 0x6F28F0F0 shipped with the rest of this
//  0x6F28xxxx CTaskTarget2 module, not with CDataAllocator's own 0x6F4C1xxx
//  one - the same "one translation unit per original module" reasoning that
//  keeps ConstructCTaskTarget's own definition in
//  tasktargetinstancegenerator.cpp instead of dataallocator.cpp.
//----------------------------------------------------------------------------
#define CTASKTARGET2_CTOR_THUNK(addr) \
    {                                 \
        __asm { mov eax, addr }      \
        __asm { jmp eax }            \
    }

__declspec(naked) CTaskTarget2* CDataAllocator::ConstructCTaskTarget2(int /*zeroMemory*/) CTASKTARGET2_CTOR_THUNK(0x6F28F0F0)
