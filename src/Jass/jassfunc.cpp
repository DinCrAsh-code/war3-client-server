//============================================================================
//  CJassFunc's own construction path and its InstanceGenerator<CJassFunc>
//  (0x6F942A1C, agent_worktrees/classes/0x6F942A1C.json) factory - see
//  src/Agent/jassfunc.h for the class itself and the correction to this
//  session's own starting premise about the constructor being
//  undiscoverable.  This is the FIRST InstanceGenerator<T>/
//  TInstanceAllocator<T> pair reconstructed anywhere in this repo (grepping
//  src/ for "InstanceGenerator"/"TInstanceAllocator" before this file
//  landed returns nothing but the type/dtor comments this file itself
//  adds) - there was no established sibling shape to reuse beyond the
//  generic CDataAllocator interface (dataallocator.h) every other pool in
//  this codebase already goes through.
//
//  All five addresses below sit in the same 0x6F3Bxxxx module (the
//  InstanceGenerator<CJassFunc> vtable itself is one module over, at
//  0x6F942A1C, and its own 3-slot body lives here rather than at that
//  address):
//    0x6F3B1470 - GetJassFuncGeneratorSingleton (2-instr leaf,
//                 jassfuncgeneratorsingleton.cpp - see that file/header for
//                 why it is split out)
//    0x6F3BCFF0 - InstanceGenerator<CJassFunc>::CreateInstance   (slot 0)
//    0x6F3B89A0 - InstanceGenerator<CJassFunc>::DestroyInstance  (slot 1)
//    0x6F3B89D0 - InstanceGenerator<CJassFunc>::ReleaseAll       (slot 2)
//    0x6F3B9CE0 - CDataAllocator::ConstructCJassFunc (THUNK - see
//                 dataallocator.h's own comment for why: a real
//                 `__except_handler4`-shaped SEH frame this toolchain
//                 cannot emit)
//
//  None of the three InstanceGenerator<CJassFunc> slots is spelled
//  `virtual` here, for this repo's usual reason (agentwar3.h's own header
//  comment gives the fullest version): nothing in the currently-reachable
//  data ever calls through this vtable indirectly - the static
//  initializer below is the only writer of the vtable pointer, and these
//  three reconstructions are the only readers of the singleton at all -
//  so there is no evidence-backed reason to model polymorphic dispatch
//  that would just be guessed at.  They are ordinary functions that
//  happen to sit at the three real vtable-slot addresses.
//============================================================================
#include "jassfunc.h"
#include "jassfuncinstancegenerator.h"

//----------------------------------------------------------------------------
//  0x6F3BCFF0 - InstanceGenerator<CJassFunc>::CreateInstance, slot 0
//  (+0x00).  `retn 0`, no arguments: hand the generator's own embedded
//  allocator a hard-coded `zeroMemory=0` and let it Alloc-and-construct a
//  fresh CJassFunc.
//----------------------------------------------------------------------------
CJassFunc* InstanceGenerator_CJassFunc_CreateInstance()
{
    return GetJassFuncGeneratorSingleton()->m_allocator.ConstructCJassFunc(0);
}

//----------------------------------------------------------------------------
//  0x6F3B89A0 - InstanceGenerator<CJassFunc>::DestroyInstance, slot 1
//  (+0x04).  `retn 4`, one stack argument (the object to tear down), no
//  register carries anything in - `__stdcall` (not the plain `__cdecl` a
//  free function defaults to, which never cleans its own stack) is the
//  calling convention that matches both: call its own (inherited, not
//  overridden here - see jassfunc.h) virtual DeleteSelf(0) - the shipped
//  body reads `[[obj]][4]` and calls through it exactly as an ordinary
//  virtual dispatch would - then free the block back to the same pool
//  with the tag/line the shipped call site pushes (0, 0), matching
//  CDataAllocator::Free's own established defaulting.
//----------------------------------------------------------------------------
void __stdcall InstanceGenerator_CJassFunc_DestroyInstance(CJassFunc* obj)
{
    obj->DeleteSelf(0);
    GetJassFuncGeneratorSingleton()->m_allocator.Free(obj, 0, 0);
}

//----------------------------------------------------------------------------
//  0x6F3B89D0 - InstanceGenerator<CJassFunc>::ReleaseAll, slot 2 (+0x08).
//  `retn 0`, no arguments: release every block the pool has ever grown by
//  back to Storm, tagged with CJassFunc's own RTTI name (matching
//  CDataAllocator::ReleaseAllBlocks' own established (tag, line, unused)
//  signature and the shipped call site's own three immediates).
//----------------------------------------------------------------------------
void InstanceGenerator_CJassFunc_ReleaseAll()
{
    GetJassFuncGeneratorSingleton()->m_allocator.ReleaseAllBlocks(".?AVCJassFunc@@", -2, 1);
}

//----------------------------------------------------------------------------
//  0x6F3B9CE0 - CDataAllocator::ConstructCJassFunc.  THUNK: a real
//  `__except_handler4`-shaped SEH frame this repo's fixed `/GS- /EHs-c-`
//  toolchain cannot emit (see dataallocator.h's own comment on the
//  declaration, and jassfunc.h's header comment for exactly what the real
//  body does).  Declared here, in this address neighbourhood, rather than
//  in dataallocator.cpp, because 0x6F3B9CE0 shipped with the rest of this
//  0x6F3Bxxxx JASS-VM module, not with CDataAllocator's own 0x6F4C1xxx one
//  - the same "one translation unit per original module" reasoning that
//  keeps ConstructCWidget's own definition in widget.cpp instead of here.
//----------------------------------------------------------------------------
#define JASSFUNC_THUNK(addr)     \
    {                            \
        __asm { mov eax, addr }  \
        __asm { jmp eax }        \
    }

__declspec(naked) CJassFunc* CDataAllocator::ConstructCJassFunc(int /*zeroMemory*/) JASSFUNC_THUNK(0x6F3B9CE0)
